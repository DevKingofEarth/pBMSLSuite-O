# pBMSLSuite-O

## Portable BMS Lab Suite (Open)

An educational Battery Management System (BMS) simulator demonstrating scalability from ESP32 to MW-scale systems.

## 🌟 Features

### Core Functionality
- **4-Cell Monitoring**: Real-time voltage monitoring with ADC readings
- **Temperature Sensing**: NTC thermistor with Steinhart-Hart equation for accuracy
- **Passive Balancing**: GPIO-controlled balancing resistors (4×100Ω)
- **4 Power States**: DEEP_SLEEP, LIGHT_SLEEP, ACTIVE, CRITICAL

### Advanced Features (Phase 2)
- **State of Charge (SoC)**: Hybrid voltage-based + coulomb counting estimation
- **State of Health (SoH)**: Capacity tracking, cycle counting, temperature exposure
- **Fault Injection Framework**: Software-based testing for education

### Safety Systems (Phase 1)
- **Unified Fault Framework**: Detect → Diagnose → Mitigate → Recover
- **Threshold Hysteresis**: Prevents oscillation around threshold values
- **Noise Filtering**: Moving average filter + spike detection + debouncing
- **Pattern Detection**: Voltage slope analysis, temperature rate monitoring
- **Early Warning System**: Runtime prediction and depletion alerts

## 📁 Project Structure

```
pBMSLSuite-O/
├── pBMSLSuite-O.ino       # Main firmware (1200+ lines)
├── diagram.json           # Wokwi circuit design
├── wokwi.toml            # Wokwi configuration
├── compile.sh            # ESP32 compilation script
├── README.md             # This file
├── FAULT_INJECTION_GUIDE.md
├── SCALABILITY.md
└── build/                # Compiled binaries
    ├── pBMSLSuite-O.ino.bin
    ├── pBMSLSuite-O.ino.elf
    └── ...
```

## 🔧 Hardware Requirements

### Minimum (Wokwi Simulation)
- ESP32 Dev Module
- 4× Potentiometers (cell voltage simulation)
- 1× NTC Thermistor (10kΩ, B=3950)
- 4× 100Ω resistors (balancing)
- 1× LED (status indication)

### Recommended (Real Hardware)
- ESP32 Dev Kit V1
- Battery cells (Li-ion 18650 or similar)
- Current sensor (ACS712 or INA219)
- Temperature sensors (multiple points)
- MOSFET drivers (balancing control)
- CAN transceiver (communication)

## 📊 Technical Specifications

### Thresholds
| Parameter | Trigger | Clear | Hysteresis |
|-----------|---------|-------|------------|
| Over-Voltage | 4.25V | 4.15V | 0.10V |
| Under-Voltage | 2.80V | 2.90V | 0.10V |
| Over-Temperature | 60°C | 55°C | 5°C |
| Under-Temperature | 0°C | 5°C | 5°C |
| Balancing | 4.10V | 4.05V | 0.05V |

### Power States
| State | Current | Sampling Rate | Use Case |
|-------|---------|---------------|----------|
| DEEP_SLEEP | ~10μA | 10s | Storage |
| LIGHT_SLEEP | ~1mA | 1s | Standby |
| ACTIVE | ~50mA | 100ms | Normal Operation |
| CRITICAL | ~100mA | 10ms | Fault Response |

### Filtering
- **Moving Average**: 8-sample circular buffer
- **Spike Detection**: ±0.5V (voltage), ±10°C (temperature)
- **Debouncing**: 3 consecutive samples required

## 🚀 Getting Started

### Wokwi Simulation
1. Open [Wokwi.com](https://wokwi.com)
2. Create new project with ESP32
3. Import `diagram.json`
4. Upload `pBMSLSuite-O.ino`
5. Run simulation

### Local Compilation
```bash
chmod +x compile.sh
./compile.sh
```

### Serial Commands (Fault Injection)
```
ENABLE INJECTION    - Enable fault injection
INJECT OV           - Inject over-voltage fault
INJECT UV           - Inject under-voltage fault  
INJECT OT           - Inject over-temperature fault
INJECT UT           - Inject under-temperature fault
LIST FAULTS         - Show available faults
STATUS              - Show injection status
DISABLE INJECTION   - Disable fault injection
```

## 📈 Scalability

### From 4 Cells to 1000+ Cells

**Current Design**: 4 cells, single ESP32
**Scalable Design**: Modular architecture for large packs

#### Scaling Strategy
1. **Cell Level**: Each module monitors 4-16 cells
2. **Module Level**: Daisy-chain modules via CAN/I2C
3. **Pack Level**: Master controller aggregates modules
4. **System Level**: Multiple packs with cloud monitoring

#### Algorithm Complexity
| Algorithm | O(n) | O(1) | Notes |
|-----------|------|------|-------|
| Voltage monitoring | ✅ | - | Parallel ADC reads |
| Temperature monitoring | ✅ | - | Multiple sensors |
| SoC estimation | ✅ | - | Per-cell tracking |
| SoH monitoring | ✅ | - | Historical data |
| Fault detection | - | ✅ | Pattern matching |
| Balancing | ✅ | - | Cell selection |

#### Communication Scaling
- **I2C**: Up to 128 devices (short range)
- **CAN**: Up to 1000+ nodes (robust, long range)
- **RS-485**: Industrial scale (very long range)

## 🎓 Educational Value

### Learning Objectives
1. **BMS Fundamentals**: Voltage, current, temperature monitoring
2. **Safety Systems**: Fault detection, hysteresis, filtering
3. **State Estimation**: SoC, SoC algorithms, accuracy tradeoffs
4. **Scalability**: Modular design for large systems
5. **Testing**: Fault injection for validation

### Experiment Ideas
1. Observe hysteresis preventing threshold oscillation
2. Inject faults and watch detection/recovery
3. Monitor SoC convergence during discharge
4. Test temperature compensation accuracy
5. Scale from 4 to 16 cells (code changes)

## 🔬 Algorithm Details

### Steinhart-Hart Temperature Calculation
```cpp
// NTC Thermistor: R(T) = R₀ × exp(B × (1/T - 1/T₀))
R = R_divider × (V_measured / (V_supply - V_measured))
1/T = 1/T₀ + (1/B) × ln(R/R₀)
```

### SoC Estimation (Hybrid)
- **70% Coulomb Counting**: Simulated charge tracking
- **30% Voltage-Based**: Piecewise linear curve
- **Temperature Compensation**: Reduces estimate at low temps

### Fault Detection Pipeline
```
Raw ADC → Spike Filter → Moving Average → Debounce → Fault Check → Hysteresis → Action
```

## 📝 License

MIT License - Educational and commercial use welcome.

## 🤝 Contributing

1. Fork the repository
2. Create feature branch
3. Add tests and documentation
4. Submit pull request

## 📧 Contact

Project maintainer: [Your Name]
Issues: GitHub Issues tab

---

**Built for Education** 🎓
*Learn BMS fundamentals with hands-on simulation*
