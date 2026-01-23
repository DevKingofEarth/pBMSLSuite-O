# 🔧 MANUAL STEPPER MOTOR INTEGRATION GUIDE

**Date**: Jan 23, 2026  
**Purpose**: Add stepper motor to Wokwi without corrupting diagram.json  
**Status**: Ready for manual integration

---

## 📋 WHY MANUAL INTEGRATION?

The diagram.json file was getting corrupted when I tried to add the stepper motor programmatically. **Manual integration in Wokwi browser is safer and more reliable.**

---

## 🎯 WHAT YOU NEED TO DO

### Step 1: Upload Clean Files to Wokwi
1. Open [Wokwi Project](https://wokwi.com/projects/453769002432989185)
2. **Delete** any existing files
3. **Upload** `pBMSLSuite-O.ino` (contains stepper motor code)
4. **Upload** `diagram.json` (clean version, stepper motor removed)

### Step 2: Add Stepper Motor Component
1. Click the **"+" button** in the Wokwi toolbar (Add Components)
2. **Search for**: `stepper motor`
3. **Select**: `Stepper Motor` (28BYJ-48 type)
4. **Click** on the breadboard to place it
5. **Position**: Place it in an empty area (doesn't affect functionality)

### Step 3: Make the Connections
Connect these 5 wires from ESP32 to stepper motor:

| ESP32 Pin | Stepper Motor Pin | Wire Color | Purpose |
|-----------|-------------------|------------|---------|
| **D4** (GPIO 4) | **A1** | Orange | Coil A1 |
| **D5** (GPIO 5) | **A2** | Orange | Coil A2 |
| **D12** (GPIO 12) | **B1** | Orange | Coil B1 |
| **D15** (GPIO 15) | **B2** | Orange | Coil B2 |
| **D23** (GPIO 23) | **ENABLE** | Orange | Motor enable |

#### How to Connect:
1. **Click** on the ESP32 pin (e.g., D4)
2. **Drag** the wire to the stepper motor pin (A1)
3. **Repeat** for all 5 connections
4. **Use orange wires** for easy identification

### Step 4: Verify Connections
Your stepper motor should have 5 orange wires connected:
- ESP32 D4 → Stepper A1
- ESP32 D5 → Stepper A2
- ESP32 D12 → Stepper B1
- ESP32 D15 → Stepper B2
- ESP32 D23 → Stepper ENABLE

---

## 🧪 TESTING THE INTEGRATION

### Basic Test
1. **Open Serial Monitor** (115200 baud, top-right in Wokwi)
2. **Type**: `LOAD 25` (press Enter)
3. **Expected Results**:
   - ✅ Stepper motor starts spinning slowly
   - ✅ Serial output shows: `Stepper Motor: Running (25% load)`
   - ✅ SoC starts decreasing very slowly

### Speed Test
1. **Type**: `LOAD 100`
2. **Expected**: Motor spins much faster, SoC decreases 1% per minute

### Stop Test
1. **Type**: `STOP LOAD`
2. **Expected**: Motor stops, discharge halts

---

## 🔍 TROUBLESHOOTING

### Motor Doesn't Spin
- **Check connections**: All 5 wires must be connected correctly
- **Check pins**: Make sure you're using D4, D5, D12, D15, D23 (not other pins)
- **Check serial**: If you see "Stepper Motor: Running" but no spin, connections are wrong

### Serial Shows Errors
- **Check baud rate**: Must be 115200
- **Check command**: Must be `LOAD 50` (not `load 50`)
- **Check firmware**: Make sure `pBMSLSuite-O.ino` is uploaded

### Motor Spins But No Discharge
- **Check SoC display**: Look for "Min Cell SoC" in serial output
- **Wait**: Discharge takes time (1% per minute at 100% load)
- **Check load level**: Higher load = faster discharge

---

## 📊 EXPECTED BEHAVIOR

### Normal Operation
```
🔋 BATTERY STATUS:
   ⚙️  Stepper Motor: Running (50% load)
   Min Cell SoC: 87.3% | SoH: 95.2% | Load: 50%
```

### Stopped Operation
```
🔋 BATTERY STATUS:
   ⚙️  Stepper Motor: Stopped
   Min Cell SoC: 87.3% | SoH: 95.2% | Load: 0%
```

### Discharge Rate
- **LOAD 0**: No discharge
- **LOAD 25**: 0.25% SoC per minute
- **LOAD 50**: 0.5% SoC per minute
- **LOAD 100**: 1.0% SoC per minute

---

## 🎯 WHAT THIS ACHIEVES

### Before (Without Stepper)
- LED bar shows average voltage (meaningless)
- No real load testing
- Fake features removed

### After (With Stepper)
- **Real current consumption** (motor draws actual power)
- **Realistic discharge curves** (SoC drops proportionally to load)
- **Load testing capability** (0-100% variable load)
- **Engineering demonstration** (real BMS load testing)

---

## 📁 FILES STATUS

### Ready to Upload
- ✅ `pBMSLSuite-O.ino` - Contains stepper motor code
- ✅ `diagram.json` - Clean, stepper motor removed

### Reference Documents
- ✅ `STEPPER_MOTOR_LOAD_TESTING.md` - Technical details
- ✅ `TRANSFORMATION_SUMMARY.md` - Before/after comparison
- ✅ `MANUAL_STEPPER_INTEGRATION.md` - This guide

---

## 🚀 QUICK START CHECKLIST

- [ ] Open Wokwi project
- [ ] Upload `pBMSLSuite-O.ino`
- [ ] Upload `diagram.json`
- [ ] Add Stepper Motor component
- [ ] Connect 5 orange wires (D4,D5,D12,D15,D23)
- [ ] Open Serial Monitor (115200 baud)
- [ ] Type `LOAD 50`
- [ ] Verify motor spins and SoC decreases

---

## 💡 PRO TIP

**Test incrementally**:
1. First test: `LOAD 25` (slow, easy to verify)
2. Speed test: `LOAD 100` (fast, clear difference)
3. Stop test: `STOP LOAD` (confirms control works)

**Monitor serial output** - it tells you exactly what's happening.

---

## ✅ SUCCESS CRITERIA

- ✅ Stepper motor spins when you type `LOAD X`
- ✅ Serial shows motor status and load percentage
- ✅ SoC decreases over time (proportional to load)
- ✅ `STOP LOAD` halts everything
- ✅ LED bar shows minimum cell SoC

---

**Ready to integrate! Follow the steps above and you'll have real load testing capability.**

**Let me know if you encounter any issues during integration.**

---

*Manual Integration Guide - Jan 23, 2026*

