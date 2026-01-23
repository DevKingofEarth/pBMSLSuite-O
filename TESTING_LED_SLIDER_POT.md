# 🧪 LED Bar Graph & Slider Pot Testing Guide

## Overview
This document provides step-by-step instructions to test:
1. **LED Bar Graph** - Verifies 10-segment display responds to SoC changes
2. **Slider Potentiometer** - Verifies thermal stress injection works
3. **Fault Injection** - Verifies fault detection and LED response

---

## 📊 Test 1: LED Bar Graph SoC Response

### Setup
1. Open Wokwi simulation with updated `diagram.json`
2. Load `pBMSLSuite-O.ino` firmware
3. Open Serial Monitor (115200 baud)

### Test Procedure

#### Test 1a: Low SoC (0-20%)
**Expected**: 0-2 LED segments lit

1. **Set Cell Voltages LOW**:
   - Adjust **Pot 1, 2, 3, 4** to minimum (0V)
   - Expected: `Average: ~0.0V`
   - LED bar should show: **OFF** (0 segments)

2. **Observation**:
   - [ ] Serial output shows `SoC: 0.0%`
   - [ ] LED bar graph is completely dark
   - [ ] Status LED is ON (green/steady)

#### Test 1b: Medium SoC (40-60%)
**Expected**: 4-6 LED segments lit

1. **Set Cell Voltages MEDIUM**:
   - Adjust **Pot 1, 2, 3, 4** to middle position (~3.6V each)
   - Expected: `Average: ~3.6V`
   - LED bar should show: **5-6 segments**

2. **Observation**:
   - [ ] Serial output shows `SoC: ~50%`
   - [ ] LED bar graph shows 5-6 segments lit from bottom
   - [ ] Status LED remains ON

#### Test 1c: High SoC (80-100%)
**Expected**: 8-10 LED segments lit

1. **Set Cell Voltages HIGH**:
   - Adjust **Pot 1, 2, 3, 4** to maximum (~4.2V each)
   - Expected: `Average: ~4.2V`
   - LED bar should show: **10 segments** (fully lit)

2. **Observation**:
   - [ ] Serial output shows `SoC: ~100%`
   - [ ] LED bar graph shows all 10 segments lit
   - [ ] Status LED remains ON

#### Test 1d: Gradual Sweep (Dynamic Test)
**Expected**: Smooth progression from 0→10 LEDs

1. **Gradually adjust all pots** from 0V → 4.2V over 10 seconds
2. **Observation**:
   - [ ] LEDs light up progressively (0→1→2→3→...→10)
   - [ ] Each segment turns on at expected SoC threshold
   - [ ] No flickering or jumping

---

## 🌡️ Test 2: Slider Potentiometer Thermal Stress

### Setup
- Slider pot (VP pin) connected and functional
- Serial monitor showing temperature readings

### Test Procedure

#### Test 2a: Slider at 0% (No Stress)
**Expected**: Temperature = NTC reading

1. **Move slider to minimum (0%)**:
   - Expected: `Stress Injection (Slider): 0.0%`
   - Temperature should match NTC sensor reading
   - Example: `Temperature: 25.0°C` (room temperature)

2. **Observation**:
   - [ ] Serial output shows `Stress Injection: 0.0%`
   - [ ] Temperature matches ambient
   - [ ] No synthetic heating applied

#### Test 2b: Slider at 50% (Medium Stress)
**Expected**: Temperature += 30°C

1. **Move slider to middle position**:
   - Expected: `Stress Injection (Slider): 50.0%`
   - Temperature should increase by ~30°C
   - Example: If base is 25°C → now shows 55°C

2. **Observation**:
   - [ ] Serial output shows `Stress Injection: ~50%`
   - [ ] Temperature increases proportionally
   - [ ] SoH starts degrading (visible in next cycles)

#### Test 2c: Slider at 100% (Maximum Stress)
**Expected**: Temperature += 60°C (thermal limit)

1. **Move slider to maximum**:
   - Expected: `Stress Injection (Slider): 100.0%`
   - Temperature should increase by ~60°C
   - Example: If base is 25°C → now shows 85°C
   - **Trigger Over-Temperature Fault (OT > 60°C)**

2. **Observation**:
   - [ ] Serial output shows `Stress Injection: 100.0%`
   - [ ] Temperature shows 85°C (25 + 60)
   - [ ] FAULT state triggered
   - [ ] Status LED flashes red (fault indication)
   - [ ] Serial shows: `⚠️  FAULT: ❌ OVER-TEMPERATURE (>60°C)`

#### Test 2d: Real-time Stress Sweep
**Expected**: Smooth temperature increase as slider moves

1. **Slowly move slider from 0% → 100%** over 10 seconds
2. **Observation**:
   - [ ] Temperature smoothly increases
   - [ ] No jumps or drops
   - [ ] SoH degrades progressively
   - [ ] At 100%, OT fault triggers

---

## ⚠️ Test 3: Fault Injection & LED Response

### Setup
- Serial monitor ready for commands
- LED status visible

### Test 3a: Over-Temperature Fault via Slider
*Already tested in 2c - Slider at 100% triggers OT fault*

### Test 3b: Manual Fault Injection

#### Step 1: Enable Injection Mode
```
Type in Serial Monitor: ENABLE INJECTION
Expected Output: ✅ Fault Injection Mode ENABLED
```

#### Step 2: Inject Over-Voltage Fault
```
Type in Serial Monitor: INJECT OV
Expected:
- 🔬 Fault Injected: OVER-VOLTAGE (>4.25V) (Value: 4.50, Duration: 5000 ms)
- All cell voltages show 4.50V
- STATE: 🔴 FAULT - ACTION REQUIRED!
- Status LED flashes (on/off every 500ms)
```

**Observation**:
- [ ] Serial output shows fault injection
- [ ] Cell voltages jump to 4.50V
- [ ] Status LED flashes
- [ ] After 5 seconds, fault auto-clears
- [ ] System returns to IDLE

#### Step 3: Inject Under-Voltage Fault
```
Type in Serial Monitor: INJECT UV
Expected:
- Cell voltages show 2.50V
- STATE: 🔴 FAULT - ACTION REQUIRED!
```

#### Step 4: Inject Over-Temperature Fault
```
Type in Serial Monitor: INJECT OT
Expected:
- Temperature shows 75.0°C
- STATE: 🔴 FAULT - ACTION REQUIRED!
- OT threshold is 60°C
```

#### Step 5: Clear Injected Fault
```
Type in Serial Monitor: CLEAR FAULT
Expected: ✅ Injected fault manually cleared
```

#### Step 6: Disable Injection Mode
```
Type in Serial Monitor: DISABLE INJECTION
Expected: ✅ Fault Injection Mode DISABLED
```

---

## 🔧 Shift Register Verification

### Expected LED Bar Graph Circuit
- **SR1** (74HC595): Segments 0-7 (LEDs 1-8)
- **SR2** (74HC595): Segments 8-9 (LEDs 9-10)
- **Data Pin**: GPIO 14
- **Clock Pin**: GPIO 13 (fixed in this update)
- **Latch Pin**: GPIO 2 (fixed in this update)

### Visual Test
1. With all pots at maximum (4.2V):
   - [ ] All 10 LED segments light up
   - [ ] No segments missing
   - [ ] No segments stuck ON

2. With all pots at minimum (0V):
   - [ ] All LED segments turn off
   - [ ] No ghosting (faint glow)

---

## 📋 Complete Test Checklist

### LED Bar Graph
- [ ] Test 1a: Low SoC → 0-2 segments lit
- [ ] Test 1b: Medium SoC → 4-6 segments lit
- [ ] Test 1c: High SoC → 8-10 segments lit
- [ ] Test 1d: Smooth sweep 0→10 segments

### Slider Potentiometer
- [ ] Test 2a: 0% stress → normal temperature
- [ ] Test 2b: 50% stress → +30°C
- [ ] Test 2c: 100% stress → +60°C, triggers OT
- [ ] Test 2d: Smooth sweep with temperature increase

### Fault Injection
- [ ] Test 3a: Slider triggers OT fault
- [ ] Test 3b: Manual OV injection
- [ ] Test 3c: Manual UV injection
- [ ] Test 3d: Manual OT injection
- [ ] Test 3e: Clear fault
- [ ] Test 3f: Disable injection mode

### Shift Register Wiring
- [ ] Data line (GPIO 14) → SR1:DS
- [ ] Clock line (GPIO 13) → SR1:SHCP ✅ (FIXED)
- [ ] Latch line (GPIO 2) → SR1:STCP ✅ (FIXED)
- [ ] SR1 cascaded to SR2 properly
- [ ] All 10 LEDs respond correctly

---

## 🐛 Expected Issues & Fixes

| Issue | Cause | Solution |
|-------|-------|----------|
| LEDs not lighting at all | Shift register pins swapped | Verify GPIO 13→SHCP, GPIO 2→STCP |
| LEDs light but in wrong pattern | Bit ordering wrong | Check endianness in firmware (MSB-first) |
| Slider pot has no effect | VP pin not reading | Verify VP pin ADC reading |
| Temperature doesn't change with slider | stressInjection not applied | Check temperature calculation formula |
| Fault doesn't trigger | Threshold not met | Verify OT_THRESHOLD (60°C) |
| Status LED doesn't flash on fault | GPIO 22 issue | Check GPIO 22 connection to breadboard |

---

## 📝 Serial Output Example (Full Status)

```
pBMSLSuite-O STATUS

⏱️  Time: 45 seconds

───────────────────────────────────────────────────────────
🔋 CELL VOLTAGES:
   Cell 1: 3.600V ✅ OK
   Cell 2: 3.600V ✅ OK
   Cell 3: 3.600V ✅ OK
   Cell 4: 3.600V ✅ OK
   Average: 3.600V | Pack: 14.40V

───────────────────────────────────────────────────────────
🌡️  Temperature: 55.0°C ⚠️ WARM
   Stress Injection (Slider): 50.0% (0% = No Stress, 100% = +60°C)

───────────────────────────────────────────────────────────
🔋 BATTERY STATUS:
   SoC: 50.0% | SoH: 99.8%

───────────────────────────────────────────────────────────
⚙️  STATE: 🟢 IDLE (Ready)
   Balancing: ⏸️  IDLE

───────────────────────────────────────────────────────────
⚠️  FAULT: ✅ NONE

───────────────────────────────────────────────────────────
📊 THRESHOLDS:
   OV: 4.25V | UV: 2.80V | OT: 60.0°C | UT: 0.0°C

═══════════════════════════════════════════════════════════
```

---

## 🎯 Success Criteria

**All tests pass when:**
1. ✅ LED bar responds correctly to SoC changes (0→10 segments)
2. ✅ Slider pot reads 0-100% stress injection
3. ✅ Temperature increases linearly with slider (0.6°C per 1% stress)
4. ✅ Fault injection triggers at thresholds
5. ✅ Shift register wiring is verified correct
6. ✅ Status LED indicates system state (steady=OK, flash=FAULT)

