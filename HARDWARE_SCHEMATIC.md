# 🔧 BMS Hardware Schematic & Reference

**Last Updated:** 2026-01-23  
**Firmware Version:** v1.0+VoltageDiv+INA219  
**Status:** Complete documentation (PNG generation recommended)

---

## 📐 Block Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                    pBMSLSuite-O BMS System                      │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐  │
│  │   Battery    │      │   Cell       │      │   Load       │  │
│  │   Pack       │      │ Balancing    │      │   Resistor   │  │
│  │ (2.8-5.0V)   │      │   Circuit    │      │  (Simulated) │  │
│  └──────┬───────┘      └──────┬───────┘      └──────┬───────┘  │
│         │                     │                     │           │
│         │                     │                     │           │
│  ┌──────▼──────────────────────▼─────────────────────▼──┐      │
│  │                                                      │      │
│  │            SIGNAL CONDITIONING STAGE               │      │
│  │                                                      │      │
│  │  ┌────────────────┐      ┌────────────────┐       │      │
│  │  │  Voltage       │      │   Temperature  │       │      │
│  │  │  Dividers      │      │    Sensor      │       │      │
│  │  │  (R dividers)  │      │ (NTC Therm)    │       │      │
│  │  │                │      │                │       │      │
│  │  │ Cell1→GPIO34   │      │ GPIO33         │       │      │
│  │  │ Cell2→GPIO35   │      │                │       │      │
│  │  │ Cell3→GPIO25   │      │                │       │      │
│  │  │ Cell4→GPIO26   │      │                │       │      │
│  │  └────────┬───────┘      └────────┬───────┘       │      │
│  │           │                      │               │      │
│  └───────────▼──────────────────────▼───────────────┘      │
│              │                      │                       │
│  ┌───────────▼──────────────────────▼──────────────────┐  │
│  │                                                      │  │
│  │         ESP32 DevKit-C MICROCONTROLLER               │  │
│  │                                                      │  │
│  │  ┌────────────────────────────────────────────────┐ │  │
│  │  │ • 12-bit ADC (4 cells, temp, load)            │ │  │
│  │  │ • I2C Bus (GPIO21=SDA, GPIO22=SCL)           │ │  │
│  │  │ • Digital I/O (GPIO16-19 balancing)          │ │  │
│  │  │ • UART (GPIO1=TX, GPIO3=RX for monitoring)   │ │  │
│  │  │ • 240MHz Dual Core                           │ │  │
│  │  │ • 320KB RAM                                  │ │  │
│  │  └────────────────────────────────────────────────┘ │  │
│  │                                                      │  │
│  └──────────────┬─────────┬─────────┬──────────┬────────┘  │
│                 │         │         │          │            │
│                 │         │         │          │            │
│  ┌──────────────▼──┐  ┌───▼────┐ ┌──▼──┐   ┌──▼────┐      │
│  │  INA219 Current │  │ Shift  │ │ LED │   │Output │      │
│  │  Sensor (I2C)   │  │Registers│ │Bar  │   │Drivers│      │
│  │                │  │        │ │Graph│   │       │      │
│  │ • Bus Voltage  │  │ SR1,SR2│ │     │   │ Pin16 │      │
│  │ • Shunt Current│  │        │ │ 10- │   │ Pin17 │      │
│  │ • Power        │  │GPIO13  │ │ Seg │   │ Pin18 │      │
│  │   (0x40)       │  │GPIO14  │ │     │   │ Pin19 │      │
│  │                │  │GPIO2   │ │     │   │       │      │
│  └────────────────┘  └────────┘ └─────┘   └───────┘      │
│                                                            │
│  ┌────────────────┐     ┌────────────────────────────┐   │
│  │ Status LEDs    │     │ Serial Monitor (115200)    │   │
│  │                │     │                            │   │
│  │ • Red LED      │     │ → USB Connection           │   │
│  │   (GPIO2)      │     │ → Real-time Data Output    │   │
│  │ • Orange LED   │     │ → Fault Injection Commands │   │
│  │   (GPIO22)     │     │                            │   │
│  └────────────────┘     └────────────────────────────┘   │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

## 🔌 Pin Configuration

### ESP32 ADC Pins (Input)

| GPIO | Function | Range | Purpose |
|------|----------|-------|---------|
| 34   | CELL1_PIN | 0-3.3V | Cell 1 voltage |
| 35   | CELL2_PIN | 0-3.3V | Cell 2 voltage |
| 25   | CELL3_PIN | 0-3.3V | Cell 3 voltage |
| 26   | CELL4_PIN | 0-3.3V | Cell 4 voltage |
| 33   | TEMP_PIN  | 0-3.3V | Temperature sensor |
| 36   | VP (GPIO36) | 0-3.3V | Load slider (analog) |

### ESP32 Digital Output Pins

| GPIO | Function | Purpose | Signal |
|------|----------|---------|--------|
| 16   | BALANCE1_PIN | Cell 1 balance enable | HIGH = active |
| 17   | BALANCE2_PIN | Cell 2 balance enable | HIGH = active |
| 18   | BALANCE3_PIN | Cell 3 balance enable | HIGH = active |
| 19   | BALANCE4_PIN | Cell 4 balance enable | HIGH = active |
| 2    | STATUS_LED_PIN | Red LED (fault indicator) | HIGH = on |
| 22   | Orange LED | Status indication | Via shift register |

### ESP32 I2C Pins (Communication)

| GPIO | Function | Purpose |
|------|----------|---------|
| 21   | SDA | I2C Data (to INA219) |
| 22   | SCL | I2C Clock (to INA219) |

### ESP32 Serial Pins

| GPIO | Function | Purpose | Baud |
|------|----------|---------|------|
| 1    | TX       | Serial output | 115200 |
| 3    | RX       | Serial input | 115200 |

---

## 🔋 Cell Voltage Measurement Circuit

### Direct Connection (Wokwi Simulation)

```
┌─────────────────────────────────────────────┐
│                                             │
│  Potentiometer (Simulated Cell Voltage)    │
│  │                                         │
│  ├─→ SIG → ESP32 GPIO34 (ADC)              │
│  ├─→ VCC → +3.3V                           │
│  └─→ GND → GND                             │
│                                             │
│  Output Range: 0-3.3V                      │
│  Resolution: 12-bit (0-4095 levels)        │
│  Measurement Formula:                      │
│    V = (ADC_reading / 4095) × 3.3V        │
│                                             │
└─────────────────────────────────────────────┘
```

### With Voltage Divider (Real Hardware)

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  Cell Voltage Input (2.5-4.2V)                 │
│  │                                             │
│  ├─→ R1 (10kΩ) ──┐                            │
│                   ├─→ ESP32 GPIO34 (ADC)       │
│  ├─→ R2 (15kΩ) ──┤                            │
│  │               │                             │
│  └─→ GND ────────┘                            │
│                                                  │
│  Voltage Divider Formula:                      │
│    V_out = V_in × (R2 / (R1+R2))              │
│           = V_in × (15k / 25k)                │
│           = V_in × 0.6                        │
│                                                  │
│  Firmware Compensation:                        │
│    V_measured = V_adc × 3.3 / 4095            │
│    V_actual = V_measured × (1 / 0.6)          │
│               = V_measured × 1.667            │
│                                                  │
│  Example: 4.2V Cell                           │
│    V_out = 4.2 × 0.6 = 2.52V → ADC           │
│    V_adc = 2.52 × 4095 / 3.3 = 3125          │
│    V_measured = 3125 × 3.3 / 4095 = 2.52V   │
│    V_actual = 2.52 × 1.667 = 4.20V ✓        │
│                                                  │
└──────────────────────────────────────────────────┘
```

---

## 🌡️ Temperature Sensor Circuit

### NTC Thermistor Configuration

```
┌──────────────────────────────────────┐
│                                      │
│  +3.3V                              │
│  │                                  │
│  ├─→ R_NTC (10kΩ @ 25°C)           │
│  │                                  │
│  ├─→ (Divider Point) → GPIO33       │
│  │                                  │
│  ├─→ R_fixed (10kΩ)                │
│  │                                  │
│  GND                                │
│                                      │
│  Measurement:                        │
│  • Read V_NTC @ GPIO33              │
│  • Calculate R_NTC                  │
│  • Use Steinhart-Hart equation      │
│                                      │
│  Steinhart-Hart:                    │
│    1/T = A + B×ln(R) + C×ln(R)³   │
│                                      │
│    Simplified (Beta Model):         │
│    1/T = 1/T0 + (1/β)×ln(R/R0)    │
│                                      │
│  Parameters:                        │
│  • T0 = 25°C (298.15K)             │
│  • R0 = 10kΩ                       │
│  • β = 3950K                       │
│                                      │
│  Output Range: -40°C to +85°C       │
│  Accuracy: ±1°C                     │
│                                      │
└──────────────────────────────────────┘
```

---

## ⚡ Cell Balancing Circuit

### Balancing Resistor Configuration

```
┌────────────────────────────────────────────┐
│                                            │
│  Cell Positive Terminal                   │
│  │                                        │
│  ├─→ Balance Resistor (100Ω)              │
│  │   └─→ GPIO16/17/18/19                  │
│  │   └─→ When HIGH: Current flows         │
│  │   └─→ Power dissipated: P = V²/R       │
│  │                                        │
│  │   At 4.2V: P = (4.2)² / 100 = 176mW  │
│  │                                        │
│  Cell Negative Terminal (GND)              │
│  │                                        │
│  ├─→ GPIO16 (Cell 1 balance)              │
│  ├─→ GPIO17 (Cell 2 balance)              │
│  ├─→ GPIO18 (Cell 3 balance)              │
│  └─→ GPIO19 (Cell 4 balance)              │
│                                            │
│  Control Logic:                           │
│  if (V_cell > 4.1V && state==BALANCING)  │
│  {                                        │
│      digitalWrite(BALANCE_PIN, HIGH);    │
│  }                                        │
│  else                                     │
│  {                                        │
│      digitalWrite(BALANCE_PIN, LOW);     │
│  }                                        │
│                                            │
└────────────────────────────────────────────┘
```

---

## 💨 I2C Bus Configuration (INA219 Current Sensor)

### I2C Connection Schematic

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  ESP32                         INA219 Module    │
│  ────────                     ──────────────    │
│                                                  │
│  GPIO21 (SDA) ┬──→ 4.7kΩ →┬──→ SDA Pin         │
│               │   Pull-up │                    │
│               └───────────┘                    │
│                                                  │
│  GPIO22 (SCL) ┬──→ 4.7kΩ →┬──→ SCL Pin        │
│               │   Pull-up │                    │
│               └───────────┘                    │
│                                                  │
│  GND ─────────────────────────→ GND            │
│  +3.3V ────────────────────────→ VDD           │
│                                                  │
│  Address Configuration (INA219):              │
│  A0 Pin → GND (or +3.3V)                      │
│  A1 Pin → GND (or +3.3V)                      │
│                                                  │
│  Default Address: 0x40 (A0=GND, A1=GND)      │
│                                                  │
│  I2C Protocol:                                │
│  • 400kHz Fast Mode                           │
│  • 7-bit addressing                           │
│  • Data bytes read sequentially               │
│                                                  │
│  Registers:                                   │
│  • Bus Voltage (V)                           │
│  • Shunt Voltage (mV across 0.1Ω)           │
│  • Power (W)                                 │
│  • Current (mA)                              │
│                                                  │
└──────────────────────────────────────────────────┘
```

---

## 🔴 Status LED Configuration

### LED Control Schematic

```
┌─────────────────────────────────────────┐
│                                         │
│  Red LED (GPIO2) - Fault Indicator     │
│  ├─→ 220Ω Current Limiting Resistor    │
│  ├─→ LED Anode (positive)              │
│  └─→ LED Cathode → GND                 │
│                                         │
│  Brightness Control:                   │
│  • HIGH (3.3V): LED ON (~15mA)        │
│  • LOW (0V): LED OFF                  │
│                                         │
│  Fault Indication Pattern:             │
│  • IDLE/BALANCING: LED solid ON       │
│  • FAULT: LED flashing (500ms on/off) │
│  • OV/UV: Fast flash (250ms)          │
│  • OT/UT: Slow flash (1s)             │
│                                         │
│  Orange LED (GPIO22) - Status LED      │
│  └─→ Via Shift Register (SR1/SR2)     │
│  └─→ 220Ω limiting resistor           │
│  └─→ Can show load level               │
│                                         │
└─────────────────────────────────────────┘
```

---

## 📊 LED Bar Graph (10-segment)

### Bar Graph Connection

```
┌────────────────────────────────────────┐
│                                        │
│  10-Segment LED Bar Graph             │
│                                        │
│  Anode Pins (A1-A10):                │
│  Each segment: ─┬──→ (To shift register) │
│                │                     │
│  Cathode Pins (C1-C10):              │
│  All connected to GND                │
│                                        │
│  Shift Register (SR1, SR2):          │
│  • SR1 outputs: Q1-Q7                │
│  • SR2 outputs: Q1-Q3                │
│  • Total: 10 outputs for bar         │
│                                        │
│  Display Pattern:                    │
│  • 0% SoC: No bars lit               │
│  • 50% SoC: 5 bars lit               │
│  • 100% SoC: 10 bars lit             │
│                                        │
│  Control via GPIO:                   │
│  • GPIO13: STCP (Strobe)            │
│  • GPIO14: DS (Data)                │
│  • GPIO2: SHCP (Clock)              │
│                                        │
└────────────────────────────────────────┘
```

---

## 🎛️ Load Simulator (Slide Potentiometer)

### Load Control Circuit

```
┌──────────────────────────────────────────┐
│                                          │
│  Slide Potentiometer (Pot5)             │
│  ├─→ +3.3V                             │
│  ├─→ GND                                │
│  └─→ SIG → ESP32 GPIO36 (ADC)          │
│                                          │
│  Simulates Variable Load:               │
│  • 0% position: 0A (no load)           │
│  • 50% position: ~1.5A (simulated)     │
│  • 100% position: ~3A (max)            │
│                                          │
│  In Firmware:                           │
│  loadCurrent = (ADC_reading / 4095)    │
│               × MAX_CURRENT            │
│                                          │
│  Effect on SoC:                        │
│  • Higher load → faster discharge      │
│  • Can be used to test SoC estimation  │
│                                          │
│  Note: Actual current not measured    │
│  This is for simulation/testing only   │
│                                          │
└──────────────────────────────────────────┘
```

---

## 📋 Component List (BOM)

### Microcontroller & Main Board

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| Microcontroller | ESP32-DEVKIT-C-V4 | 1 | $15 | 240MHz, 320KB RAM |
| USB Cable | Micro-USB | 1 | $2 | For programming |

### Voltage Measurement

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| Potentiometer | 10kΩ | 4 | $2 | Cell voltage sim |
| NTC Thermistor | 10kΩ @ 25°C | 1 | $1 | Temperature sensor |
| Resistor | 10kΩ | 5 | $0.50 | For dividers/NTC |
| Resistor | 15kΩ | 4 | $0.50 | For voltage dividers |

### Current Measurement

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| INA219 Module | Adafruit INA219 | 1 | $15 | Current/voltage/power |
| Resistor | 4.7kΩ | 2 | $0.20 | I2C pull-ups |

### Cell Balancing

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| Resistor | 100Ω 1W | 4 | $2 | Balance resistors |

### LED & Display

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| LED Red | 3mm | 1 | $0.20 | Status indicator |
| LED Orange | 3mm | 1 | $0.20 | Status indicator |
| LED Bar Graph | 10-segment | 1 | $2 | SoC/SoH display |
| Resistor | 220Ω | 12+ | $1 | Current limiting |

### Shift Registers & Control

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| Shift Register | 74HC595 | 2 | $2 | LED drivers |
| Breadboard | Half-size | 1 | $5 | Prototyping |
| Jumper Wires | Dupont | - | $3 | Connections |

### Load Simulation

| Item | Part | Qty | Cost | Notes |
|------|------|-----|------|-------|
| Slide Potentiometer | 10kΩ | 1 | $1 | Load control |

**Total BOM Cost:** ~$52 (simulation)  
**Total with INA219:** ~$67 (real hardware)

---

## ✅ Assembly Checklist

- [ ] ESP32 mounted on breadboard
- [ ] All potentiometers connected to ADC pins
- [ ] NTC thermistor connected to GPIO33
- [ ] Balance resistors connected to GPIO16-19
- [ ] LED connections verified with 220Ω resistors
- [ ] Shift registers wired for LED bar
- [ ] I2C pull-up resistors installed
- [ ] INA219 module connected (if using)
- [ ] All GND connections verified
- [ ] All +3.3V connections verified
- [ ] USB cable connected for programming
- [ ] Serial monitor working at 115200 baud

---

## 🔗 Related Documentation

- **Firmware:** pBMSLSuite-O.ino (361 lines)
- **Wokwi Circuit:** diagram.json
- **Voltage Dividers:** VOLTAGE_DIVIDER_IMPLEMENTATION.md
- **Current Sensor:** INA219_INTEGRATION_GUIDE.md
- **Test Guide:** TEST_RESULTS.md
- **Balancing:** BALANCING_TEST.md
- **SoC/SoH:** SOC_SOH_VERIFICATION.md

---

## 📞 Quick Reference

### ADC Measurement Formula
```
V_measured = (ADC_value × V_ref) / ADC_max
           = (ADC_value × 3.3) / 4095
           = ADC_value × 0.000805664
```

### Temperature Formula (Steinhart-Hart)
```
1/T = A + B×ln(R) + C×(ln(R))³
Simplified: 1/T = 1/T0 + (1/β)×ln(R/R0)
```

### Power Dissipation (Balancing)
```
P = V² / R = (4.2)² / 100 = 176mW per cell
Total (4 cells): 704mW
```

### I2C Address Calculation
```
Address = 0x40 + (A1 << 1) + A0
        = 0x40 (when A0=GND, A1=GND)
```

