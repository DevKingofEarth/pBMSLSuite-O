# 🧪 pBMSLSuite-O Test Results and Verification

**Last Updated:** 2026-01-23
**Firmware Version:** v1.0
**Wokwi Project:** https://wokwi.com/projects/453769002432989185

---

## 📋 Test Overview

This document tracks all testing performed on the pBMSLSuite-O firmware. Tests verify:
- ✅ Voltage threshold detection (2.8V-4.2V)
- ✅ Fault detection (OV/UV/OT/UT)
- ✅ Cell balancing logic
- ✅ SoC/SoH estimation
- ✅ State machine transitions

---

## 🔋 Test 1: Voltage Threshold Verification

### Objective
Verify that the firmware correctly detects and classifies cell voltages within the safe operating range (2.8V-4.2V).

### Test Configuration
- **Platform:** Wokwi Simulation
- **Potentiometers:** 4× (CELL1_PIN=34, CELL2_PIN=35, CELL3_PIN=25, CELL4_PIN=26)
- **Thresholds:**
  - OV_THRESHOLD: 4.25V
  - UV_THRESHOLD: 2.80V
  - BALANCE_THRESHOLD: 4.10V

### Test Cases

| Test # | Cell 1 | Cell 2 | Cell 3 | Cell 4 | Expected Outcome | Status |
|--------|--------|--------|--------|--------|------------------|--------|
| 1.1    | 3.3V   | 3.3V   | 3.3V   | 3.3V   | All OK ✅        | ⏳     |
| 1.2    | 4.2V   | 4.2V   | 4.2V   | 4.2V   | All CHARGED 🔋   | ⏳     |
| 1.3    | 2.8V   | 2.8V   | 2.8V   | 2.8V   | All LOW 🔋       | ⏳     |
| 1.4    | 4.3V   | 3.3V   | 3.3V   | 3.3V   | OV on Cell1 ⚠️   | ⏳     |
| 1.5    | 2.7V   | 3.3V   | 3.3V   | 3.3V   | UV on Cell1 ⚠️   | ⏳     |
| 1.6    | 3.0V   | 3.1V   | 3.2V   | 3.3V   | All OK, mixed 🔋 | ⏳     |

### Voltage Reading Formula Verification

**Current Formula (Line 129-132):**
```cpp
float rawV1 = analogRead(CELL1_PIN) * (V_REF / ADC_MAX_VALUE);
// With V_REF=3.3V, ADC_MAX_VALUE=4095
// Range: 0V to 3.3V (ESP32 hardware limit)
```

**Limitations:**
- ❌ Cannot measure voltages above 3.3V (hardware clamped)
- ✅ Works for simulation with potentiometers (0-3.3V range)
- ⚠️ Requires voltage divider for real 4.2V measurements

### Expected Serial Output Pattern
```
═══════════════════════════════════════════════════════════
                    pBMSLSuite-O STATUS                     
═══════════════════════════════════════════════════════════
⏱️  Time: XX seconds
───────────────────────────────────────────────────────────
🔋 CELL VOLTAGES:
   Cell 1: 3.300V ✅ OK
   Cell 2: 3.300V ✅ OK
   Cell 3: 3.300V ✅ OK
   Cell 4: 3.300V ✅ OK
   Average: 3.30V | Pack: 13.20V
───────────────────────────────────────────────────────────
🌡️  Temperature: 25.0°C ✅ OK
───────────────────────────────────────────────────────────
🔋 BATTERY STATUS:
   SoC: XX.X% | SoH: 100.0%
───────────────────────────────────────────────────────────
⚙️  STATE: 🟢 IDLE (Ready)
   Balancing: ⏸️  IDLE
───────────────────────────────────────────────────────────
⚠️  FAULT: ✅ NONE
───────────────────────────────────────────────────────────
📊 THRESHOLDS:
   OV: 4.25V | UV: 2.80V | OT: 60°C | UT: 0°C
═══════════════════════════════════════════════════════════
```

---

## ⚠️ Test 2: Fault Detection (OV/UV/OT/UT)

### Objective
Verify that the firmware correctly detects and reports all fault conditions.

### Test Configuration
- **Fault Detection Logic:** Lines 225-235
- **State Transition on Fault:** Lines 186-187, 205

### Test Cases

| Test # | Condition | Threshold | Expected Fault | Status |
|--------|-----------|-----------|-----------------|--------|
| 2.1    | Over-Voltage | Cell > 4.25V | FAULT_OV 🔴 | ⏳     |
| 2.2    | Under-Voltage | Cell < 2.80V | FAULT_UV 🔴 | ⏳     |
| 2.3    | Over-Temperature | Temp > 60°C | FAULT_OT 🔴 | ⏳     |
| 2.4    | Under-Temperature | Temp < 0°C | FAULT_UT 🔴 | ⏳     |
| 2.5    | Fault Recovery | Condition clears | FAULT_NONE ✅ | ⏳ |

### Fault Indication
- **Serial Output:** "❌ [FAULT_TYPE]"
- **LED Behavior:** Red LED (GPIO 2) flashes (500ms on/off)
- **State:** Transitions to FAULT state
- **Recovery:** Auto-recover when fault condition clears

---

## 🔄 Test 3: Cell Balancing Logic

### Objective
Verify that the firmware correctly activates balancing when cell voltages exceed balance threshold.

### Test Configuration
- **Balance Threshold:** 4.10V
- **Balance Pins:** GPIO 16-19 (4× balance resistors)
- **Balancing Logic:** Lines 189-219

### Test Cases

| Test # | Cell 1 | Cell 2 | Cell 3 | Cell 4 | Balance Active | Status |
|--------|--------|--------|--------|--------|-----------------|--------|
| 3.1    | 4.15V  | 4.15V  | 4.15V  | 4.15V  | Yes 🔄         | ⏳     |
| 3.2    | 4.05V  | 4.05V  | 4.05V  | 4.05V  | No ⏸️          | ⏳     |
| 3.3    | 4.15V  | 4.00V  | 4.00V  | 4.00V  | Selective (C1)  | ⏳     |
| 3.4    | 4.20V  | 4.18V  | 4.19V  | 4.17V  | Yes (all pins)  | ⏳     |

### Expected Behavior
- When any cell > 4.10V → Enable corresponding balance resistor
- Balance resistor dissipates energy from that cell
- Cells equalize over time
- State = BALANCING 🟠
- When all cells < 4.10V → State returns to IDLE 🟢

---

## 📊 Test 4: SoC/SoH Estimation

### Objective
Verify that State of Charge (SoC) and State of Health (SoH) are calculated correctly.

### Test Configuration
- **SoC Calculation:** Lines 157-165
  - Formula: `((avgVoltage - 3.00f) / 1.20f) * 100.0f`
  - Range: 0% (3.0V) to 100% (4.2V)
- **SoH Calculation:** Lines 167-171
  - Degrades by 0.01% per cycle in extreme temps

### Test Cases

| Test # | Avg Voltage | Expected SoC | Status |
|--------|-------------|--------------|--------|
| 4.1    | 3.0V        | 0%           | ⏳     |
| 4.2    | 3.6V        | 50%          | ⏳     |
| 4.3    | 4.2V        | 100%         | ⏳     |
| 4.4    | 3.3V        | 25%          | ⏳     |

### Temperature Impact on SoH
- **Condition:** Temp > 45°C or Temp < 10°C
- **Effect:** SoH decreases by 0.01% per loop (100ms)
- **Outcome:** Over 1 hour of extreme temp → 0.36% SoH loss

---

## 🔧 Test 5: State Machine Verification

### Objective
Verify that state transitions work correctly for all conditions.

### State Diagram

```
    ┌─────────────┐
    │   IDLE 🟢   │
    │  (Ready)    │
    └─────────────┘
          ↕ (needs balancing)
    ┌─────────────┐
    │BALANCING 🟠 │
    │ (Active)    │
    └─────────────┘
          ↕ (fault detected)
    ┌─────────────┐
    │  FAULT 🔴   │
    │ (ACTION!)   │
    └─────────────┘
```

### Test Cases

| Test # | Current State | Trigger | Expected Next State | Status |
|--------|---------------|---------|---------------------|--------|
| 5.1    | IDLE          | None    | IDLE (stable)       | ⏳     |
| 5.2    | IDLE          | Cell > 4.1V | BALANCING       | ⏳     |
| 5.3    | IDLE          | Cell > 4.25V | FAULT         | ⏳     |
| 5.4    | BALANCING     | All cells < 4.1V | IDLE      | ⏳     |
| 5.5    | BALANCING     | OV/UV/OT/UT | FAULT         | ⏳     |
| 5.6    | FAULT         | Condition clears | IDLE      | ⏳     |

---

## 🌡️ Test 6: Temperature Sensing (NTC Thermistor)

### Objective
Verify temperature calculations using Steinhart-Hart equation.

### Test Configuration
- **Sensor:** NTC Thermistor (GPIO 33)
- **Steinhart-Hart Implementation:** Lines 144-155
- **Beta Coefficient:** 3950

### Test Cases

| Temp(°C) | Expected | Actual | Status |
|----------|----------|--------|--------|
| 25°C     | Reference | ⏳     | ⏳     |
| 0°C      | Cold     | ⏳     | ⏳     |
| 60°C     | Over-temp| ⏳     | ⏳     |

---

## 📈 Test 7: Filtering & Noise Reduction

### Objective
Verify that moving average filter (5-sample) reduces ADC noise effectively.

### Filter Configuration
- **Buffer Size:** 5 samples
- **Implementation:** Lines 33-53
- **Update Rate:** 10Hz (100ms loop)
- **Settling Time:** ~500ms (5 samples × 100ms)

### Test Method
1. Set potentiometer to 3.3V
2. Observe serial output
3. Verify readings stabilize within 500ms
4. Measure noise reduction (ADC jitter)

### Expected Results
- ✅ Noise reduced from ±5-10mV to ±1-2mV
- ✅ Response time: ~500ms after change
- ⏳ Status: Testing pending

---

## 🚨 Test 8: Serial Output Validation

### Objective
Verify that serial output is correctly formatted and contains all required data.

### Serial Configuration
- **Baud Rate:** 115200
- **Format:** Human-readable with icons
- **Update Rate:** 1Hz (CSV format every 1000ms)

### Output Components
- ✅ Timestamp
- ✅ All 4 cell voltages with status
- ✅ Average voltage & pack voltage
- ✅ Temperature with status
- ✅ SoC & SoH percentages
- ✅ State machine status with icon
- ✅ Balancing status
- ✅ Fault status
- ✅ Threshold values

---

## 🔬 Hardware Verification Checklist

### ADC Reading Accuracy
- [ ] **Test 1.1-1.6:** Verify voltage readings match pot positions
- [ ] **Accuracy:** ±20mV acceptable for simulation
- [ ] **Linearity:** Voltage should scale linearly with pot

### Pin Configuration
- [ ] **Cell Pins:** 34, 35, 25, 26 correctly read ADC
- [ ] **Temp Pin:** 33 correctly read NTC
- [ ] **Balance Pins:** 16-19 output HIGH when active
- [ ] **LED Pins:** 2 (red) responds to faults

### Power & Stability
- [ ] **Compile Size:** 291857 bytes (22% of 1310720) ✅
- [ ] **RAM Usage:** 20464 bytes (6% of 327680) ✅
- [ ] **Runtime:** Stable for >1 hour simulation
- [ ] **No Crashes:** Serial output continuous

---

## 📝 Test Execution Log

### Test Date: 2026-01-23

| Test ID | Status | Notes |
|---------|--------|-------|
| 1.1-1.6 | ⏳     | Pending Wokwi simulation |
| 2.1-2.5 | ⏳     | Pending Wokwi simulation |
| 3.1-3.4 | ⏳     | Pending Wokwi simulation |
| 4.1-4.4 | ⏳     | Pending Wokwi simulation |
| 5.1-5.6 | ⏳     | Pending Wokwi simulation |
| 6.1-6.2 | ⏳     | Pending Wokwi simulation |
| 7.1     | ⏳     | Pending serial monitoring |
| 8.1-8.4 | ⏳     | Pending hardware check |

---

## 🔧 Known Issues & Limitations

### Current Implementation (v1.0)
1. **Voltage Measurement Limit:** Max 3.3V (ESP32 ADC limit)
   - ❌ Cannot measure cell voltages above 3.3V in real hardware
   - ✅ Works in Wokwi simulation with potentiometers
   - 🔧 **Fix:** Add voltage dividers for real 4.2V measurement

2. **No Current Sensing:** Only voltage and temperature
   - ❌ Cannot detect load current or discharge rate
   - 🔧 **Fix:** Add INA219 current sensor

3. **No Load Simulation:** Static voltage measurement only
   - ❌ Cannot test SoC estimation under load
   - 🔧 **Fix:** Add load slider (Wokwi simulation)

4. **Manual State Control:** No automatic CHARGING/DISCHARGING states
   - ❌ Only IDLE, BALANCING, FAULT implemented
   - 🔧 **Fix:** Add current sensor for automatic state transitions

---

## ✅ Completion Criteria

A test is **PASSED** when:
- [ ] All assertions in test case satisfied
- [ ] Serial output shows expected values
- [ ] LED behavior matches expected pattern
- [ ] No runtime errors or crashes
- [ ] Thresholds enforced correctly

---

## 📊 Test Summary (To be completed)

```
Total Tests: 32
Passed:      0/32 ⏳
Failed:      0/32 ⏳
Pending:     32/32 ⏳
Pass Rate:   0%
```

---

## 🔗 References

- **Firmware:** `pBMSLSuite-O.ino`
- **Wokwi Project:** https://wokwi.com/projects/453769002432989185
- **Circuit Diagram:** `diagram.json`
- **ADC Analysis:** `ESP32_ADC_ANALYSIS.md`
- **Voltage Analysis:** `VOLTAGE_ANALYSIS.md`

---

## 📞 Next Steps

1. ✅ Upload firmware to Wokwi (compile successful)
2. ⏳ Run Test 1-8 in Wokwi simulation
3. ⏳ Document all results
4. ⏳ Fix voltage divider issue (Task 5)
5. ⏳ Add current sensor support (Task 6)
6. ⏳ Update Wokwi diagram (Task 7)

