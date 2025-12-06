# WiFi 6 CAC Simulation - Quick Start Guide

## What Was Created

A complete research-grade simulation for WiFi 6 networks with airtime-based Call Admission Control (CAC) for your research work.

## Location

All files are in: `/home/rohan/Public/CAC-ct/wifi6-cac-research/`

## Files Created

### Core Simulation Files
1. **wifi6-cac-airtime.h** - CAC module header
2. **wifi6-cac-airtime.cc** - CAC implementation with WiFi 6 PHY calculations
3. **wifi6-cac-simulation.cc** - Main simulation with 4 traffic types

### Analysis & Automation
4. **analyze-results.py** - Generates 5 publication-quality graphs
5. **run-simulation.sh** - Automated execution for all scenarios
6. **test-compile.sh** - Quick compilation test

### Documentation
7. **README.md** - Complete usage guide
8. **simulation-config.txt** - All parameters documented

## How to Run

### Option 1: Automated (Recommended)
```bash
cd /home/rohan/Public/CAC-ct/wifi6-cac-research
./run-simulation.sh
```

This will:
- Build the simulation
- Run 6 scenarios (25, 30, 35, 40, 45, 50 clients)
- Generate all graphs and statistics

### Option 2: Manual Single Run
```bash
# Copy files to NS-3
cd /home/rohan/Public/CAC-ct/wifi6-cac-research
cp wifi6-cac-simulation.cc ../ns-3/scratch/

# Build
cd ../ns-3
./ns3 build

# Run with 30 clients
./ns3 run "wifi6-cac-simulation --nStations=30 --enableCac=1"

# Analyze results
cd ../wifi6-cac-research
python3 analyze-results.py wifi6-cac-30
```

## What You'll Get

### Graphs (PNG files)
1. **throughput_vs_flows.png** - Aggregate throughput vs offered flows
2. **delay_vs_flows.png** - Average delay by traffic type
3. **blocking_probability.png** - CAC blocking probability
4. **voip_delay_cdf.png** - VoIP delay distribution
5. **airtime_utilization.png** - Airtime breakdown

### Data Files (CSV)
- Delay measurements per packet
- Admission decisions per flow
- FlowMonitor statistics
- Throughput data

### Statistics
- **summary_statistics.txt** - Detailed numerical results

## Key Features

### Traffic Types
- **VoIP**: G.711 codec (64 kbps, strict delay <150ms)
- **Video**: Streaming (3 Mbps, adaptive rate)
- **Bursty**: File transfers (5 Mbps peak)
- **Web**: HTTP-like (1 Mbps average)

### CAC Mechanism
- Airtime-based admission control
- 80% threshold for dense environments
- QoS-aware (prioritizes VoIP)
- Prevents network collapse

### WiFi 6 Configuration
- 802.11ax standard
- 5 GHz, 80 MHz channel
- MCS 5 (64-QAM)
- 2 spatial streams

## Research Metrics

All requested metrics are implemented:
✅ Aggregate throughput vs number of offered flows  
✅ Offered flows vs average delay  
✅ Blocking probability graphs  
✅ End-to-end VoIP delay  
✅ Airtime utilization breakdown  

## Troubleshooting

### If compilation fails:
```bash
cd /home/rohan/Public/CAC-ct/ns-3
./ns3 clean
./ns3 configure --enable-examples
./ns3 build
```

### If Python analysis fails:
```bash
pip3 install pandas numpy matplotlib seaborn scipy
```

## Next Steps

1. **Run the simulation**: `./run-simulation.sh`
2. **Review results**: Check `results/` directory for graphs
3. **Analyze data**: Read `summary_statistics.txt`
4. **Use in paper**: Graphs are publication-ready (300 DPI)

## Support

For detailed information, see:
- **README.md** - Complete documentation
- **walkthrough.md** - Implementation details
- **simulation-config.txt** - All parameters

---

**Status**: ✅ Ready for research use!
