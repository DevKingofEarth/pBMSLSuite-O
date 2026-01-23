# 🧪 Wokwi Testing Guide - Fixed Issues

**Issues Fixed:**
- ✅ GPIO 2 conflict resolved (moved status LED to GPIO 22)
- ✅ LED bar graph control code added
- ✅ Shift register pins properly configured

---

## 🔧 What Was Wrong

### 1. **LED Bar Graph Not Working**
**Problem:** No firmware code to control shift registers
**Solution:** Added `updateLEDbarGraph()` function with 16-bit shift register control

### 2. **GPIO 2 Conflict**
**Problem:** GPIO 2 used for both status LED AND shift register clock
**Solution:** Moved status LED to GPIO 22, shift register clock stays on GPIO 2

### 3. **Only UV & Temperature Faults**
**Problem:** Voltage pots in Wokwi only go to ~3.3V, not high enough for OV faults
**Solution:** Need to adjust potentiometer ranges or use different test method

---

## 🧪 Test the Fixes

### Test 1: LED Bar Graph (SoC Display)

**Expected:** LED bar graph shows battery level based on SoC

1. **Start Simulation**
2. **Set all 4 cell pots to ~50%** (should show ~50% SoC)
3. **Watch LED bar graph:** Should show ~5 segments lit
4. **Change pots to 80%:** Should show ~8 segments lit
5. **Change pots to 20%:** Should show ~2 segments lit

**If not working:** Check Wokwi console for "⚙️ VOLTAGE MODE: Direct ADC" message

### Test 2: Status LED (Fault Indication)

**Expected:** Orange LED (GPIO 22) shows fault state

1. **Normal operation:** LED should be ON (solid)
2. **Fault condition:** LED should blink (500ms on/off)
3. **Test with UV fault:** Set cell pots very low (<20%)

### Test 3: Fault Injection Testing

**Open Serial Monitor** in Wokwi (115200 baud)

**Test Commands:**
```
ENABLE INJECTION
INJECT OV    # Should work now (voltage override)
INJECT UV    # Already worked
INJECT OT    # Already worked  
INJECT UT    # Already worked
CLEAR FAULT  # Clear injected faults
DISABLE INJECTION
```

### Test 4: Cell Balancing

**Expected:** When cells >4.1V, balancing activates

**Problem:** Wokwi pots only go to 3.3V, but balancing threshold is 4.1V
**Workaround:** Use fault injection to test:
```
ENABLE INJECTION
INJECT OV  # Sets all cells to 4.5V (above balance threshold)
```

**Should see:**
- State changes to BALANCING 🟠
- Balance pins (GPIO 16-19) go HIGH
- Serial shows "🔄 ACTIVE"

### Test 5: Over-Voltage Faults

**Expected:** When cells >4.25V, OV fault triggers

**Problem:** Wokwi pots max at 3.3V
**Solution:** Use fault injection:
```
ENABLE INJECTION
INJECT OV  # Sets cells to 4.5V
```

**Should see:**
- State changes to FAULT 🔴
- Serial shows "❌ OVER-VOLTAGE"
- Status LED blinks
- Balance pins stay LOW (fault overrides balancing)

---

## 📊 Expected Wokwi Behavior Now

### Serial Output Pattern
```
pBMSLSuite-O v1.0 - Portable BMS Lab Suite
==========================================
⚙️  VOLTAGE MODE: Direct ADC (0-3.3V range - Wokwi Simulation)

═══════════════════════════════════════════════════════════
                    pBMSLSuite-O STATUS                     
═══════════════════════════════════════════════════════════
⏱️  Time: 1200 seconds
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
   SoC: 100.0% | SoH: 100.0%
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

### Visual Indicators
- **LED Bar Graph:** Shows SoC level (0-10 segments)
- **Orange LED:** Solid ON (normal), blinking (fault)
- **Balance LEDs:** Light up when balancing active

---

## 🔍 Troubleshooting

### If LED Bar Graph Still Not Working

1. **Check Serial:** Should show "⚙️ VOLTAGE MODE: Direct ADC"
2. **Check SoC:** LED bar follows SoC percentage
3. **Test Pots:** Try different positions (0%, 50%, 100%)
4. **Check Connections:** Verify Wokwi diagram has SR1/SR2 connected

### If Status LED Not Working

1. **Check GPIO:** Now uses GPIO 22 (orange LED in Wokwi)
2. **Fault Test:** Try `INJECT UV` in serial
3. **Blink Pattern:** Should blink during faults

### If OV Faults Still Not Triggering

1. **Wokwi Limitation:** Pots max at 3.3V
2. **Use Injection:** `ENABLE INJECTION` then `INJECT OV`
3. **Voltage Override:** Injection sets cells to 4.5V

### If Balancing Not Working

1. **Threshold:** Needs cells >4.1V
2. **Injection:** Use `INJECT OV` (sets to 4.5V)
3. **State Check:** Should show BALANCING 🟠

---

## ✅ Success Criteria

**Test passes if you can see:**

1. ✅ **LED Bar Graph** responds to SoC changes
2. ✅ **Status LED** blinks during faults  
3. ✅ **OV Faults** trigger with `INJECT OV`
4. ✅ **Balancing** activates with `INJECT OV`
5. ✅ **Serial Output** shows all data correctly

---

## 🚀 Ready for Testing!

The firmware now properly controls:
- LED bar graph (SoC display)
- Status LED (fault indication)  
- Shift registers (no more conflicts)

Upload the updated `pBMSLSuite-O.ino.bin` to Wokwi and test!