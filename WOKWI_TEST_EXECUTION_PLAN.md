# 🔬 Wokwi Test Execution Plan - LED Bar Graph Fix Verification

**Date**: Jan 23, 2026  
**Status**: READY FOR TESTING  
**Wokwi Project**: https://wokwi.com/projects/453769002432989185

---

## 📋 Test Objectives

After applying fixes:
1. ✅ **LED Bar Graph**: All 10 segments light sequentially (0→10)
2. ✅ **Status LED**: GPIO 22 indicates system state (steady=OK, flash=FAULT)
3. ✅ **Serial Output**: Clean, readable status messages
4. ✅ **Fault Injection**: OV/UV/OT/UT properly detected and logged

---

## 🔧 Setup Instructions

### Pre-Test Checklist
- [ ] Firmware: `pBMSLSuite-O.ino` uploaded with LED bar loop fix (LSB-first: `for (int i = 0; i < 16; i++)`)
- [ ] Circuit: `diagram.json` loaded (no pot5, no led1, shift registers on GPIO 13/2/14)
- [ ] Serial Console: Open and monitoring at 115200 baud
- [ ] Simulation Time: Set to 1x or 2x speed for faster testing

---

## 🧪 Test Cases

### Test 1: LED Bar Graph Sequential Lighting ⭐ CRITICAL

**Objective**: Verify all 10 LED segments light in order as SoC increases

**Procedure**:
1. **Start simulation** with cell voltage at minimum (turn pot all the way down)
   - Expected: 0 LEDs lit (SoC ≈ 0%)
   - Verify: All segments dark

2. **Adjust potentiometer 1 (GPIO 34)** slowly from 0V → 4.2V
   - **Watch the LED bar graph**
   - Expected pattern:
     ```
     SoC: 0%  → LEDs: 0 (────────────)
     SoC: 10% → LEDs: 1 (●───────────)
     SoC: 20% → LEDs: 2 (●●──────────)
     SoC: 30% → LEDs: 3 (●●●─────────)
     ...
     SoC: 100% → LEDs: 10 (●●●●●●●●●●)
     ```

3. **Record observations**:
   - [ ] Do all 10 LEDs light?
   - [ ] Do they light in sequence (A1, A2, A3, ... A10)?
   - [ ] No random segments glowing?
   - [ ] No missing segments?

**Success Criteria**:
- All 10 segments light sequentially
- No gaps or out-of-order LEDs
- Each segment is bright and clear

**Failure Analysis** (if only A7-A10 light):
- LSB-first loop didn't apply correctly
- Check line 573: should be `for (int i = 0; i < 16; i++)`

---

### Test 2: Status LED Behavior

**Objective**: Verify GPIO 22 LED indicates system state correctly

**Normal State (IDLE)**:
1. Maintain all cell voltages between 2.5V and 4.2V (safe range)
2. Temperature at 25°C (NTC default)
3. Expected: **Status LED steady ON** (green or appropriate color)

**Fault State (After Injection)**:
1. Status LED should **flash** when system detects fault
2. See Test 5 for injection details

---

### Test 3: Serial Output Clarity

**Objective**: Verify serial console output is clean and readable

**Expected Output** (every 1 second):
```
========== BMS Status ==========
STATE: IDLE (Steady, 0 LEDs)
VOLTAGE:     2.7V  3.0V  3.2V  3.0V
TEMP:        25°C
SoC:         30%
Fault Status: NONE
=================================

(repeat every 1 second)
```

**Check for**:
- [ ] No garbage characters
- [ ] All values present (4 voltages, temp, SoC, fault status)
- [ ] Consistent format
- [ ] No repeated errors

---

### Test 4: Cell Voltage Reading

**Objective**: Verify potentiometer values map to correct cell voltages

**Procedure**:
1. Turn Pot1 (GPIO 34) to 0% → Should read ~1.0V (no cell)
2. Turn Pot1 to 50% → Should read ~2.65V (typical discharged cell)
3. Turn Pot1 to 100% → Should read ~4.2V (charged cell)

**Check**: Serial output shows matching voltages

---

### Test 5: Fault Injection & Detection

**Objective**: Verify fault detection works correctly

**Procedure**: Use serial console to inject faults

#### 5a. Over-Voltage (OV)
```
Serial Input: INJECT OV
Expected Serial Output: "FAULT DETECTED: Over-voltage on Cell X"
Expected Status LED: Flash pattern (rapid blink)
Expected LED Bar: All 10 LEDs light (red alert)
```

#### 5b. Under-Voltage (UV)
```
Serial Input: INJECT UV
Expected Serial Output: "FAULT DETECTED: Under-voltage on Cell X"
Expected Status LED: Flash pattern
Expected LED Bar: All 10 LEDs light
```

#### 5c. Over-Temperature (OT)
```
Serial Input: INJECT OT
Expected Serial Output: "FAULT DETECTED: Over-temperature"
Expected Status LED: Flash pattern
Expected LED Bar: All 10 LEDs light
```

#### 5d. Under-Temperature (UT)
```
Serial Input: INJECT UT
Expected Serial Output: "FAULT DETECTED: Under-temperature"
Expected Status LED: Flash pattern
Expected LED Bar: All 10 LEDs light
```

**Recovery**:
```
Serial Input: CLEAR FAULTS
Expected: System returns to IDLE state
Expected LED Bar: Back to SoC-based display (0-10 LEDs)
Expected Status LED: Steady on
```

---

### Test 6: State Transitions

**Objective**: Verify state machine transitions work correctly

**IDLE → BALANCING**:
1. Set Cell1 = 3.8V, Cell2 = 3.2V, Cell3 = 3.0V, Cell4 = 2.8V (voltage imbalance)
2. Wait 5+ seconds for balancing logic to detect
3. Expected Serial Output: `STATE: BALANCING`
4. Expected: Balance control pins (GPIO 16-19) should activate

**BALANCING → IDLE** (after voltage equalizes):
1. Manually adjust pots to make all cells ≈ 3.5V
2. Wait for automatic transition
3. Expected: Back to `STATE: IDLE`

---

## 📊 Test Results Template

```
TEST EXECUTION REPORT
====================
Date: [DATE]
Tester: [NAME]
Wokwi Build: [BUILD_ID]

TEST 1: LED Bar Graph Sequential Lighting
Status: [ ] PASS [ ] FAIL [ ] PARTIAL
Observations: 
- All 10 LEDs: YES / NO
- Sequential order: YES / NO
- Any gaps: NO / YES (which ones?)
Details: 

TEST 2: Status LED Behavior
Status: [ ] PASS [ ] FAIL
Observations:
- Steady on in IDLE: YES / NO
- Flashes on fault: YES / NO
Details:

TEST 3: Serial Output
Status: [ ] PASS [ ] FAIL
Observations:
- Format clear: YES / NO
- No garbage: YES / NO
- All fields present: YES / NO
Details:

TEST 4: Cell Voltage Reading
Status: [ ] PASS [ ] FAIL
Observations:
- Pot1 0% → reading: ___V (expected ~1V)
- Pot1 50% → reading: ___V (expected ~2.65V)
- Pot1 100% → reading: ___V (expected ~4.2V)
Details:

TEST 5: Fault Injection
Status: [ ] PASS [ ] FAIL
Observations:
- OV detected: YES / NO
- UV detected: YES / NO
- OT detected: YES / NO
- UT detected: YES / NO
- LED bar shows alert (all 10): YES / NO
- Status LED flashes: YES / NO
- Recovery works: YES / NO
Details:

TEST 6: State Transitions
Status: [ ] PASS [ ] FAIL
Observations:
- IDLE→BALANCING: YES / NO
- BALANCING→IDLE: YES / NO
Details:

OVERALL RESULT: [ ] PASS [ ] FAIL
Notes:
```

---

## ✅ Success Criteria (All Must Pass)

| Test | Requirement | Status |
|------|-------------|--------|
| LED Bar Graph | All 10 segments light sequentially 0→10 | ⬜ |
| Status LED | Steady in IDLE, flashes on FAULT | ⬜ |
| Serial Output | Clean, readable, no errors | ⬜ |
| Fault Detection | OV/UV/OT/UT all work | ⬜ |
| LED Alert | All 10 LEDs light on fault | ⬜ |
| Recovery | System returns to normal after clearing fault | ⬜ |

---

## 🐛 Troubleshooting

### Issue: Only A7 LED lights (Previous bug)
**Solution**: 
- Verify firmware line 573: `for (int i = 0; i < 16; i++)`
- Should NOT be: `for (int i = 15; i >= 0; i--)`
- Re-upload firmware if needed

### Issue: LEDs don't light at all
**Check**:
- [ ] Shift registers connected to GPIO 13/2/14
- [ ] 220Ω resistors present on LED bar anodes
- [ ] LED bar cathodes grounded
- [ ] Firmware compiling without errors

### Issue: Serial output missing
**Check**:
- [ ] Console open at 115200 baud
- [ ] USB cable connected
- [ ] Wokwi serial monitor active

### Issue: Status LED doesn't flash on fault
**Check**:
- [ ] GPIO 22 configured as OUTPUT
- [ ] LED2 (STATUS_LED) physically on GPIO 22 in diagram
- [ ] Fault detection code reached (watch serial output)

---

## 📝 Notes for Future Testing

1. **Real Hardware Testing**: Once Wokwi tests pass, deploy to actual ESP32 + breadboard
2. **Load Simulation**: Consider adding stepper motor or PWM load for realistic discharge
3. **Balancing Current**: Implement actual sense resistor for balance current measurement
4. **INA219 Integration**: Add power monitoring (if applicable)

---

## 🎯 Next Steps After Testing

✅ If all tests PASS:
1. Update REAL_ISSUES_FIXED.md with test results
2. Clean up outdated documentation
3. Create final summary document
4. Git commit: "Fix LED bar graph bit-order and remove fake features"
5. Mark project as **COMPLETE**

❌ If tests FAIL:
1. Document exact failure in notes
2. Use debugging to identify root cause
3. Propose fix with evidence
4. Re-test until all pass

---

**Created**: Jan 23, 2026 16:00 IST  
**Status**: READY FOR WOKWI TESTING  
**Critical Tests**: LED Bar Graph (Test 1), Fault Injection (Test 5)

