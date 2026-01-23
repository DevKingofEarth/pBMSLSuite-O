# 🎨 Wokwi Circuit Diagram - Enhancement Guide

**Last Updated:** 2026-01-23  
**Firmware Version:** v1.0+VoltageDiv+INA219  
**Status:** Ready for enhancement

---

## 📋 Current Wokwi Setup

### What's Implemented

**Main Components:**
- ✅ ESP32 Dev Module
- ✅ 4× Potentiometers (Cell voltage inputs, 0-3.3V)
- ✅ 1× NTC Thermistor (Temperature input)
- ✅ 1× Voltage divider (10kΩ - 10kΩ for NTC)
- ✅ 4× Balance resistors (100Ω each)
- ✅ 2× Status LEDs (Red, Orange)
- ✅ 1× 10-segment LED bar graph
- ✅ 2× 74HC595 Shift registers (for LED driving)
- ✅ 1× Load slider potentiometer

**Circuit Status:**
- Voltage Range: 0-3.3V (simulation only)
- Current: Not measured (load via potentiometer)
- Load: Manual slider control (pot5)
- Balancing: GPIO 16-19 outputs to LED bar

---

## 🔌 Proposed Enhancements

### Enhancement 1: Voltage Dividers for Cell Measurement

**Current Setup:**
```
Pot1 → ESP32 GPIO34 (0-3.3V range)
```

**Proposed Setup (for hardware testing):**
```
Pot1 (0-5V simulation)
  │
  ├─→ R1 (10kΩ)
  │
  ├─→ (Point A) → ESP32 GPIO34
  │
  ├─→ R2 (15kΩ)
  │
  GND

Where: V_measured = V_in × (15k / 25k) = V_in × 0.6
```

**Components to Add:**
- 8× Resistors: 4× 10kΩ (R1 for each cell) + 4× 15kΩ (R2 for each cell)

**Note:** This is optional for Wokwi since potentiometers already output 0-3.3V  
Use `USE_VOLTAGE_DIVIDER = 0` in firmware for simulation

---

### Enhancement 2: INA219 Current Sensor Module

**Current Setup:**
```
Load controlled by: Pot5 (slider, GPIO36)
Current measurement: None
```

**Proposed Setup:**
```
ESP32 ← I2C Bus → INA219 Module
│       ├─ GPIO21 (SDA)
│       └─ GPIO22 (SCL)
│
Battery Pack ← Shunt → INA219
```

**INA219 Features:**
- Measures bus voltage (0-32V)
- Measures shunt current (0-3.2A)
- Calculates power (W)
- I2C communication
- Address: 0x40 (with A0/A1 to GND)

---

## 📐 Circuit Design Details

### Voltage Divider Circuit (Per Cell)

```
Wokwi Potentiometer Output (Simulated Cell: 0-3.3V)
         │
         R1 (10kΩ)
         │
    +────┴────+
    │         │
    └─ESP32   │
      (ADC)   │
             R2 (15kΩ)
              │
              GND
```

**Component List (for all 4 cells):**

| Cell | R1 Value | R2 Value | Purpose |
|------|----------|----------|---------|
| 1    | 10kΩ     | 15kΩ     | Voltage scaling |
| 2    | 10kΩ     | 15kΩ     | Voltage scaling |
| 3    | 10kΩ     | 15kΩ     | Voltage scaling |
| 4    | 10kΩ     | 15kΩ     | Voltage scaling |

**Total Resistors:** 8 (4 top + 4 bottom)

---

### INA219 Connection Diagram

```
ESP32 Pins               INA219 Module
┌──────────┐            ┌──────────┐
│ GPIO21   ├───SDA──────┤ SDA      │
│ GPIO22   ├───SCL──────┤ SCL      │
│ GND      ├────────────┤ GND      │
│ +3.3V    ├────────────┤ VDD      │
└──────────┘            │          │
                        │ A0→GND   │ (0x40 address)
                        │ A1→GND   │
                        └──────────┘
                             │
                         Shunt
                        ┌─────────┐
Battery ─────────────→ │IN+│ INA │ OUT ─→ Device
Pack              0.1Ω  │IN-│ 219│
                        └─────────┘
```

---

## 🛠️ Step-by-Step Modification Guide

### For Wokwi Circuit Enhancement

#### Step 1: Add Voltage Divider Resistors (Optional)

1. Open Wokwi Circuit Editor
2. Add resistor for each cell voltage divider:
   - 4× 10kΩ resistors (top of dividers)
   - 4× 15kΩ resistors (bottom of dividers)
3. Connect between potentiometers and ADC pins
4. Update diagram.json with new component entries

**Expected JSON Addition:**
```json
{
  "type": "wokwi-resistor",
  "id": "r_divider_1_top",
  "attrs": { "value": "10000" }
},
{
  "type": "wokwi-resistor",
  "id": "r_divider_1_bottom",
  "attrs": { "value": "15000" }
}
```

#### Step 2: Add INA219 Module (Simulation)

**Note:** INA219 module may not be directly available in Wokwi  
Alternative: Create placeholder or document expected connections

1. Add I2C lines between ESP32 and labeled INA219 position
2. Document I2C pins (GPIO21=SDA, GPIO22=SCL)
3. Add conceptual connections in diagram

#### Step 3: Update diagram.json

1. Add new component definitions
2. Add new connections
3. Adjust layout for new components
4. Test in Wokwi editor

---

## 📝 Detailed JSON Changes

### New Components to Add

#### For Voltage Dividers (Cell 1 as example)

```json
{
  "type": "wokwi-resistor",
  "id": "r_cell1_div_top",
  "top": 180,
  "left": 300,
  "rotate": 90,
  "attrs": { "value": "10000" }
},
{
  "type": "wokwi-resistor",
  "id": "r_cell1_div_bottom",
  "top": 220,
  "left": 300,
  "rotate": 90,
  "attrs": { "value": "15000" }
}
```

**Repeat for Cells 2, 3, 4**  
**Change left position:** 355, 410, 465 (for each cell)

#### For INA219 I2C Connections

```json
{
  "type": "wokwi-resistor",
  "id": "r_i2c_sda_pullup",
  "top": -120,
  "left": 150,
  "attrs": { "value": "4700" }
},
{
  "type": "wokwi-resistor",
  "id": "r_i2c_scl_pullup",
  "top": -120,
  "left": 200,
  "attrs": { "value": "4700" }
}
```

### New Connections to Add

#### Voltage Dividers (Cell 1 as example)

```json
[ "pot1:SIG", "r_cell1_div_top:1", "green", [] ],
[ "r_cell1_div_top:2", "r_cell1_div_bottom:1", "green", [] ],
[ "r_cell1_div_bottom:2", "bb1:bn.5", "black", [] ],
[ "r_cell1_div_top:2", "esp:34", "green", [] ]
```

#### I2C Lines

```json
[ "esp:21", "r_i2c_sda_pullup:1", "yellow", [] ],
[ "r_i2c_sda_pullup:2", "bb1:bp.1", "red", [] ],
[ "esp:22", "r_i2c_scl_pullup:1", "yellow", [] ],
[ "r_i2c_scl_pullup:2", "bb1:bp.1", "red", [] ]
```

---

## ✅ Verification Checklist

### Before Deployment

- [ ] All voltage dividers connected correctly
- [ ] I2C pull-up resistors added (4.7kΩ)
- [ ] I2C connections to GPIO21/22
- [ ] Circuit compiles in Wokwi
- [ ] Visual layout is clean and organized
- [ ] Component values verified (10k, 15k, 4.7k)
- [ ] Connections match firmware expectations
- [ ] No floating pins or dangling wires

### After Testing

- [ ] Wokwi simulation runs without errors
- [ ] ESP32 connects to INA219 (I2C OK)
- [ ] Voltage readings match expectations
- [ ] Current measurements (simulated or real)
- [ ] LED bar graph responds to changes
- [ ] Serial output shows all data

---

## 📊 Comparison: Current vs Enhanced

| Feature | Current | With Dividers | With INA219 |
|---------|---------|---------------|------------|
| Cell Voltages | 0-3.3V | 0-5V (hw) | 0-5V |
| Temperature | Yes | Yes | Yes |
| Current | No | No | Yes |
| Power | No | No | Yes |
| I2C Devices | 0 | 0 | 1 |
| Total Components | 30 | 38 | 40+ |
| Firmware Mode | Direct | Divider | Full |

---

## 🚀 Implementation Timeline

### Phase 1: Current (✅ Complete)
- [x] Firmware supports voltage divider flag
- [x] Firmware has INA219 placeholder
- [x] Wokwi circuit working
- Status: Complete

### Phase 2: Enhancement (⏳ Ready)
- [ ] Add voltage divider resistors to Wokwi
- [ ] Update diagram.json with new components
- [ ] Add I2C lines for INA219
- [ ] Test in Wokwi simulator
- [ ] Document all changes

### Phase 3: Hardware (⏳ Future)
- [ ] Build physical circuit with dividers
- [ ] Install INA219 hardware module
- [ ] Enable USE_VOLTAGE_DIVIDER = 1
- [ ] Enable USE_INA219 = 1
- [ ] Test on real ESP32 + hardware

---

## 📞 Reference Information

### ESP32 I2C Pins
- **SDA:** GPIO21
- **SCL:** GPIO22
- **Speed:** 400kHz standard
- **Voltage:** 3.3V (no level shifting needed)

### INA219 Default Address
- **Address:** 0x40
- **A0 Pin:** GND
- **A1 Pin:** GND

### Voltage Divider Calculations

For each cell:
```
V_out = V_in × (R2 / (R1 + R2))
      = V_in × (15kΩ / 25kΩ)
      = V_in × 0.6
```

**Scaling factor in firmware:** 1.667 (1 / 0.6)

---

## ✅ Completion Status

**Firmware Support:**
- [x] Configuration flags added
- [x] Voltage divider calculation ready
- [x] INA219 placeholder implemented
- [x] Serial output prepared

**Wokwi Circuit:**
- [x] Current circuit complete and working
- [ ] Voltage dividers added (ready for implementation)
- [ ] INA219 connections (ready for implementation)

**Next Steps:**
1. Add voltage divider resistors to Wokwi diagram
2. Add I2C pull-up resistors
3. Add INA219 module connections
4. Test in Wokwi simulator
5. Validate all measurements

---

## 🔗 Files Involved

- **Firmware:** `/home/dharrun/Projects/Local_Projects/pBMSLSuite-O/pBMSLSuite-O.ino`
- **Circuit:** `/home/dharrun/Projects/Local_Projects/pBMSLSuite-O/diagram.json`
- **Docs:** 
  - `VOLTAGE_DIVIDER_IMPLEMENTATION.md`
  - `INA219_INTEGRATION_GUIDE.md`
  - `WOKWI_ENHANCEMENT.md` (this file)

