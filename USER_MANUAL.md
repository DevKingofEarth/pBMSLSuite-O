# 📖 pBMSLSuite-O User Manual

## 🚀 Quick Start

1. **Hardware Setup**
   - Connect components per `diagram.json`
   - Power ESP32 via USB
   - Open Serial Monitor (115200 baud)

2. **Initial Test**
   - Adjust all pots to ~3.7V
   - Verify all cells show [OK] status
   - Confirm state = IDLE

3. **Build & Flash**
   ```bash
   ./compile.sh
   # Flash if needed (see README.md)
   ```

---

## 📊 Understanding the Display

### Cell Voltages
- **[OK]**: Normal range (2.8V - 4.25V)
- **[CHARGED]**: > 4.1V (balancing may activate)
- **[LOW]**: < 3.0V (low charge warning)
- **[OV]**: > 4.25V (over-voltage fault)
- **[UV]**: < 2.8V (under-voltage fault)

### Temperature
- **[OK]**: 0°C - 45°C
- **[WARM]**: 45°C - 60°C
- **[HOT]**: > 60°C (over-temp fault)
- **[COLD]**: < 0°C (under-temp fault)

### States
- **IDLE**: Normal monitoring
- **BALANCING**: Discharging high cells
- **FAULT**: Protection triggered

---

## 🧪 Testing Procedures

### 1. Normal Operation Test
```
Set all pots to: 3.6V - 3.8V
Expected: State = IDLE, Balancing = IDLE, Fault = NONE
```

### 2. Cell Balancing Test
```
Set one pot to: 4.2V (others at 3.7V)
Expected: State = BALANCING, Balancing = ACTIVE
```

### 3. Over-Voltage Test
```
Set any pot to: 4.3V
Expected: State = FAULT, Fault = OVER-VOLTAGE
```

### 4. Under-Voltage Test
```
Set any pot to: 2.7V
Expected: State = FAULT, Fault = UNDER-VOLTAGE
```

### 5. Temperature Test
```
Heat/cool thermistor past thresholds
Expected: State = FAULT, appropriate temp fault
```

---

## 🔧 Calibration

### Voltage Calibration
1. Measure actual pot voltage with multimeter
2. Compare to serial output
3. Adjust `V_REF` constant if needed (default 3.3V)

### Temperature Calibration
1. Measure actual temperature
2. Compare to serial output
3. Adjust `NTC_BETA_COEFFICIENT` if needed (default 3950.0)

---

## 📋 Troubleshooting

### No Serial Output
- Check baud rate (115200)
- Verify USB connection
- Reset ESP32

### Incorrect Voltage Readings
- Check pot connections
- Verify 3.3V reference
- Calibrate as above

### Fault Not Triggering
- Verify threshold values in code
- Check pot range (0-3.3V on ADC)
- Test with extreme values

### Balancing Not Working
- Check balance resistor connections
- Verify GPIO 16-19 wiring
- Ensure cell > 4.1V threshold

---

## 🎓 Learning Exercises

### Exercise 1: Threshold Modification
Modify protection thresholds and observe behavior changes.

### Exercise 2: Filter Adjustment
Change moving average filter size and note response differences.

### Exercise 3: SoC Algorithm
Implement different SoC calculation method (Coulomb counting).

### Exercise 4: Add New Fault
Create custom fault condition (e.g., rapid voltage change).

---

## 📈 Data Interpretation

### SoC Calculation
Current method: Voltage-based linear interpolation
- 4.20V = 100%
- 3.00V = 0%
- Linear between these points

### Limitations
- No current sensing
- Simplified temperature model
- No aging effects
- No cell chemistry differences

---

## ⚙️ Advanced Configuration

### Modify Thresholds
Edit these constants in `pBMSLSuite-O.ino`:
```cpp
#define OV_THRESHOLD 4.25f
#define UV_THRESHOLD 2.8f
#define OT_THRESHOLD 60.0f
#define BALANCE_THRESHOLD 4.1f
```

### Change Update Rate
Modify loop delay:
```cpp
delay(100);  // 10Hz (default)
// delay(50);   // 20Hz (faster)
```

### Filter Tuning
Adjust buffer size in `MovingAverageFilter`:
```cpp
float buffer[5] = {0};  // 5-sample (default)
// float buffer[10] = {0}; // 10-sample (smoother)
```

---

## 🔍 Code Structure

### Main Components
- `PBMSController`: Core BMS logic
- `MovingAverageFilter`: Signal processing
- State machine: Fault handling
- Serial output: Telemetry

### Key Functions
- `readSensors()`: ADC data acquisition
- `updateSoC()`: State of charge calculation
- `checkFaults()`: Protection logic
- `updateStateMachine()`: State transitions

---

## 📞 Support

For issues:
1. Check this manual first
2. Verify hardware connections
3. Review serial output for error clues
4. Compare with expected behavior in test procedures

---

*Remember: This is an educational prototype. Always use proper BMS hardware for real applications.*