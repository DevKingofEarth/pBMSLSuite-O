# Fault Injection Guide

## 🎯 Purpose

The fault injection framework allows testing and validation of BMS safety systems without risking real hardware. Perfect for education and testing.

## ⚡ Quick Start

### Enable Fault Injection
```
ENABLE INJECTION
```

### Inject a Fault
```
INJECT OV    # Over-voltage (4.5V)
INJECT UV    # Under-voltage (2.5V)
INJECT OT    # Over-temperature (75°C)
INJECT UT    # Under-temperature (-10°C)
```

### Disable When Done
```
DISABLE INJECTION
```

## 📋 Available Commands

| Command | Description | Duration |
|---------|-------------|----------|
| `ENABLE INJECTION` | Activate fault injection | Until disabled |
| `DISABLE INJECTION` | Deactivate fault injection | - |
| `INJECT OV` | Over-voltage fault (4.5V) | 5 seconds |
| `INJECT UV` | Under-voltage fault (2.5V) | 5 seconds |
| `INJECT OT` | Over-temperature fault (75°C) | 5 seconds |
| `INJECT UT` | Under-temperature fault (-10°C) | 5 seconds |
| `CLEAR FAULT` | Immediately clear injected fault | - |
| `LIST FAULTS` | Show all available fault types | - |
| `STATUS` | Show current injection status | - |

## 🔬 What Happens During Fault Injection

### 1. Fault Detection
```
FAULT DETECTED: Over-Voltage [FILTERED+DEBOUNCED] 4.50V (3 samples)
```

### 2. Diagnosis
```
DIAGNOSIS: Over-voltage - checking charger connection
```

### 3. Mitigation
```
MITIGATION: Charging disabled, balancing activated
```

### 4. Recovery (after fault clears)
```
OV RECOVERED: 4.15V < 4.15V [HYSTERESIS]
FAULT RECOVERY: Fault cleared - monitoring for 10s
SYSTEM NORMAL - All systems operational
```

## 🎓 Educational Experiments

### Experiment 1: Hysteresis Observation
1. Inject OV fault
2. Watch system trigger at 4.25V
3. Lower voltage slowly
4. Observe recovery at 4.15V (not 4.25V)
5. **Learn**: Hysteresis prevents oscillation

### Experiment 2: Filtering Demonstration
1. Enable fault injection
2. Inject fault
3. Observe "3 samples" message
4. **Learn**: Debouncing prevents false triggers

### Experiment 3: Recovery Timing
1. Inject UV fault
2. Clear fault with `CLEAR FAULT`
3. Watch 10-second monitoring period
4. **Learn**: Graceful recovery prevents rapid cycling

### Experiment 4: Fault Severity
1. Try different fault types
2. Observe LED flash rates (faster = more severe)
3. **Learn**: Severity-based response prioritization

## 🔧 Advanced Usage

### Programmatic Fault Injection
```cpp
// In your code
bmsController.injectFault(FAULT_OV, 4.5f, 5000);  // 5 seconds
bmsController.runFaultInjectionDemo();  // Automated demo
```

### Custom Fault Duration
```cpp
// Inject fault for 10 seconds
bmsController.injectFault(FAULT_OT, 80.0f, 10000);

// Infinite duration (until cleared)
bmsController.injectFault(FAULT_SENSOR, 0.0f, 0);
```

### Fault Statistics
```cpp
// Get fault statistics
bmsController.printFaultStats();
// Output:
// Total Faults: 5
// OV: 2, UV: 1, OT: 2, UT: 0
```

## ⚠️ Safety Notes

1. **Simulation Only**: Fault injection is for simulation/testing only
2. **Real Hardware**: Never inject faults on actual battery systems
3. **Recovery Time**: Always wait for full recovery cycle
4. **Monitor Serial**: Watch console for fault progression

## 🐛 Troubleshooting

### Fault Not Triggering
- ✅ Ensure fault injection is enabled
- ✅ Check fault value exceeds threshold + hysteresis
- ✅ Verify 3 consecutive samples (debouncing)

### System Stuck in Fault
- ✅ Check physical sensors (are they realistic?)
- ✅ Clear injected fault manually
- ✅ Reset ESP32 if needed

### No Serial Response
- ✅ Check baud rate (115200)
- ✅ Verify correct COM port
- ✅ Try terminal software (PuTTY, minicom)

## 📊 Expected Behavior Summary

| Fault Type | Trigger | Recovery | LED Behavior |
|------------|---------|----------|--------------|
| OV | >4.25V | <4.15V | Fast flash |
| UV | <2.80V | >2.90V | Fast flash |
| OT | >60°C | <55°C | Medium flash |
| UT | <0°C | >5°C | Slow flash |

## 🎯 Learning Outcomes

After completing fault injection experiments, you should understand:

1. ✅ How BMS detects faults in real-time
2. ✅ Why hysteresis prevents threshold oscillation
3. ✅ How debouncing filters sensor noise
4. ✅ The Detect → Diagnose → Mitigate → Recover workflow
5. ✅ How severity affects system response
6. ✅ Importance of recovery monitoring

---

**Happy Testing!** 🔬
*Remember: Fault injection is for learning, not for production BMS systems.*
