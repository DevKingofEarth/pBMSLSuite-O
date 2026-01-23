# 🔋 Voltage Calculation Analysis

## Current Implementation

### Voltage Reading Formula (Lines 129-132)
```cpp
float rawV1 = analogRead(CELL1_PIN) * (V_REF / ADC_MAX_VALUE);
```

### Constants (Lines 22-23)
```cpp
#define V_REF 3.3f              // Reference voltage
#define ADC_MAX_VALUE 4095.0f   // ESP32 ADC resolution
```

## Formula Explanation

### Step 1: Analog Read
- `analogRead()` returns a value between 0-4095 (12-bit ADC)
- This is a digital representation of the input voltage

### Step 2: Voltage Conversion
- Formula: `digital_value * (V_REF / ADC_MAX_VALUE)`
- Example: If analogRead returns 2048 (midpoint)
  - `2048 * (3.3 / 4095) = 1.65V`

### Step 3: Actual Input Voltage
- The ESP32 ADC measures 0-3.3V directly
- If you apply 5V to an analog pin, it will be **clamped** to 3.3V
- The ADC cannot measure voltages above V_REF (3.3V)

## ⚠️ Critical Issue: 5V Input Problem

### Current Behavior
- If you apply 5V to a cell pin:
  - ESP32 will clamp to 3.3V
  - Formula calculates: `4095 * (3.3 / 4095) = 3.3V`
  - **Result**: Shows 3.3V, NOT 5V ❌

### Why This is Wrong
- The firmware assumes the input voltage = ADC reading
- In reality, ESP32 ADC has a **hard limit of 3.3V**
- Any voltage above 3.3V gets clamped, causing **false readings**

## ✅ Solution: Voltage Divider

### Required Hardware
We need to add voltage dividers to scale 0-5V down to 0-3.3V:

```
Cell Voltage (0-5V)
        |
        R1 (10kΩ)
        |
        +
        |
        R2 (15kΩ)
        |
        GND
        |
  → ESP32 ADC Pin

Voltage at ADC = V_in * (R2 / (R1 + R2))
               = V_in * (15k / 25k)
               = V_in * 0.6
```

### Updated Formula
```cpp
// Current formula (WRONG for 5V input):
float voltage = analogRead(pin) * (3.3 / 4095);

// Corrected formula (with voltage divider):
float rawADC = analogRead(pin);
float voltage = rawADC * (3.3 / 4095) * (1 / 0.6);  // Scale back up
```

## 📊 Example Calculations

### With Current Implementation (3.3V Max)
| Input Voltage | ADC Reading | Calculated Voltage |
|---------------|-------------|-------------------|
| 0V            | 0           | 0V                |
| 1.65V         | 2048        | 1.65V             |
| 3.3V          | 4095        | 3.3V              |
| **5V**        | **4095**    | **3.3V (WRONG!)** |

### With Voltage Divider (0-5V Range)
| Input Voltage | ADC Reading | Calculated Voltage |
|---------------|-------------|-------------------|
| 0V            | 0           | 0V                |
| 1.65V         | 1365        | 1.65V             |
| 3.3V          | 2250        | 3.3V              |
| **5V**        | **3375**    | **5V (CORRECT!)** |

## 🔍 Current Implementation Issues

1. **No voltage divider scaling** - Cannot measure >3.3V
2. **No current sensing** - Only voltage and temperature
3. **No load testing** - Only works in no-load conditions
4. **No hardware protection** - Direct 5V input will damage ESP32

## ✅ Next Steps

1. **Add voltage divider** (R1=10kΩ, R2=15kΩ)
2. **Update firmware** with scaling formula
3. **Add current sensing** (shunt resistor + amplifier)
4. **Test with load** (resistive load to simulate discharge)
5. **Add hardware protection** (TVS diodes, current limiting)

