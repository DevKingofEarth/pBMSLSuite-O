# 🚀 Wokwi Test Execution Plan - Stepper Motor Load Testing

**Date**: Jan 23, 2026  
**Status**: READY FOR TESTING  
**Wokwi Project**: https://wokwi.com/projects/453769002432989185

---

## 📋 Test Objectives

Verify the stepper motor load testing implementation:
1. ✅ **Stepper Motor Control**: PWM speed control (0-100% load)
2. ✅ **Real Discharge**: SoC decreases proportionally to load
3. ✅ **LED Bar Graph**: Shows minimum cell SoC (not average)
4. ✅ **Status LED**: GPIO 22 indicates system state
5. ✅ **Serial Output**: Shows motor status and discharge rates
6. ✅ **Fault Injection**: Works under load conditions

---

## 🔧 Setup Instructions

### Pre-Test Checklist
- [ ] Firmware: `pBMSLSuite-O.ino` uploaded (stepper motor code included)
- [ ] Circuit: `diagram.json` loaded (clean version)
- [ ] Stepper Motor: Manually added in Wokwi browser (see MANUAL_STEPPER_INTEGRATION.md)
- [ ] Connections: 5 wires from ESP32 GPIO 4,5,12,15,23 to stepper motor
- [ ] Serial Console: Open and monitoring at 115200 baud
- [ ] Simulation Time: Set to 1x speed for accurate timing

---

## 🧪 Test Cases

### Test 1: Stepper Motor Load Control ⭐ CRITICAL

**Objective**: Verify stepper motor responds to load commands and affects discharge

**Procedure**:
1. **Start simulation** with cells at 4.2V (100% SoC)
   - Type `LOAD 0` (should show motor stopped)
   - Verify: No discharge, motor status shows "Stopped"

2. **Test low load**: Type `LOAD 25`
   - **Expected**: Motor spins slowly, discharge begins
   - Serial output: `Stepper Motor: Running (25% load)`
   - SoC decreases: ~0.25% per minute

3. **Test high load**: Type `LOAD 100`
   - **Expected**: Motor spins fast, rapid discharge
   - Serial output: `Stepper Motor: Running (100% load)`
   - SoC decreases: ~1.0% per minute

4. **Stop load**: Type `STOP LOAD`
   - **Expected**: Motor stops immediately
   - Serial output: `Stepper Motor: Stopped`
   - Discharge halts

**Success Criteria**:
- Motor speed changes with load commands
- SoC decreases proportionally to load
- STOP LOAD halts everything
- Serial output shows correct motor status

**Failure Analysis**:
- No motor spin: Check GPIO connections (4,5,12,15,23)
- No discharge: Check SoC calculation code
- Wrong speed: Check PWM timing in stepper control

---

### Test 2: LED Bar Graph - Minimum Cell SoC Display

**Objective**: Verify LED bar shows minimum cell SoC (critical for BMS safety)

**Procedure**:
1. **Set balanced cells**: All 4 cells at 4.0V
   - Expected: LED bar shows ~8-9 LEDs (80-90% SoC)
   - Serial: `Min Cell SoC: ~90%`

2. **Create imbalance**: Set Cell 1 to 3.0V, others at 4.0V
   - Expected: LED bar drops to show minimum cell (Cell 1)
   - Serial: `Min Cell SoC: ~10%` (reflects weakest cell)
   - This demonstrates why minimum cell matters for safety

3. **Test with load**: Type `LOAD 50`, then create imbalance
   - Expected: LED bar shows discharge of weakest cell
   - Serial: Shows real discharge impact on minimum cell

**Success Criteria**:
- LED bar reflects minimum cell voltage, not average
- Imbalanced cells show lower SoC on bar
- Load affects minimum cell discharge rate

**Why This Matters**: BMS safety depends on weakest cell, not average

---

### Test 3: Serial Output with Motor Status

**Objective**: Verify serial output includes stepper motor status and load information

**Expected Output** (every 1 second):
```
🔋 BATTERY STATUS:
   ⚙️  Stepper Motor: Running (75% load)
   Min Cell SoC: 85.2% | SoH: 98.5% | Load: 75%

========== BMS Status ==========
STATE: IDLE (Steady, 8 LEDs)
VOLTAGE:     4.0V  4.0V  4.0V  3.8V
TEMP:        25°C
Min Cell SoC: 82.3%
Fault Status: NONE
=================================
```

**Check for**:
- [ ] Stepper motor status line present
- [ ] Load percentage shown (0-100%)
- [ ] "Min Cell SoC" instead of just "SoC"
- [ ] Load affects discharge rate in output
- [ ] No errors when motor starts/stops

---

### Test 4: Load-Aware State Transitions

**Objective**: Verify BMS state machine works correctly under load

**Procedure**:
1. **Create balancing scenario**: Set cells to 4.0V, 3.8V, 3.6V, 3.4V
   - Expected: System enters BALANCING state
   - Serial: `STATE: BALANCING`
   - Balance pins activate (~5 second cycles)

2. **Add load during balancing**: Type `LOAD 50`
   - Expected: Balancing continues under load
   - Serial: Shows both balancing and motor running
   - Discharge affects balancing effectiveness

3. **Stop load**: Type `STOP LOAD`
   - Expected: Motor stops, balancing continues until cells equalize

**Success Criteria**:
- BALANCING state activates on voltage imbalance
- Motor can run during balancing
- Load affects discharge but doesn't crash system

---

### Test 5: Fault Injection Under Load

**Objective**: Verify fault detection works with stepper motor running

**Procedure**: Test faults while motor is consuming power

#### 5a. Over-Voltage Under Load
```
Serial Input: LOAD 50
Serial Input: INJECT OV
Expected: Fault detection + motor continues running
Expected Serial: "FAULT DETECTED: Over-voltage" + motor status
Expected LED: Status LED flashes, bar shows alert
```

#### 5b. Under-Voltage Under Load
```
Serial Input: LOAD 100
Serial Input: INJECT UV
Expected: Fault + rapid discharge from motor
Expected Serial: Shows fault + high load consumption
```

#### 5c. Over-Temperature Under Load
```
Serial Input: LOAD 75
Serial Input: INJECT OT
Expected: Thermal fault while motor generates heat
Expected Serial: Temperature fault + load status
```

**Recovery Under Load**:
```
Serial Input: CLEAR FAULTS
Expected: Fault clears but motor keeps running
Serial Input: STOP LOAD
Expected: Motor stops, system fully recovers
```

**Success Criteria**:
- Faults detected with motor running
- Motor doesn't crash system during faults
- Recovery commands work with motor active

---

### Test 6: Comprehensive Load Testing Scenarios

**Objective**: Test real-world BMS load testing capabilities

#### Scenario A: Endurance Test
1. Set cells to 80% SoC (3.5V each)
2. Type `LOAD 30` (moderate load)
3. Monitor discharge over 5+ minutes
4. Expected: SoC drops steadily, LED bar shows progress
5. Stop at 20% SoC: `STOP LOAD`

#### Scenario B: Load Stress Test
1. Start with full charge (4.2V)
2. Type `LOAD 100` (maximum load)
3. Expected: Rapid discharge (1% per minute)
4. LED bar shows fast decreasing levels
5. Stop before hitting UV faults

#### Scenario C: Variable Load Simulation
1. Start `LOAD 25` for 2 minutes
2. Change to `LOAD 75` for 2 minutes
3. Change to `LOAD 50` for 2 minutes
4. Expected: Discharge rate changes with load commands
5. LED bar reflects changing discharge rates

**Success Criteria**:
- Load changes affect discharge rates immediately
- System handles variable loads without crashing
- Long-term testing shows realistic battery behavior

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
| Stepper Motor | Responds to LOAD commands, speed changes | ⬜ |
| Load Discharge | SoC decreases proportionally to load | ⬜ |
| Minimum Cell SoC | LED bar shows weakest cell, not average | ⬜ |
| Serial Output | Shows motor status and discharge rates | ⬜ |
| Load + Faults | Faults detected with motor running | ⬜ |
| Load Scenarios | Variable loads work for endurance testing | ⬜ |

---

## 🐛 Troubleshooting

### Issue: Stepper motor doesn't spin
**Check**:
- [ ] Stepper motor component added to Wokwi
- [ ] All 5 wires connected: GPIO 4,5,12,15,23 to stepper pins
- [ ] Wires are orange for easy identification
- [ ] Serial shows "Stepper Motor: Running" when LOAD command sent

### Issue: Motor spins but no discharge
**Check**:
- [ ] Serial output shows "Min Cell SoC" decreasing
- [ ] Load percentage affects discharge rate (higher load = faster discharge)
- [ ] Wait several minutes - discharge is gradual
- [ ] LED bar shows decreasing levels

### Issue: Wrong discharge rate
**Check**:
- [ ] LOAD 100 should discharge 1% per minute
- [ ] LOAD 50 should discharge 0.5% per minute
- [ ] LOAD 25 should discharge 0.25% per minute
- [ ] LOAD 0 should stop all discharge

### Issue: Serial shows errors
**Check**:
- [ ] Baud rate is 115200
- [ ] Commands are uppercase: `LOAD 50`, not `load 50`
- [ ] Stepper motor is properly connected in Wokwi

### Issue: System crashes with load
**Check**:
- [ ] All GPIO pins (4,5,12,15,23) are available (not used elsewhere)
- [ ] Firmware uploaded correctly
- [ ] No compilation errors in Wokwi

---

## 📝 Notes for Real Hardware Deployment

1. **Stepper Motor Driver**: Real hardware needs ULN2003 or A4988 driver
2. **Power Supply**: Stepper motor needs separate 5-12V supply
3. **Current Sensing**: Add INA219 for actual current measurement
4. **Heat Dissipation**: High loads generate real heat - add cooling
5. **Load Profiles**: Create standard discharge test profiles

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

