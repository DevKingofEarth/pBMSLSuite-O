# 🔄 Cell Balancing Test Guide

**Test Date:** 2026-01-23  
**Firmware Version:** v1.0+FaultInjection  
**Platform:** Wokwi Simulation

---

## 📋 Balancing Logic Overview

### How Cell Balancing Works

The firmware implements **selective cell balancing** to equalize voltages across all 4 cells:

1. **Monitoring Phase:** Reads all cell voltages continuously
2. **Detection Phase:** Identifies cells > BALANCE_THRESHOLD (4.1V)
3. **Activation Phase:** Enables balance resistors for high cells
4. **Dissipation Phase:** High cells dissipate energy through resistors
5. **Equalization Phase:** Cell voltages converge
6. **Completion Phase:** Returns to IDLE when all cells < 4.1V

### Firmware Implementation

```cpp
// Lines 189-219: State machine with balancing logic

if (cellVoltages[i] > BALANCE_THRESHOLD) needsBalancing = true;

// Selective balancing - only balance cells > 4.1V
digitalWrite(BALANCE1_PIN, (cellVoltages[0] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
digitalWrite(BALANCE2_PIN, (cellVoltages[1] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
digitalWrite(BALANCE3_PIN, (cellVoltages[2] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
digitalWrite(BALANCE4_PIN, (cellVoltages[3] > BALANCE_THRESHOLD && currentState == BALANCING) ? HIGH : LOW);
```

### Key Parameters

| Parameter | Value | Notes |
|-----------|-------|-------|
| BALANCE_THRESHOLD | 4.1V | Trigger threshold |
| Balance Resistors | 100Ω (4×) | Energy dissipation |
| GPIO Pins | 16-19 | Digital outputs (HIGH = active) |
| Hysteresis | 0.0V | No hysteresis (potential fix) |

---

## 🧪 Test Suite: Balancing Logic

### Test 3.1: Balanced Pack (No Balancing Needed)

**Setup:**
```
Cell 1: 3.3V
Cell 2: 3.3V  
Cell 3: 3.3V
Cell 4: 3.3V
```

**Expected:**
- All cells = 3.3V (within safe range)
- State: IDLE 🟢
- Balancing pins: ALL LOW (inactive)
- Serial: "Balancing: ⏸️ IDLE"

**How to Test in Wokwi:**
1. Set all 4 potentiometers to middle position (~50%)
2. Observe serial output
3. Verify state is IDLE
4. Check balance pins remain LOW

**Success Criteria:**
- ✅ State = IDLE
- ✅ All balance pins = LOW
- ✅ No balancing activity reported

---

### Test 3.2: Single Cell High (Selective Balancing)

**Setup:**
```
Cell 1: 4.2V ← HIGH
Cell 2: 3.3V
Cell 3: 3.3V
Cell 4: 3.3V
```

**Expected:**
- Cell 1 > 4.1V threshold
- State: BALANCING 🟠
- Balance Pin 1: HIGH (active)
- Balance Pins 2-4: LOW (inactive)
- Serial: "Balancing: 🔄 ACTIVE"

**How to Test in Wokwi:**
1. Set Cell1 potentiometer to ~88% (≈4.2V)
2. Set Cells 2-4 to ~50% (≈3.3V)
3. Wait 1 second for settling
4. Observe state change to BALANCING
5. Verify only Balance Pin 1 = HIGH

**Success Criteria:**
- ✅ State transitions to BALANCING
- ✅ BALANCE1_PIN (GPIO 16) = HIGH
- ✅ BALANCE2_PIN (GPIO 17) = LOW
- ✅ BALANCE3_PIN (GPIO 18) = LOW
- ✅ BALANCE4_PIN (GPIO 19) = LOW
- ✅ Serial shows "🔄 ACTIVE"

**Physics Check:**
- 100Ω resistor with 4.2V: P = V²/R = (4.2)² / 100 = 176mW ✅
- Current through resistor: I = 4.2 / 100 = 42mA ✅
- Over ~5 minutes: ΔQ = 42mA × 5min = ~210mAh dissipated

---

### Test 3.3: Multiple Cells High (Multi-Cell Balancing)

**Setup:**
```
Cell 1: 4.15V ← HIGH
Cell 2: 4.15V ← HIGH
Cell 3: 3.5V  ← NORMAL
Cell 4: 3.5V  ← NORMAL
```

**Expected:**
- Cells 1,2 > 4.1V
- State: BALANCING 🟠
- Balance Pins 1,2: HIGH (active)
- Balance Pins 3,4: LOW (inactive)
- Serial: "Balancing: 🔄 ACTIVE"

**How to Test in Wokwi:**
1. Set Cell1 pot to ~82% (≈4.15V)
2. Set Cell2 pot to ~82% (≈4.15V)
3. Set Cell3 pot to ~60% (≈3.5V)
4. Set Cell4 pot to ~60% (≈3.5V)
5. Observe dual balancing

**Success Criteria:**
- ✅ State = BALANCING
- ✅ BALANCE1_PIN = HIGH
- ✅ BALANCE2_PIN = HIGH
- ✅ BALANCE3_PIN = LOW
- ✅ BALANCE4_PIN = LOW
- ✅ Serial shows "🔄 ACTIVE"

---

### Test 3.4: All Cells High (Full Pack Balancing)

**Setup:**
```
Cell 1: 4.2V ← HIGH
Cell 2: 4.18V ← HIGH
Cell 3: 4.19V ← HIGH
Cell 4: 4.17V ← HIGH
```

**Expected:**
- All cells > 4.1V
- State: BALANCING 🟠
- All balance pins: HIGH
- Rapid voltage equalization
- Serial: "Balancing: 🔄 ACTIVE"

**How to Test in Wokwi:**
1. Set all pots to ~85-87% range (4.15-4.2V)
2. Observe all 4 balance pins go HIGH
3. Watch voltages slowly converge
4. Monitor transition back to IDLE

**Success Criteria:**
- ✅ All 4 balance pins = HIGH
- ✅ State = BALANCING
- ✅ All cells dissipating power
- ✅ Voltages converge toward 4.1V

---

### Test 3.5: Balancing Completion (Return to IDLE)

**Setup:**
1. Start with Test 3.4 (all cells high, balancing active)
2. Slowly lower all potentiometers together
3. Watch voltages decrease below 4.1V

**Expected:**
- Cells drop below 4.1V
- State: BALANCING → IDLE 🟢
- All balance pins: HIGH → LOW
- Serial: "Balancing: ⏸️ IDLE"

**How to Test in Wokwi:**
1. From Test 3.4 state, slowly turn all 4 pots down
2. Watch for state transition
3. Verify all balance pins turn OFF
4. Observe return to IDLE state

**Success Criteria:**
- ✅ State transitions BALANCING → IDLE
- ✅ All balance pins go LOW
- ✅ No balancing activity after transition
- ✅ System stable in IDLE

---

### Test 3.6: Balancing with Fault (Fault Takes Priority)

**Setup:**
1. Start balancing (all cells > 4.1V, State = BALANCING)
2. Inject OV fault (Cell > 4.25V)

**Expected:**
- Fault detected immediately
- State: BALANCING → FAULT 🔴
- All balance pins: LOW (stop balancing)
- Red LED flashes rapidly
- Serial: "❌ OVER-VOLTAGE"

**How to Test in Wokwi:**
1. Use fault injection: `INJECT OV` in serial terminal
2. Observe state transition to FAULT
3. Verify all balance pins turn OFF immediately
4. Check LED flashes red

**Success Criteria:**
- ✅ State transitions to FAULT
- ✅ All balance pins disabled
- ✅ LED enters fault mode
- ✅ Balancing paused during fault

---

### Test 3.7: Balancing Recovery from Fault

**Setup:**
1. From Test 3.6 (in FAULT state due to OV)
2. Clear the fault
3. Return to balancing conditions

**Expected:**
- Fault cleared
- State: FAULT → IDLE 🟢
- If cells still > 4.1V: IDLE → BALANCING 🟠
- Balance resumes

**How to Test in Wokwi:**
1. With OV fault active, use: `CLEAR FAULT`
2. Watch state transition back to IDLE
3. If cells still high, state transitions to BALANCING
4. Balance pins activate again

**Success Criteria:**
- ✅ Fault clears successfully
- ✅ State returns to IDLE or BALANCING appropriately
- ✅ Balancing resumes if needed
- ✅ Smooth recovery without oscillation

---

## 📊 Expected Balancing Performance

### Voltage Equalization Timeline

With 100Ω balance resistors and 4.2V → 4.1V drop:

| Phase | Time | Cell 1 | Cell 2 | Status |
|-------|------|--------|--------|--------|
| Start | 0s   | 4.20V  | 3.20V  | Imbalanced |
| Initial | 30s  | 4.18V  | 3.22V  | Balancing |
| Mid    | 2min | 4.10V  | 3.30V  | Converging |
| Final  | 5min | 4.08V  | 3.32V  | Balanced |
| Idle   | >5min| 4.08V  | 3.32V  | IDLE state |

**Energy Dissipated (Simulation):**
- Cell 1: ~210mAh at 4.15V avg
- Total pack: ~210mAh × 4 cells = 840mAh

---

## 🔍 Pin Verification Checklist

### Balance Control Pins

| GPIO | Cell | Balancer | Test |
|------|------|----------|------|
| 16   | 1    | R2 (100Ω)| [ ] |
| 17   | 2    | R3 (100Ω)| [ ] |
| 18   | 3    | R4 (100Ω)| [ ] |
| 19   | 4    | R5 (100Ω)| [ ] |

**Verification Steps:**
1. [ ] Set all cells high (> 4.1V)
2. [ ] Monitor each GPIO pin individually
3. [ ] Confirm HIGH when corresponding cell > 4.1V
4. [ ] Confirm LOW when corresponding cell < 4.1V

---

## 🐛 Troubleshooting

### Issue: Balance pins never activate

**Diagnosis:**
- Potentiometers stuck at low voltage
- Firmware not reading ADC correctly
- GPIO pin not configured as output

**Solution:**
1. Verify potentiometer position (should be > 80% for > 4.1V)
2. Check `pinMode()` is called in `init()` (Line 91-93)
3. Verify firmware compiles without errors
4. Test single GPIO pin isolation

---

### Issue: Balance pins stuck ON

**Diagnosis:**
- Potentiometer stuck high
- Firmware stuck in BALANCING state
- Over-voltage fault preventing return to IDLE

**Solution:**
1. Manually lower potentiometers below 4.1V
2. Check fault status (is OV_THRESHOLD triggered?)
3. Use `CLEAR FAULT` if fault active
4. Restart simulation if stuck

---

### Issue: Balancing doesn't stop when cells are balanced

**Diagnosis:**
- Hysteresis issue (cells at exactly 4.1V)
- Floating-point comparison precision
- Threshold not being checked correctly

**Solution:**
1. Lower cells slightly below 4.1V (e.g., 4.08V)
2. Check line 204: `if (!needsBalancing) currentState = IDLE;`
3. Consider adding 0.02V hysteresis for stability

---

## ✅ Test Completion Summary

| Test | Status | Notes |
|------|--------|-------|
| 3.1 - Balanced Pack | ⏳ | No balancing needed |
| 3.2 - Single Cell High | ⏳ | Selective balancing |
| 3.3 - Multiple Cells High | ⏳ | Multi-cell balancing |
| 3.4 - All Cells High | ⏳ | Full pack balancing |
| 3.5 - Balancing Completion | ⏳ | Return to IDLE |
| 3.6 - Fault During Balancing | ⏳ | Fault priority |
| 3.7 - Recovery from Fault | ⏳ | Resume balancing |

**Pass Rate:** 0/7 (⏳ Pending Wokwi execution)

---

## 📝 Execution Log

### Session 1: Initial Testing (2026-01-23)

```
Time: 12:42 PM IST
Status: Ready for Wokwi simulation
Tests 3.1-3.7: Pending
```

---

## 🔗 References

- **Source Code:** Lines 174-223 (State machine & balancing)
- **Pin Config:** Lines 11-14 (Balance pins GPIO 16-19)
- **Threshold:** Line 30 (BALANCE_THRESHOLD = 4.1V)
- **Wokwi Project:** https://wokwi.com/projects/453769002432989185

