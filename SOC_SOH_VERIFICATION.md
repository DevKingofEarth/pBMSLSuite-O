# 📊 SoC/SoH Estimation Algorithm Verification

**Test Date:** 2026-01-23  
**Firmware Version:** v1.0+FaultInjection  
**Platform:** Wokwi Simulation

---

## 🔋 State of Charge (SoC) Overview

### What is SoC?

State of Charge represents the **available energy** in the battery pack as a percentage:
- **100% SoC:** Battery fully charged (nominal 4.2V per cell)
- **50% SoC:** Battery half discharged (nominal 3.6V per cell)
- **0% SoC:** Battery empty (nominal 3.0V per cell)

### Firmware Implementation

**Source Code:** Lines 157-165

```cpp
void updateSoC() {
    float avgVoltage = (cellVoltages[0] + cellVoltages[1] + cellVoltages[2] + cellVoltages[3]) / 4.0f;
    
    // Simple voltage-based SoC estimation
    if (avgVoltage >= 4.20f) estimatedSoC = 100.0f;
    else if (avgVoltage <= 3.00f) estimatedSoC = 0.0f;
    else {
        estimatedSoC = ((avgVoltage - 3.00f) / 1.20f) * 100.0f;
    }
}
```

### Formula Breakdown

**Linear Voltage-to-SoC Mapping:**
```
SoC(%) = ((V_avg - V_min) / (V_max - V_min)) * 100
       = ((V_avg - 3.0V) / (4.2V - 3.0V)) * 100
       = ((V_avg - 3.0V) / 1.2V) * 100
```

**Range Limits:**
- V_max = 4.20V (100% SoC)
- V_min = 3.00V (0% SoC)
- Range = 1.20V total usable

---

## 🧪 SoC Estimation Tests

### Test 4.1: Empty Battery (0% SoC)

**Setup:**
```
Cell 1: 3.0V
Cell 2: 3.0V
Cell 3: 3.0V
Cell 4: 3.0V
Average: 3.0V
```

**Expected:**
- SoC = 0%
- Serial: "SoC: 0.0% | SoH: 100.0%"
- State: IDLE (no fault at 3.0V boundary)

**How to Test:**
1. Set all 4 potentiometers to ~20% (≈3.0V)
2. Wait 1 second for settling
3. Check serial output
4. Verify SoC = 0.0%

**Success Criteria:**
- ✅ SoC displays as 0.0% or close
- ✅ No UV fault triggered (threshold is 2.8V, not 3.0V)
- ✅ Calculation: `(3.0-3.0)/1.2 * 100 = 0%` ✓

**Edge Case Note:**
- At exactly 3.0V, SoC = 0% is enforced by `if (avgVoltage <= 3.00f)`
- This is safely above UV_THRESHOLD (2.8V)

---

### Test 4.2: Half Charged (50% SoC)

**Setup:**
```
Cell 1: 3.6V
Cell 2: 3.6V
Cell 3: 3.6V
Cell 4: 3.6V
Average: 3.6V
```

**Expected:**
- SoC = 50%
- Serial: "SoC: 50.0% | SoH: 100.0%"
- Calculation: `(3.6-3.0)/1.2 * 100 = 50%`

**How to Test:**
1. Set all 4 potentiometers to ~50% (≈3.6V)
2. Wait for settling
3. Check SoC in serial output
4. Verify percentage ≈ 50%

**Success Criteria:**
- ✅ SoC displays as 50.0% (±1%)
- ✅ Calculation verified: `0.6V / 1.2V * 100 = 50%` ✓
- ✅ Linear scaling working correctly

**Verification Math:**
- Voltage span: 3.6V - 3.0V = 0.6V
- Total range: 4.2V - 3.0V = 1.2V
- Percentage: 0.6 / 1.2 = 0.5 = 50% ✅

---

### Test 4.3: Fully Charged (100% SoC)

**Setup:**
```
Cell 1: 4.2V
Cell 2: 4.2V
Cell 3: 4.2V
Cell 4: 4.2V
Average: 4.2V
```

**Expected:**
- SoC = 100%
- Serial: "SoC: 100.0% | SoH: 100.0%"
- Note: Cells will trigger BALANCING (> 4.1V)

**How to Test:**
1. Set all 4 potentiometers to ~88% (≈4.2V)
2. Wait for settling
3. Note: State will be BALANCING
4. Check SoC = 100.0%

**Success Criteria:**
- ✅ SoC displays as 100.0%
- ✅ Calculation enforced: `if (avgVoltage >= 4.20f) estimatedSoC = 100.0f` ✓
- ✅ Clamping works correctly (no >100%)

**Important Note:**
- This voltage will trigger balancing (threshold 4.1V)
- This is expected behavior for a fully charged battery
- SoC calculation happens independently of state machine

---

### Test 4.4: Quarter Charged (25% SoC)

**Setup:**
```
Cell 1: 3.3V
Cell 2: 3.3V
Cell 3: 3.3V
Cell 4: 3.3V
Average: 3.3V
```

**Expected:**
- SoC = 25%
- Calculation: `(3.3-3.0)/1.2 * 100 = 25%`
- Serial: "SoC: 25.0% | SoH: 100.0%"

**How to Test:**
1. Set all 4 potentiometers to ~50% (≈3.3V)
2. Observe SoC ≈ 25%

**Success Criteria:**
- ✅ SoC displays as 25.0% (±1%)
- ✅ Linear scaling: `0.3V / 1.2V * 100 = 25%` ✓

---

### Test 4.5: Three-Quarter Charged (75% SoC)

**Setup:**
```
Cell 1: 3.9V
Cell 2: 3.9V
Cell 3: 3.9V
Cell 4: 3.9V
Average: 3.9V
```

**Expected:**
- SoC = 75%
- Calculation: `(3.9-3.0)/1.2 * 100 = 75%`
- Serial: "SoC: 75.0% | SoH: 100.0%"

**How to Test:**
1. Set all 4 potentiometers to ~75% (≈3.9V)
2. Observe SoC ≈ 75%

**Success Criteria:**
- ✅ SoC displays as 75.0% (±1%)
- ✅ Linear scaling: `0.9V / 1.2V * 100 = 75%` ✓

---

## 💚 State of Health (SoH) Overview

### What is SoH?

State of Health represents the **battery capacity degradation** over time:
- **100% SoH:** Brand new, full capacity
- **80% SoH:** 20% capacity loss (typical replacement threshold)
- **50% SoH:** Half original capacity (end of life)
- **0% SoH:** Dead battery (cannot hold charge)

### Firmware Implementation

**Source Code:** Lines 167-171

```cpp
// Simple SoH estimation (degrades with extreme temps)
if (temperature > 45.0f || temperature < 10.0f) {
    estimatedSoH -= 0.01f;  // Degrade slowly
}
estimatedSoH = constrain(estimatedSoH, 0.0f, 100.0f);
```

### Degradation Model

**Temperature-Based Degradation:**
- **Normal Temp (10-45°C):** No degradation
- **Extreme Temp (>45°C or <10°C):** 0.01% loss per 100ms cycle (10Hz)

**Degradation Rate:**
- Per loop: -0.01%
- Per second: -0.1% (10 loops/sec)
- Per minute: -6% (600 loops/min)
- Per hour: -360% (theoretical max)

**Practical Degradation Timeline:**

| Time | Temperature | SoH Change | Total SoH |
|------|-------------|------------|----------|
| Start | 25°C | 0% | 100.0% |
| 10 min @ 50°C | 50°C | -0.6% | 99.4% |
| 1 hour @ 50°C | 50°C | -6.0% | 94.0% |
| 24 hours @ 50°C | 50°C | -144% | Limited to 0% |

---

## 🧪 SoH Estimation Tests

### Test 4.6: Normal Temperature (No Degradation)

**Setup:**
```
Temperature: 25°C (normal)
Duration: 2 minutes
Expected Degradation: 0%
```

**Expected:**
- SoH remains at 100.0%
- No degradation occurs
- Serial: "SoC: XX.X% | SoH: 100.0%" (constant)

**How to Test:**
1. Set NTC thermistor to room temperature (~25°C)
2. Let firmware run for 2 minutes
3. Monitor SoH value in serial output
4. Verify it stays at 100.0%

**Success Criteria:**
- ✅ SoH constant at 100.0% throughout test
- ✅ No degradation during normal operation
- ✅ Confirms temperature gate is working

---

### Test 4.7: Warm Temperature (Degradation)

**Setup:**
```
Temperature: 50°C (warm - excessive)
Duration: 1 minute
Expected Degradation: ~6%
```

**Expected:**
- SoH decreases at 0.1% per second
- After 60 seconds: 100% - 6% = 94%
- Serial shows: "SoH: 94.0%" (approximately)

**How to Test:**
1. Use fault injection: `INJECT OT` (sets temp to 75°C)
2. Run for 1 minute
3. Monitor SoH degradation
4. Verify ~6% loss

**Calculation:**
- Cycles per minute: 600 (10Hz × 60s)
- Loss per cycle: 0.01%
- Total loss: 600 × 0.01% = 6% ✓

**Success Criteria:**
- ✅ SoH degrades at expected rate
- ✅ After 60s: SoH ≈ 94% (100% - 6%)
- ✅ Degradation matches formula: cycles × 0.01%

---

### Test 4.8: Cold Temperature (Degradation)

**Setup:**
```
Temperature: 5°C (cold - excessive)
Duration: 2 minutes
Expected Degradation: ~12%
```

**Expected:**
- SoH decreases at 0.1% per second
- After 120 seconds: 100% - 12% = 88%
- Serial: "SoH: 88.0%" (approximately)

**How to Test:**
1. Use fault injection: `INJECT UT` (sets temp to -10°C)
2. Run for 2 minutes
3. Monitor SoH degradation
4. Verify ~12% loss

**Calculation:**
- Cycles in 2 minutes: 1200 (10Hz × 120s)
- Loss per cycle: 0.01%
- Total loss: 1200 × 0.01% = 12% ✓

**Success Criteria:**
- ✅ SoH degrades during cold operation
- ✅ After 120s: SoH ≈ 88% (100% - 12%)
- ✅ Cold and hot both degrade equally

---

### Test 4.9: SoH Lower Bound (0% Clamping)

**Setup:**
```
Run at 50°C for extended time
Let SoH decrease until it reaches 0%
```

**Expected:**
- SoH decreases continuously until 0%
- Clamping enforces: `constrain(estimatedSoH, 0.0f, 100.0f)`
- SoH stays at 0% after reaching bottom

**How to Test:**
1. Inject OT fault for extended period (multiple injections)
2. Let SoH degrade for 10+ minutes
3. Verify it reaches 0%
4. Verify it doesn't go negative

**Success Criteria:**
- ✅ SoH eventually reaches 0%
- ✅ Never goes below 0% (clamping works)
- ✅ Represents completely dead battery

---

### Test 4.10: SoH Upper Bound (100% Clamping)

**Setup:**
```
Start at 100% SoH
Operate at normal temp
Simulate "charge cycles" without degradation
```

**Expected:**
- SoH stays at 100.0% (no degradation at normal temp)
- Never exceeds 100%
- Clamping enforces: `constrain(estimatedSoH, 0.0f, 100.0f)`

**How to Test:**
1. Run firmware at normal temperature for 5+ minutes
2. Verify SoH remains at 100.0%
3. Check upper bound enforcement

**Success Criteria:**
- ✅ SoH stays at 100.0%
- ✅ Never exceeds 100%
- ✅ Clamping prevents unrealistic values

---

## 📈 Combined SoC + SoH Tests

### Test 4.11: Realistic Charge Cycle

**Scenario:** Battery charges from 0% to 100%, monitoring SoC and SoH

| Step | Cell Voltage | Expected SoC | Expected SoH | Notes |
|------|--------------|--------------|--------------|-------|
| 1    | 3.0V         | 0%           | 100%         | Empty |
| 2    | 3.3V         | 25%          | 100%         | Quarter |
| 3    | 3.6V         | 50%          | 100%         | Half |
| 4    | 3.9V         | 75%          | 100%         | Three-quarter |
| 5    | 4.2V         | 100%         | 100%         | Full (triggers balancing) |

**How to Test:**
1. Start with pots at 20% (≈3.0V)
2. Gradually increase all pots together
3. At each step, record SoC and SoH
4. Verify linear SoC scaling
5. Verify SoH stays at 100% (normal temps)

**Success Criteria:**
- ✅ SoC scales linearly: 0%, 25%, 50%, 75%, 100%
- ✅ SoH constant at 100% (normal operation)
- ✅ Both algorithms working together correctly

---

### Test 4.12: Degradation During Charge

**Scenario:** Battery charges while at elevated temperature

| Step | Temp | SoC | Expected SoH |
|------|------|-----|--------------|
| Start | 25°C | 0% | 100% |
| After 30s @ 50°C | 50°C | 25% | ~97% |
| After 1min @ 50°C | 50°C | 50% | ~94% |
| After 2min @ 50°C | 50°C | 100% | ~88% |

**How to Test:**
1. Inject OT fault (temp = 75°C)
2. Simultaneously increase cell voltages (simulate charging)
3. Monitor both SoC and SoH
4. Verify SoC increases while SoH decreases

**Success Criteria:**
- ✅ SoC increases as expected
- ✅ SoH decreases due to temperature
- ✅ Independent algorithms operating correctly

---

## 🔍 Accuracy Verification

### Voltage-to-SoC Linearity

**Test:** Check that SoC scaling is perfectly linear

| V_avg | Formula | Expected SoC | Tolerance |
|-------|---------|--------------|-----------|
| 3.0V  | (3.0-3.0)/1.2*100 | 0% | ±0.1% |
| 3.3V  | (3.3-3.0)/1.2*100 | 25.0% | ±0.1% |
| 3.6V  | (3.6-3.0)/1.2*100 | 50.0% | ±0.1% |
| 3.9V  | (3.9-3.0)/1.2*100 | 75.0% | ±0.1% |
| 4.2V  | (4.2-3.0)/1.2*100 | 100% | ±0.1% |

**Success Criteria:**
- ✅ All calculations match expected values
- ✅ Linear fit is perfect (R² = 1.0)
- ✅ No non-linearity observed

---

### Temperature Degradation Rate

**Test:** Verify degradation at exactly -0.01% per 100ms

**Measurement:**
- Start: SoH = 100%
- After 10 seconds (100 cycles): SoH should = 99%
- After 100 seconds: SoH should = 99%

**Calculation Check:**
- 100 cycles × 0.01% = 1.0% ✓

---

## ✅ Test Completion Summary

| Test | Purpose | Status |
|------|---------|--------|
| 4.1 | Empty (0% SoC) | ⏳ |
| 4.2 | Half (50% SoC) | ⏳ |
| 4.3 | Full (100% SoC) | ⏳ |
| 4.4 | Quarter (25% SoC) | ⏳ |
| 4.5 | Three-quarter (75% SoC) | ⏳ |
| 4.6 | Normal temp (no degrad) | ⏳ |
| 4.7 | Warm temp (degrad) | ⏳ |
| 4.8 | Cold temp (degrad) | ⏳ |
| 4.9 | SoH lower bound | ⏳ |
| 4.10 | SoH upper bound | ⏳ |
| 4.11 | Realistic charge cycle | ⏳ |
| 4.12 | Degradation during charge | ⏳ |

**Total Tests:** 12  
**Passed:** 0/12 ⏳  
**Pass Rate:** 0% ⏳

---

## 📝 Execution Log

### Session 1: Initial Setup (2026-01-23)

```
Time: 12:42 PM IST
Status: Test suite created
Tests: Ready for Wokwi execution
```

---

## 🔗 References

- **SoC Source:** Lines 157-165 (updateSoC)
- **SoH Source:** Lines 167-171 (updateSoH)
- **Thresholds:** Lines 25-30 (Constants)
- **Wokwi:** https://wokwi.com/projects/453769002432989185

