# ✅ WOKWI INTEGRATION CHECKLIST - FINAL STEPS

**Date**: Jan 23, 2026  
**Status**: Ready for Browser Integration  
**Project URL**: https://wokwi.com/projects/453769002432989185

---

## 🎯 WHAT YOU NEED TO DO (IN ORDER)

### STEP 1: Verify Files in Wokwi ✅

1. **Go to Wokwi Project**: https://wokwi.com/projects/453769002432989185
2. **Check Files Panel** (left side):
   - ✅ `pBMSLSuite-O.ino` - Main firmware
   - ✅ `diagram.json` - Circuit configuration
3. **If files are missing**:
   - Click "+" → "Add File"
   - Upload both files from your local directory

---

### STEP 2: Add Stepper Motor Component 🤖

**This is the CRITICAL step - the firmware is ready, just needs the hardware component.**

1. **Click the "+" button** in Wokwi toolbar (top-left area)
2. **Search for**: `stepper motor`
3. **Select**: `Stepper Motor 28BYJ-48`
4. **Click** on the breadboard to place it (anywhere is fine)

**Expected Result**: A stepper motor component appears on your breadboard

---

### STEP 3: Connect 5 Wires - ESP32 to Stepper Motor 🔌

**These 5 connections are CRITICAL for the motor to work.**

| Wire # | From | To | Purpose |
|--------|------|----|---------| 
| 1️⃣ | ESP32 **D4** | Stepper **A1** | Coil A1 signal |
| 2️⃣ | ESP32 **D5** | Stepper **A2** | Coil A2 signal |
| 3️⃣ | ESP32 **D12** | Stepper **B1** | Coil B1 signal |
| 4️⃣ | ESP32 **D15** | Stepper **B2** | Coil B2 signal |
| 5️⃣ | ESP32 **D23** | Stepper **ENABLE** | Enable motor |

**How to Connect Each Wire**:
1. Click on the ESP32 pin (e.g., D4)
2. Drag to the stepper motor pin (e.g., A1)
3. Release to create connection
4. Repeat for all 5 connections

**Use ORANGE wires for easy identification** (you can set color when creating wire)

---

### STEP 4: Verify All Connections ✓

After making 5 connections, your breadboard should show:
```
ESP32 PINS CONNECTED:
├── D4  (GPIO 4)  ← Orange wire → Stepper A1  ✅
├── D5  (GPIO 5)  ← Orange wire → Stepper A2  ✅
├── D12 (GPIO 12) ← Orange wire → Stepper B1  ✅
├── D15 (GPIO 15) ← Orange wire → Stepper B2  ✅
└── D23 (GPIO 23) ← Orange wire → Stepper ENABLE  ✅
```

**No other wires should touch stepper motor** - it shouldn't connect to:
- ❌ Power (5V/GND) - handled by firmware PWM
- ❌ Other GPIO pins

---

### STEP 5: Start Simulation 🚀

1. **Click green "Start" button** (top-right in Wokwi)
2. **Wait for compilation** (should take 2-5 seconds)
3. **Simulation starts** - you should see:
   - ESP32 boots
   - LEDs on breadboard light up (shift register test)
   - Serial console shows output

---

### STEP 6: Test Motor with Serial Commands 📡

**Open Serial Console**: Click "Serial Monitor" button (top-right, monitor icon)

**Test 1: Start Motor (Low Speed)**
```
Type: LOAD 25
Press: ENTER
Expected: 
  ✅ Motor spins slowly
  ✅ Serial shows: "Stepper Motor: Running (25% load)"
  ✅ Min Cell SoC starts decreasing (~0.25% per minute)
```

**Test 2: Increase Speed**
```
Type: LOAD 100
Press: ENTER
Expected:
  ✅ Motor spins much faster
  ✅ Serial shows: "Stepper Motor: Running (100% load)"
  ✅ SoC decreases faster (~1.0% per minute)
```

**Test 3: Stop Motor**
```
Type: STOP LOAD
Press: ENTER
Expected:
  ✅ Motor stops spinning immediately
  ✅ Serial shows: "Stepper Motor: Stopped"
  ✅ SoC stops decreasing
```

---

## 🔍 TROUBLESHOOTING

### ❌ Motor Doesn't Spin
**Check List**:
- [ ] All 5 orange wires are connected (D4, D5, D12, D15, D23)
- [ ] Wires go to correct stepper pins (A1, A2, B1, B2, ENABLE)
- [ ] Simulation is running (green "Start" button active)
- [ ] Serial shows "Stepper Motor: Running" when LOAD command sent
- [ ] No error messages in compilation

**If still not working**:
1. Click "Stop" button
2. Remove all 5 wires by clicking wire → delete
3. Re-create connections more carefully
4. Start simulation again

### ❌ Serial Shows Errors
- [ ] Baud rate is 115200 (check Serial Monitor settings)
- [ ] Commands are UPPERCASE: `LOAD 50`, not `load 50`
- [ ] After each command, press ENTER
- [ ] Firmware compiled without errors

### ❌ Motor Spins But SoC Doesn't Decrease
- [ ] Wait 1-2 minutes (discharge happens over time)
- [ ] Check LED bar graph - does it show fewer LEDs? If yes, it's working
- [ ] Serial output should show "Min Cell SoC" decreasing
- [ ] At 100% load, should see 1% decrease per minute

### ❌ Simulation Won't Start
- [ ] Files were uploaded correctly (check Files panel)
- [ ] No compilation errors in console
- [ ] Try clicking "Stop" then "Start" again
- [ ] Refresh browser: Ctrl+R (Windows/Linux) or Cmd+R (Mac)

---

## 📊 EXPECTED SERIAL OUTPUT

### When Motor is Running at 50% Load
```
🔋 BATTERY STATUS:
   ⚙️  Stepper Motor: Running (50% load)
   Min Cell SoC: 89.3% | SoH: 97.8% | Load: 50%

========== BMS Status ==========
STATE: DISCHARGING (Active, 7 LEDs)
VOLTAGE:     4.0V  4.0V  4.0V  3.95V
TEMP:        25°C
Min Cell SoC: 87.6%
Fault Status: NONE
=================================
```

### When Motor is Stopped
```
🔋 BATTERY STATUS:
   ⚙️  Stepper Motor: Stopped
   Min Cell SoC: 85.2% | SoH: 97.5% | Load: 0%

========== BMS Status ==========
STATE: IDLE (Steady, 6 LEDs)
VOLTAGE:     3.8V  3.8V  3.8V  3.8V
TEMP:        25°C
Min Cell SoC: 85.2%
Fault Status: NONE
=================================
```

---

## ✅ INTEGRATION SUCCESS CRITERIA

All of these should be TRUE:

- [ ] Stepper motor component added to Wokwi
- [ ] 5 orange wires connected (D4, D5, D12, D15, D23)
- [ ] `LOAD 25` command makes motor spin slowly
- [ ] `LOAD 100` command makes motor spin fast
- [ ] `STOP LOAD` command stops motor
- [ ] Serial output shows motor status
- [ ] SoC decreases with motor running
- [ ] LED bar decreases as battery discharges
- [ ] No errors in console
- [ ] System runs for 5+ minutes without crashing

**When all ✅ are checked = INTEGRATION COMPLETE**

---

## 🎯 NEXT STEPS AFTER INTEGRATION

Once basic integration works, run the 6 comprehensive tests from **WOKWI_TEST_EXECUTION_PLAN.md**:

1. ✅ **Test 1**: Stepper Motor Load Control
2. ✅ **Test 2**: LED Bar Graph - Minimum Cell SoC
3. ✅ **Test 3**: Serial Output Validation
4. ✅ **Test 4**: Load-Aware State Transitions
5. ✅ **Test 5**: Fault Injection Under Load
6. ✅ **Test 6**: Comprehensive Load Scenarios

---

## 💾 PROJECT FILES

| File | Purpose | Status |
|------|---------|--------|
| `pBMSLSuite-O.ino` | Firmware (ready to upload) | ✅ Ready |
| `diagram.json` | Circuit config (ready to upload) | ✅ Ready |
| `MANUAL_STEPPER_INTEGRATION.md` | Detailed wiring guide | ✅ Reference |
| `WOKWI_TEST_EXECUTION_PLAN.md` | 6 comprehensive tests | ✅ Reference |

---

## 🚀 QUICK START (TL;DR)

1. Go to Wokwi project URL
2. Add stepper motor component
3. Connect 5 orange wires: D4→A1, D5→A2, D12→B1, D15→B2, D23→ENABLE
4. Click Start
5. Open Serial Monitor
6. Type: `LOAD 50` and press ENTER
7. See motor spin, SoC decrease, serial output show status

**That's it! Integration complete.**

---

## 📝 NOTES

- **Motor placement**: Doesn't matter where on breadboard
- **Wire color**: Use orange for visibility (or any color)
- **Simulation speed**: Use 1x for accurate timing
- **Serial baud**: Must be 115200
- **Commands**: Case-sensitive (LOAD, not load)

---

**Created**: Jan 23, 2026 - Ready for final Wokwi integration  
**All firmware code complete. Just need to connect hardware in Wokwi browser.**

