# 📊 ESP32 ADC Behavior Analysis

## Critical Clarification

### Q: Does ESP32 have different ADC ranges for 3.3V vs 5V pins?
**A: NO** - ESP32 ADC **always** measures 0-3.3V, regardless of pin type!

### ESP32 ADC Specifications
- **Resolution**: 12-bit (0-4095)
- **Input Range**: 0-3.3V **MAXIMUM**
- **VREF**: Internal 1.1V or external 3.3V (configurable)
- **Accuracy**: ±10% typical (requires calibration)

## 🔍 Current Implementation Analysis

### Line 23: V_REF Definition
```cpp
#define V_REF 3.3f
```

This assumes the ADC is using **external 3.3V reference** (default on most ESP32 boards).

### Line 22: ADC_MAX_VALUE
```cpp
#define ADC_MAX_VALUE 4095.0f
```

Correct for 12-bit ADC (2^12 = 4096, but 0-4095).

### Voltage Formula (Line 129)
```cpp
float rawV1 = analogRead(CELL1_PIN) * (V_REF / ADC_MAX_VALUE);
```

**This formula is CORRECT for measuring 0-3.3V signals!**

## 📊 What Happens with Different Inputs?

### Case 1: Potentiometer (0-3.3V) ✅
- Input: 0-3.3V
- ADC Reading: 0-4095
- Calculated: 0-3.3V **CORRECT!**

### Case 2: 5V Input ⚠️
- Input: 5V
- ESP32 ADC **clamps** to 3.3V (hardware limit)
- ADC Reading: 4095
- Calculated: 3.3V **WRONG!** (should be 5V)

### Case 3: Battery Voltage (3.7V Li-ion) ⚠️
- Input: 3.7V
- ESP32 ADC **clamps** to 3.3V (hardware limit)
- ADC Reading: ~3850
- Calculated: 3.3V **WRONG!** (should be 3.7V)

## ✅ Current Status: Working Correctly for Test Setup

### Your Wokwi Setup (Pots on 3.3V)
```
3.3V ---------[Potentiometer]-------- GND
                     |
                     → ESP32 ADC Pin
```

**This is CORRECT!** ✅
- Pot outputs 0-3.3V
- ESP32 ADC reads 0-3.3V
- Formula calculates correctly

## ❌ Real-World Problem: Battery Voltage

### Real Battery (3.7V-4.2V)
```
Battery (3.7V-4.2V) --------- ESP32 ADC Pin
```

**This is DANGEROUS!** ❌
1. ESP32 will clamp voltage to 3.3V
2. ADC will always read ~3.3V
3. **Cannot measure actual battery voltage**
4. **Risk of damaging ESP32** (if voltage spikes)

## ✅ Solution: Voltage Divider

### Hardware Required
```
Battery (0-5V)
        |
        R1 = 10kΩ
        |
        +
        |
        R2 = 15kΩ
        |
        GND
        |
  → ESP32 ADC Pin
```

### Voltage Divider Formula
```
V_adc = V_in * (R2 / (R1 + R2))
      = V_in * (15k / 25k)
      = V_in * 0.6
```

### Updated Firmware Formula
```cpp
float rawADC = analogRead(pin);
float voltage = rawADC * (V_REF / ADC_MAX_VALUE) / 0.6;
```

## 📊 Example Calculations

### With Voltage Divider
| Battery Voltage | V_adc | ADC Reading | Calculated |
|----------------|-------|-------------|------------|
| 0V             | 0V    | 0           | 0V         |
| 1.65V          | 0.99V | 1296        | 1.65V      |
| 3.3V           | 1.98V | 2592        | 3.3V       |
| 3.7V           | 2.22V | 2880        | 3.7V       |
| 4.2V           | 2.52V | 3294        | 4.2V       |
| **5V**         | **3V** | **3960**    | **5V**     |

## 🔋 Current Sensing (Separate Issue)

### Current Problem
- Potentiometers control **voltage**, not **current**
- We need to measure **actual battery current** during charge/discharge
- Requires **shunt resistor + amplifier**

### Hardware Required
```
Battery (+) ---------[Shunt Resistor 0.1Ω]-------- Load
                     |
                     → Amplifier (INA219)
                           |
                           → ESP32 ADC
```

### Current Formula
```cpp
current = (voltage_across_shunt / shunt_resistance)
         = (0.001V / 0.1Ω)
         = 10A
```

## 🎯 Recommendations

### Phase 1: Wokwi Testing (Current - ✅ Working)
- Keep potentiometers on 3.3V
- Test voltage thresholds (2.8V-4.2V)
- Verify fault detection
- Test balancing logic

### Phase 2: Hardware Preparation (Next Steps)
1. Add voltage dividers (R1=10k, R2=15k) for real battery testing
2. Update firmware with scaling formula
3. Add current sensor (INA219 module)
4. Add load resistors for discharge testing

### Phase 3: Real-World Testing
1. Test with actual Li-ion batteries (3.7V-4.2V)
2. Test with resistive loads (simulate device usage)
3. Test charging scenarios
4. Verify current measurement accuracy

## ✅ Summary

- **Current implementation is CORRECT** for Wokwi testing with pots
- **Voltage dividers are NEEDED** for real battery testing
- **Current sensing is SEPARATE** and requires different hardware
- **No need to change V_REF or ADC_MAX_VALUE** - they're correct

**Next Step**: Let's update the Wokwi schematic to add voltage dividers and current sensing!
