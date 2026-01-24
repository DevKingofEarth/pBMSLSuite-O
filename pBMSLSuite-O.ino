// pBMSLSuite-O - Portable BMS Lab Suite
// Core BMS functionality: cell monitoring, SoC, fault protection
// Educational prototype for BMS firmware concepts

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
#define STATUS_LED_PIN 22  // BMS state indication

// NTC Thermistor Configuration
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
};

// BMS State Machine
enum BMSState { IDLE, BALANCING, FAULT };

// Fault Types
enum FaultType { FAULT_NONE, FAULT_OV, FAULT_UV, FAULT_OT, FAULT_UT };

// Global Instances
MovingAverageFilter cellFilters[4];
MovingAverageFilter tempFilter;
float cellVoltages[4] = {0};
float temperature = 25.0f;
float estimatedSoC = 100.0f;
float estimatedSoH = 100.0f;

BMSState currentState = IDLE;
FaultType currentFault = FAULT_NONE;
unsigned long lastCSVTime = 0;

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
        Serial.println("Core BMS: 4-cell monitoring, SoC, fault protection");
        Serial.println();
    }
      
    void run() {
        // Read sensors
        readSensors();
        
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
      
private:
    void readSensors() {
        // Read cell voltages with filtering
        float rawV1 = analogRead(CELL1_PIN) * (V_REF / ADC_MAX_VALUE);
        float rawV2 = analogRead(CELL2_PIN) * (V_REF / ADC_MAX_VALUE);
        float rawV3 = analogRead(CELL3_PIN) * (V_REF / ADC_MAX_VALUE);
        float rawV4 = analogRead(CELL4_PIN) * (V_REF / ADC_MAX_VALUE);
          
        cellVoltages[0] = cellFilters[0].add(rawV1);
        cellVoltages[1] = cellFilters[1].add(rawV2);
        cellVoltages[2] = cellFilters[2].add(rawV3);
        cellVoltages[3] = cellFilters[3].add(rawV4);
        
        // Read temperature using Steinhart-Hart
        float tempVoltage = analogRead(TEMP_PIN) * (V_REF / ADC_MAX_VALUE);
        temperature = calculateTemperatureNTC(tempVoltage);
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
        
        // SoC estimation based on average voltage
        if (avgVoltage >= 4.20f) estimatedSoC = 100.0f;
        else if (avgVoltage <= 3.00f) estimatedSoC = 0.0f;
        else {
            estimatedSoC = ((avgVoltage - 3.00f) / 1.20f) * 100.0f;
        }
        
        // Simple SoH estimation (degrades with extreme temps)
        if (temperature > 45.0f || temperature < 10.0f) {
            estimatedSoH -= 0.01f;
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
                
            case BALANCING: {
                bool needsBalancing = false;
                for (int i = 0; i < 4; i++) {
                    if (cellVoltages[i] > BALANCE_THRESHOLD) needsBalancing = true;
                }
                if (!needsBalancing) currentState = IDLE;
                if (currentFault != FAULT_NONE) currentState = FAULT;
                break;
            }
                
            case FAULT:
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
        
        if (maxCell > OV_THRESHOLD) currentFault = FAULT_OV;
        else if (maxCell < UV_THRESHOLD) currentFault = FAULT_UV;
        else if (temperature > OT_THRESHOLD) currentFault = FAULT_OT;
        else if (temperature < UT_THRESHOLD) currentFault = FAULT_UT;
        else currentFault = FAULT_NONE;
    }
    
    void logCSV() {
        Serial.println();
        Serial.println("==================================================");
        Serial.println("              pBMSLSuite-O STATUS                 ");
        Serial.println("==================================================");
        
        Serial.print("Time: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        Serial.println("--------------------------------------------------");
        Serial.println("CELL VOLTAGES:");
        Serial.printf("  Cell 1: %.3fV %s\n", cellVoltages[0], getVoltageStatus(cellVoltages[0]));
        Serial.printf("  Cell 2: %.3fV %s\n", cellVoltages[1], getVoltageStatus(cellVoltages[1]));
        Serial.printf("  Cell 3: %.3fV %s\n", cellVoltages[2], getVoltageStatus(cellVoltages[2]));
        Serial.printf("  Cell 4: %.3fV %s\n", cellVoltages[3], getVoltageStatus(cellVoltages[3]));
        
        float avgVoltage = (cellVoltages[0] + cellVoltages[1] + cellVoltages[2] + cellVoltages[3]) / 4.0f;
        Serial.printf("  Average: %.3fV | Pack: %.2fV\n", avgVoltage, avgVoltage * 4);
        
        Serial.println("--------------------------------------------------");
        Serial.print("Temperature: ");
        Serial.print(temperature, 1);
        Serial.print("C ");
        Serial.println(getTempStatus());
        
        Serial.println("--------------------------------------------------");
        Serial.println("BATTERY STATUS:");
        Serial.printf("  SoC: %.1f%% | SoH: %.1f%%\n", estimatedSoC, estimatedSoH);
        
        Serial.println("--------------------------------------------------");
        Serial.print("STATE: ");
        Serial.println(getStateName());
        
        Serial.print("Balancing: ");
        bool balancing = digitalRead(BALANCE1_PIN) || digitalRead(BALANCE2_PIN) || 
                        digitalRead(BALANCE3_PIN) || digitalRead(BALANCE4_PIN);
        Serial.println(balancing ? "ACTIVE" : "IDLE");
        
        Serial.println("--------------------------------------------------");
        Serial.print("FAULT: ");
        if (currentFault == FAULT_NONE) {
            Serial.println("NONE");
        } else {
            printFaultName();
            Serial.println();
        }
        
        Serial.println("--------------------------------------------------");
        Serial.println("THRESHOLDS:");
        Serial.printf("  OV: %.2fV | UV: %.2fV | OT: %.0fC | UT: %.0fC\n", 
                     OV_THRESHOLD, UV_THRESHOLD, OT_THRESHOLD, UT_THRESHOLD);
        
        Serial.println("==================================================");
    }
    
    const char* getVoltageStatus(float voltage) {
        if (voltage > OV_THRESHOLD) return "[OV]";
        if (voltage < UV_THRESHOLD) return "[UV]";
        if (voltage > BALANCE_THRESHOLD) return "[CHARGED]";
        if (voltage < 3.0f) return "[LOW]";
        return "[OK]";
    }
    
    const char* getTempStatus() {
        if (temperature > OT_THRESHOLD) return "[HOT]";
        if (temperature < UT_THRESHOLD) return "[COLD]";
        if (temperature > 45.0f) return "[WARM]";
        return "[OK]";
    }
    
    const char* getStateName() {
        switch (currentState) {
            case IDLE: return "IDLE";
            case BALANCING: return "BALANCING";
            case FAULT: return "FAULT";
            default: return "UNKNOWN";
        }
    }
    
    void printFaultName() {
        switch (currentFault) {
            case FAULT_OV: Serial.print("OVER-VOLTAGE"); break;
            case FAULT_UV: Serial.print("UNDER-VOLTAGE"); break;
            case FAULT_OT: Serial.print("OVER-TEMPERATURE"); break;
            case FAULT_UT: Serial.print("UNDER-TEMPERATURE"); break;
            default: Serial.print("NONE");
        }
    }
};

PBMSController bmsController;

void setup() {
    bmsController.init();
}

void loop() {
    bmsController.run();
}