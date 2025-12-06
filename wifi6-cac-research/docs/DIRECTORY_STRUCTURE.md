# WiFi 6 Airtime-Based CAC Research - Directory Structure

## 📁 Organized Directory Layout

```
wifi6-cac-research/
│
├── 📊 graphs/                          # Generated visualization graphs
│   ├── admission-results.png           # Flow admission by traffic type
│   ├── airtime-utilization.png         # Airtime breakdown with threshold
│   ├── blocking-probability.png        # Blocking probability per type
│   └── performance-metrics.png         # Throughput and delay metrics
│
├── 📈 results/                         # Simulation output data
│   ├── wifi6-cac-demo-results.csv      # Detailed admission decisions
│   └── wifi6-cac-demo-summary.txt      # Quick results summary
│
├── 📜 gnuplot-scripts/                 # Graph generation scripts
│   ├── plot-admission.gnu              # Admission results plot
│   ├── plot-airtime.gnu                # Airtime utilization plot
│   ├── plot-blocking-prob.gnu          # Blocking probability plot
│   └── plot-performance.gnu            # Performance metrics plot
│
├── 💻 simulation-code/                 # NS-3 simulation source
│   └── wifi6-cac-demo.cc               # Compiled demo simulation
│
├── 🔬 Core Implementation Files
│   ├── wifi6-cac-airtime.h             # CAC module header
│   ├── wifi6-cac-airtime.cc            # CAC implementation
│   └── wifi6-cac-simulation.cc         # Full simulation (with dependencies)
│
├── 🚀 Execution Scripts
│   ├── run-simulation.sh               # Automated multi-scenario execution
│   ├── test-compile.sh                 # Quick compilation test
│   └── analyze-results.py              # Python analysis script
│
└── 📖 Documentation
    ├── README.md                       # Complete usage guide
    ├── QUICKSTART.md                   # Quick start guide
    ├── GRAPHS.md                       # Graph interpretation guide
    ├── SIMULATION_RESULTS.md           # Detailed results analysis
    ├── simulation-config.txt           # Configuration parameters
    └── DIRECTORY_STRUCTURE.md          # This file
```

## 📊 Graphs Directory

Contains all publication-quality PNG graphs generated from simulation results:

| File | Size | Description |
|------|------|-------------|
| `admission-results.png` | 23 KB | Shows admitted vs blocked flows per traffic type |
| `airtime-utilization.png` | 22 KB | Stacked bar with airtime breakdown and CAC threshold |
| `blocking-probability.png` | 27 KB | Blocking percentage for each traffic type |
| `performance-metrics.png` | 37 KB | Dual chart: throughput and delay metrics |

**Usage**: Ready for inclusion in research papers, presentations, or reports.

## 📈 Results Directory

Simulation output data files:

| File | Description |
|------|-------------|
| `wifi6-cac-demo-results.csv` | Per-flow admission decisions with airtime calculations |
| `wifi6-cac-demo-summary.txt` | Quick summary of key metrics |

**Format**: CSV for easy import into Excel, MATLAB, or Python for further analysis.

## 📜 Gnuplot Scripts Directory

Reusable gnuplot scripts for graph generation:

| Script | Generates |
|--------|-----------|
| `plot-admission.gnu` | Admission results graph |
| `plot-airtime.gnu` | Airtime utilization graph |
| `plot-blocking-prob.gnu` | Blocking probability graph |
| `plot-performance.gnu` | Performance metrics graph |

**Regenerate graphs**:
```bash
cd gnuplot-scripts
gnuplot plot-<name>.gnu
```

## 💻 Simulation Code Directory

Compiled and tested NS-3 simulation code:

| File | Description |
|------|-------------|
| `wifi6-cac-demo.cc` | Standalone demo simulation (9.3 KB) |

**To run**:
```bash
# Copy to NS-3 scratch directory
cp simulation-code/wifi6-cac-demo.cc ../ns-3/scratch/

# Build and run
cd ../ns-3
./ns3 build
./ns3 run "wifi6-cac-demo --nStations=30"
```

## 🔬 Core Implementation

Research-grade implementation files:

| File | Size | Description |
|------|------|-------------|
| `wifi6-cac-airtime.h` | 6.7 KB | CAC module header with API |
| `wifi6-cac-airtime.cc` | 11 KB | Full CAC implementation |
| `wifi6-cac-simulation.cc` | 23 KB | Complete simulation with all traffic types |

**Note**: These require proper NS-3 module structure for compilation.

## 🚀 Execution Scripts

Automation and helper scripts:

| Script | Purpose |
|--------|---------|
| `run-simulation.sh` | Run multiple scenarios (25-50 clients) automatically |
| `test-compile.sh` | Quick compilation test |
| `analyze-results.py` | Generate graphs from CSV data |

## 📖 Documentation Files

Comprehensive documentation:

| File | Content |
|------|---------|
| `README.md` | Complete usage guide with installation and examples |
| `QUICKSTART.md` | Quick start for immediate use |
| `GRAPHS.md` | Detailed graph interpretation |
| `SIMULATION_RESULTS.md` | Full results analysis with tables |
| `simulation-config.txt` | All configuration parameters |

## 🎯 Quick Access

### View Results
```bash
cd wifi6-cac-research
cat results/wifi6-cac-demo-summary.txt
```

### View Graphs
```bash
cd wifi6-cac-research/graphs
ls -lh *.png
```

### Regenerate Graphs
```bash
cd wifi6-cac-research/gnuplot-scripts
gnuplot plot-admission.gnu
mv *.png ../graphs/
```

### Run New Simulation
```bash
cd wifi6-cac-research
./run-simulation.sh
```

## 📊 File Statistics

| Category | Files | Total Size |
|----------|-------|------------|
| Graphs | 4 | ~109 KB |
| Results | 2 | ~1.2 KB |
| Scripts | 7 | ~20 KB |
| Code | 4 | ~50 KB |
| Documentation | 6 | ~25 KB |
| **Total** | **23** | **~205 KB** |

## 🔄 Workflow

1. **Modify simulation** → Edit `simulation-code/wifi6-cac-demo.cc`
2. **Run simulation** → Use `run-simulation.sh` or manual NS-3 commands
3. **Analyze results** → Check `results/` directory
4. **Generate graphs** → Use gnuplot scripts in `gnuplot-scripts/`
5. **View visualizations** → Open PNGs in `graphs/`
6. **Document findings** → Update documentation files

## ✅ Organization Complete

All research materials are now properly organized and ready for:
- Research paper preparation
- Presentation creation
- Further analysis
- Code sharing
- Publication submission

---

**Location**: `/home/rohan/Public/CAC-ct/wifi6-cac-research/`
**Last Updated**: 2024-11-20
