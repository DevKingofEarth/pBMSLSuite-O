// pBMSLSuite-O - Portable BMS Lab Suite
// Energy-Efficient, Portable, Predictive BMS for National Security
// Based on Wokwi Circuit Design

// I2C Libraries (for INA219 current sensor)
// #include <Wire.h>
// #include <Adafruit_INA219.h>  // Install: Sketch → Include Library → Manage Libraries → Search "Adafruit INA219"

// Pin Definitions
#define CELL1_PIN    34    // Potentiometer 1
#define CELL2_PIN    35    // Potentiometer 2
#define CELL3_PIN    25    // Potentiometer 3
#define CELL4_PIN    26    // Potentiometer 4
#define TEMP_PIN     33    // NTC Thermistor
#define BALANCE1_PIN 16    // Cell 1 balance resistor
#define BALANCE2_PIN 17    // Cell 2 balance resistor
#define BALANCE3_PIN 18    // Cell 3 balance resistor
#define BALANCE4_PIN 19    // Cell 4 balance resistor
#define STATUS_LED_PIN 2   // BMS state indication

// NTC Thermistor Configuration (Steinhart-Hart)
#define NTC_NOMINAL_RESISTANCE 10000.0f
#define NTC_BETA_COEFFICIENT 3950.0f
#define NTC_NOMINAL_TEMP 25.0f
#define NTC_VOLTAGE_DIVIDER 10000.0f
#define ADC_MAX_VALUE 4095.0f
#define V_REF 3.3f

// Safety Thresholds
#define OV_THRESHOLD 4.25f    // Over-voltage
#define UV_THRESHOLD 2.8f     // Under-voltage
#define OT_THRESHOLD 60.0f    // Over-temperature
#define UT_THRESHOLD 0.0f     // Under-temperature
#define BALANCE_THRESHOLD 4.1f // Balance trigger

// Voltage Divider Configuration
// For real hardware: Use voltage dividers to measure 0-5V on 0-3.3V ADC
// Set USE_VOLTAGE_DIVIDER = 1 for hardware, 0 for Wokwi simulation
#define USE_VOLTAGE_DIVIDER 0

// Voltage Divider Parameters (if enabled)
// Hardware: R1=10kΩ (top), R2=15kΩ (bottom)
// V_measured = V_in * (R2 / (R1 + R2)) = V_in * (15k / 25k) = V_in * 0.6
// Scaling factor: 1.0 / 0.6 = 1.667
#define VOLTAGE_DIVIDER_RATIO 0.6f
#define VOLTAGE_DIVIDER_SCALE (1.0f / VOLTAGE_DIVIDER_RATIO)  // 1.667

// Current Sensor (INA219) Configuration
// Set USE_INA219 = 1 to enable current sensing via I2C
// INA219 provides: Bus voltage, Shunt current, Power
#define USE_INA219 0

// INA219 I2C Configuration
#define INA219_ADDRESS 0x40  // Default I2C address (A0=GND, A1=GND)
#define INA219_MAX_CURRENT_AMPS 3.2f  // 3.2A max (typical for battery)
#define INA219_SHUNT_OHMS 0.1f  // 0.1Ω shunt resistor

// Global INA219 Instance (will be initialized if enabled)
// Adafruit_INA219 ina219(INA219_ADDRESS);

// Moving Average Filter
struct MovingAverageFilter {
    float buffer[5] = {0};
    int index = 0;
    float sum = 0.0f;
    int count = 0;
    
    float add(float value) {
        sum = sum - buffer[index] + value;
        buffer[index] = value;
        index = (index + 1) % 5;
        if (count < 5) count++;
        return sum / count;
    }
    
    void reset() {
        for (int i = 0; i < 5; i++) buffer[i] = 0.0f;
        sum = 0.0f;
        index = 0;
        count = 0;
    }
};

// BMS State Machine
enum BMSState { IDLE, CHARGING, DISCHARGING, BALANCING, FAULT };

// Fault Types
enum FaultType { FAULT_NONE, FAULT_OV, FAULT_UV, FAULT_OT, FAULT_UT };

// Global Instances
MovingAverageFilter cellFilters[4];
MovingAverageFilter tempFilter;
float cellVoltages[4] = {0};
float temperature = 25.0f;
float loadCurrent = 0.0f;
float busVoltage = 0.0f;      // From INA219 (pack voltage)
float shuntCurrent = 0.0f;    // From INA219 (charge/discharge current)
float busPower = 0.0f;        // From INA219 (power dissipation)
float estimatedSoC = 100.0f;
float estimatedSoH = 100.0f;
BMSState currentState = IDLE;
FaultType currentFault = FAULT_NONE;
unsigned long lastCSVTime = 0;
unsigned long lastSoCUpdate = 0;

// Fault Injection Testing
bool faultInjectionEnabled = false;
FaultType injectedFault = FAULT_NONE;
float injectedValue = 0.0f;
unsigned long injectionStartTime = 0;
unsigned long injectionDuration = 0;
int faultSampleCount = 0;
const int FAULT_DEBOUNCE_SAMPLES = 3;

// INA219 Status
bool ina219Initialized = false;
bool ina219Error = false;

// Simple spike filter
float filterSpike(float current, float last) {
    if (last == 0) return current;
    return abs(current - last) < 0.5f ? current : last;
}

class PBMSController {
public:
    void init() {
         // Initialize ADC pins
         pinMode(CELL1_PIN, INPUT);
         pinMode(CELL2_PIN, INPUT);
         pinMode(CELL3_PIN, INPUT);
         pinMode(CELL4_PIN, INPUT);
         pinMode(TEMP_PIN, INPUT);
         
         // Initialize balancing outputs
         pinMode(BALANCE1_PIN, OUTPUT);
         pinMode(BALANCE2_PIN, OUTPUT);
         pinMode(BALANCE3_PIN, OUTPUT);
         pinMode(BALANCE4_PIN, OUTPUT);
         
         // Initialize status LED
         pinMode(STATUS_LED_PIN, OUTPUT);
         
         Serial.begin(115200);
         Serial.println("pBMSLSuite-O v1.0 - Portable BMS Lab Suite");
         Serial.println("==========================================");
         
         // Display voltage measurement configuration
         if (USE_VOLTAGE_DIVIDER) {
             Serial.println("⚙️  VOLTAGE MODE: Hardware Dividers (0-5V range)");
             Serial.printf("    R1=10kΩ, R2=15kΩ, Ratio=%.2f, Scale=%.3f\n", 
                          VOLTAGE_DIVIDER_RATIO, VOLTAGE_DIVIDER_SCALE);
         } else {
             Serial.println("⚙️  VOLTAGE MODE: Direct ADC (0-3.3V range - Wokwi Simulation)");
         }
         
         // Initialize INA219 Current Sensor if enabled
         if (USE_INA219) {
             initializeINA219();
         } else {
             Serial.println("⚙️  CURRENT SENSOR: Disabled (load current from slider pot)");
         }
         
         Serial.println();
     }
     
     void initializeINA219() {
         #if USE_INA219
             // Uncomment these lines when Adafruit_INA219 library is installed:
             // Wire.begin();
             // if (ina219.begin()) {
             //     ina219.setCalibration_32V_3A();  // 32V max, 3A max
             //     ina219Initialized = true;
             //     Serial.println("⚙️  CURRENT SENSOR: INA219 initialized via I2C");
             //     Serial.printf("    Address: 0x%02X, Shunt: %.1fΩ\n", INA219_ADDRESS, INA219_SHUNT_OHMS);
             // } else {
             //     ina219Error = true;
             //     Serial.println("⚠️  CURRENT SENSOR: INA219 not found on I2C bus!");
             // }
             Serial.println("⚙️  CURRENT SENSOR: INA219 support compiled (library install required)");
         #endif
     }
    
    void run() {
         // Read sensors
         readSensors();
         
         // Handle fault injection for testing
         handleFaultInjection();
         
         // Update SoC estimation
         updateSoC();
         
         // State machine
         updateStateMachine();
         
         // Fault detection
         checkFaults();
         
         // CSV logging (1Hz)
         if (millis() - lastCSVTime > 1000) {
             logCSV();
             lastCSVTime = millis();
         }
         
         delay(100);  // 10Hz loop
     }
     
     // ============ Fault Injection Methods ============
     void enableFaultInjection() {
         faultInjectionEnabled = true;
         Serial.println("\n✅ Fault Injection Mode ENABLED");
         Serial.println("Commands: INJECT OV/UV/OT/UT, DISABLE INJECTION, CLEAR FAULT");
     }
     
     void disableFaultInjection() {
         faultInjectionEnabled = false;
         injectedFault = FAULT_NONE;
         Serial.println("\n✅ Fault Injection Mode DISABLED");
     }
     
     void injectFault(FaultType faultType, float value, unsigned long durationMs) {
         if (!faultInjectionEnabled) {
             Serial.println("⚠️  Fault injection not enabled. Use: ENABLE INJECTION");
             return;
         }
         
         injectedFault = faultType;
         injectedValue = value;
         injectionStartTime = millis();
         injectionDuration = durationMs;
         faultSampleCount = 0;
         
         Serial.print("🔬 Fault Injected: ");
         printFaultName(faultType);
         Serial.printf(" (Value: %.2f, Duration: %lu ms)\n", value, durationMs);
     }
     
     void handleFaultInjection() {
         if (!faultInjectionEnabled || injectedFault == FAULT_NONE) return;
         
         // Check if injection duration expired
         if (injectionDuration > 0) {
             if (millis() - injectionStartTime > injectionDuration) {
                 injectedFault = FAULT_NONE;
                 faultSampleCount = 0;
                 Serial.println("✅ Injected fault cleared (duration expired)");
                 return;
             }
         }
         
         // Apply injected values
         if (injectedFault == FAULT_OV || injectedFault == FAULT_UV) {
             // Override cell voltage for testing
             for (int i = 0; i < 4; i++) {
                 cellVoltages[i] = injectedValue;
             }
         } else if (injectedFault == FAULT_OT || injectedFault == FAULT_UT) {
             // Override temperature for testing
             temperature = injectedValue;
         }
     }
     
     void clearInjectedFault() {
         if (faultInjectionEnabled) {
             injectedFault = FAULT_NONE;
             faultSampleCount = 0;
             Serial.println("✅ Injected fault manually cleared");
         }
     }
    
private:
    void readSensors() {
         // Read cell voltages with filtering
         float rawV1 = analogRead(CELL1_PIN) * (V_REF / ADC_MAX_VALUE);
         float rawV2 = analogRead(CELL2_PIN) * (V_REF / ADC_MAX_VALUE);
         float rawV3 = analogRead(CELL3_PIN) * (V_REF / ADC_MAX_VALUE);
         float rawV4 = analogRead(CELL4_PIN) * (V_REF / ADC_MAX_VALUE);
         
         // Apply voltage divider correction if enabled (for real hardware)
         if (USE_VOLTAGE_DIVIDER) {
             rawV1 *= VOLTAGE_DIVIDER_SCALE;
             rawV2 *= VOLTAGE_DIVIDER_SCALE;
             rawV3 *= VOLTAGE_DIVIDER_SCALE;
             rawV4 *= VOLTAGE_DIVIDER_SCALE;
         }
         
         cellVoltages[0] = cellFilters[0].add(rawV1);
         cellVoltages[1] = cellFilters[1].add(rawV2);
         cellVoltages[2] = cellFilters[2].add(rawV3);
         cellVoltages[3] = cellFilters[3].add(rawV4);
         
         // Read temperature using Steinhart-Hart
         float tempVoltage = analogRead(TEMP_PIN) * (V_REF / ADC_MAX_VALUE);
         temperature = calculateTemperatureNTC(tempVoltage);
         
         // Read current from INA219 if available
         readCurrentSensor();
     }
     
     void readCurrentSensor() {
         if (USE_INA219 && ina219Initialized && !ina219Error) {
             // Placeholder for actual INA219 reading
             // Uncomment when library is available:
             // busVoltage = ina219.getBusVoltage_V();
             // shuntCurrent = ina219.getCurrent_mA() / 1000.0f;  // Convert mA to A
             // busPower = ina219.getPower_mW() / 1000.0f;  // Convert mW to W
         }
     }
    
    float calculateTemperatureNTC(float voltage) {
        if (voltage >= V_REF - 0.01f) return -273.15f;
        
        float thermistorR = NTC_VOLTAGE_DIVIDER * (voltage / (V_REF - voltage));
        float steinhart = thermistorR / NTC_NOMINAL_RESISTANCE;
        steinhart = log(steinhart);
        steinhart /= NTC_BETA_COEFFICIENT;
        steinhart += 1.0f / (NTC_NOMINAL_TEMP + 273.15f);
        steinhart = 1.0f / steinhart;
        
        return steinhart - 273.15f;
    }
    
    void updateSoC() {
        float avgVoltage = (cellVoltages[0] + cellVoltages[1] + cellVoltages[2] + cellVoltages[3]) / 4.0f;
        
        // Simple voltage-based SoC estimation
        if (avgVoltage >= 4.20f) estimatedSoC = 100.0f;
        else if (avgVoltage <= 3.00f) estimatedSoC = 0.0f;
        else {
            estimatedSoC = ((avgVoltage - 3.00f) / 1.20f) * 100.0f;
        }
        
        // Simple SoH estimation (degrades with extreme temps)
        if (temperature > 45.0f || temperature < 10.0f) {
            estimatedSoH -= 0.01f;  // Degrade slowly
        }
        estimatedSoH = constrain(estimatedSoH, 0.0f, 100.0f);
    }
    
    void updateStateMachine() {
        float maxCell = cellVoltages[0];
        float minCell = cellVoltages[0];
        
        for (int i = 1; i < 4; i++) {
            maxCell = max(maxCell, cellVoltages[i]);
            minCell = min(minCell, cellVoltages[i]);
        }
        
        // State transitions
        switch (currentState) {
            case IDLE:
                if (currentFault != FAULT_NONE) {
                    currentState = FAULT;
                } else {
                    bool needsBalancing = false;
                    for (int i = 0; i < 4; i++) {
                        if (cellVoltages[i] > BALANCE_THRESHOLD) needsBalancing = true;
                    }
                    if (needsBalancing) currentState = BALANCING;
                }
                break;
                
            case BALANCING:
                // Check if balancing still needed
                {
                    bool needsBalancing = false;
                    for (int i = 0; i < 4; i++) {
                        if (cellVoltages[i] > BALANCE_THRESHOLD) needsBalancing = true;
                    }
                    if (!needsBalancing) currentState = IDLE;
                    if (currentFault != FAULT_NONE) currentState = FAULT;
                }
                break;
                
            case FAULT:
                // Auto-recovery when fault clears
                if (currentFault == FAULT_NONE) currentState = IDLE;
                break;
        }
        
        // Control balancing
        digitalWrite(BALANCE1_PIN, (cellVoltages[0] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
        digitalWrite(BALANCE2_PIN, (cellVoltages[1] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
        digitalWrite(BALANCE3_PIN, (cellVoltages[2] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
        digitalWrite(BALANCE4_PIN, (cellVoltages[3] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
        
        // Status LED
        digitalWrite(STATUS_LED_PIN, currentState == FAULT ? (millis() % 500 < 250 ? HIGH : LOW) : HIGH);
    }
    
    void checkFaults() {
        float maxCell = cellVoltages[0];
        for (int i = 1; i < 4; i++) maxCell = max(maxCell, cellVoltages[i]);
        
        // Check for faults
        if (maxCell > OV_THRESHOLD) currentFault = FAULT_OV;
        else if (maxCell < UV_THRESHOLD) currentFault = FAULT_UV;
        else if (temperature > OT_THRESHOLD) currentFault = FAULT_OT;
        else if (temperature < UT_THRESHOLD) currentFault = FAULT_UT;
        else currentFault = FAULT_NONE;
    }
    
    void logCSV() {
        // Human-readable formatted output
        Serial.println();
        Serial.println("═══════════════════════════════════════════════════════════");
        Serial.println("                    pBMSLSuite-O STATUS                     ");
        Serial.println("═══════════════════════════════════════════════════════════");
        
        // Timestamp
        Serial.print("⏱️  Time: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        // Cell Voltages
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.println("🔋 CELL VOLTAGES:");
        Serial.printf("   Cell 1: %.3fV %s\n", cellVoltages[0], getVoltageStatus(cellVoltages[0]));
        Serial.printf("   Cell 2: %.3fV %s\n", cellVoltages[1], getVoltageStatus(cellVoltages[1]));
        Serial.printf("   Cell 3: %.3fV %s\n", cellVoltages[2], getVoltageStatus(cellVoltages[2]));
        Serial.printf("   Cell 4: %.3fV %s\n", cellVoltages[3], getVoltageStatus(cellVoltages[3]));
        
         float avgVoltage = (cellVoltages[0] + cellVoltages[1] + cellVoltages[2] + cellVoltages[3]) / 4.0f;
         Serial.printf("   Average: %.3fV | Pack: %.2fV\n", avgVoltage, avgVoltage * 4);
         
         // Current Information (if available)
         if (USE_INA219 && ina219Initialized && !ina219Error) {
             Serial.println("───────────────────────────────────────────────────────────");
             Serial.println("⚡ CURRENT INFORMATION:");
             Serial.printf("   Bus Voltage: %.2fV | Current: %.3fA | Power: %.2fW\n", 
                          busVoltage, shuntCurrent, busPower);
         }
        
        // Temperature
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.print("🌡️  Temperature: ");
        Serial.print(temperature, 1);
        Serial.print("°C ");
        Serial.println(getTempStatus());
        
        // Battery Status
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.println("🔋 BATTERY STATUS:");
        Serial.printf("   SoC: %.1f%% | SoH: %.1f%%\n", estimatedSoC, estimatedSoH);
        
        // State Machine
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.print("⚙️  STATE: ");
        Serial.print(getStateIcon());
        printStateName();
        Serial.println();
        
        // Balancing Status
        Serial.print("   Balancing: ");
        bool balancing = digitalRead(BALANCE1_PIN) || digitalRead(BALANCE2_PIN) || 
                        digitalRead(BALANCE3_PIN) || digitalRead(BALANCE4_PIN);
        Serial.println(balancing ? "🔄 ACTIVE" : "⏸️  IDLE");
        
        // Fault Status
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.print("⚠️  FAULT: ");
        if (currentFault == FAULT_NONE) {
            Serial.println("✅ NONE");
        } else {
            Serial.print("❌ ");
            printFaultName();
            Serial.println();
        }
        
        // Thresholds
        Serial.println("───────────────────────────────────────────────────────────");
        Serial.println("📊 THRESHOLDS:");
        Serial.printf("   OV: %.2fV | UV: %.2fV | OT: %.0f°C | UT: %.0f°C\n", 
                     OV_THRESHOLD, UV_THRESHOLD, OT_THRESHOLD, UT_THRESHOLD);
        
        Serial.println("═══════════════════════════════════════════════════════════");
    }
    
    const char* getVoltageStatus(float voltage) {
        if (voltage > OV_THRESHOLD) return "⚠️ OV";
        if (voltage < UV_THRESHOLD) return "⚠️ UV";
        if (voltage > BALANCE_THRESHOLD) return "🔋 CHARGED";
        if (voltage < 3.0f) return "🔋 LOW";
        return "✅ OK";
    }
    
    const char* getTempStatus() {
        if (temperature > OT_THRESHOLD) return "⚠️ HOT";
        if (temperature < UT_THRESHOLD) return "⚠️ COLD";
        if (temperature > 45.0f) return "⚠️ WARM";
        return "✅ OK";
    }
    
    const char* getStateIcon() {
        switch (currentState) {
            case IDLE: return "🟢";
            case CHARGING: return "🔵";
            case DISCHARGING: return "🟡";
            case BALANCING: return "🟠";
            case FAULT: return "🔴";
            default: return "⚪";
        }
    }
    
    void printStateName() {
        switch (currentState) {
            case IDLE: Serial.print("IDLE (Ready)"); break;
            case CHARGING: Serial.print("CHARGING"); break;
            case DISCHARGING: Serial.print("DISCHARGING"); break;
            case BALANCING: Serial.print("BALANCING"); break;
            case FAULT: Serial.print("FAULT - ACTION REQUIRED!"); break;
        }
    }
    
    void printFaultName() {
         printFaultName(currentFault);
     }
     
     void printFaultName(FaultType faultType) {
         switch (faultType) {
             case FAULT_OV: Serial.print("OVER-VOLTAGE (>4.25V)"); break;
             case FAULT_UV: Serial.print("UNDER-VOLTAGE (<2.8V)"); break;
             case FAULT_OT: Serial.print("OVER-TEMPERATURE (>60°C)"); break;
             case FAULT_UT: Serial.print("UNDER-TEMPERATURE (<0°C)"); break;
             default: Serial.print("NONE");
         }
     }
};

PBMSController bmsController;

// Serial command processing
void processSerialCommand() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        if (command == "ENABLE INJECTION") {
            bmsController.enableFaultInjection();
        } 
        else if (command == "DISABLE INJECTION") {
            bmsController.disableFaultInjection();
        }
        else if (command.startsWith("INJECT")) {
            if (command.indexOf("OV") > 0) {
                bmsController.injectFault(FAULT_OV, 4.5f, 5000);
            }
            else if (command.indexOf("UV") > 0) {
                bmsController.injectFault(FAULT_UV, 2.5f, 5000);
            }
            else if (command.indexOf("OT") > 0) {
                bmsController.injectFault(FAULT_OT, 75.0f, 5000);
            }
            else if (command.indexOf("UT") > 0) {
                bmsController.injectFault(FAULT_UT, -10.0f, 5000);
            }
        }
        else if (command == "CLEAR FAULT") {
            bmsController.clearInjectedFault();
        }
    }
}

void setup() {
    bmsController.init();
}

void loop() {
     processSerialCommand();
     bmsController.run();
}
