# WiFi 6 Airtime-Based Call Admission Control Simulation

## Overview

This project implements a comprehensive research-grade simulation of **WiFi 6 (802.11ax)** networks with **airtime-based Call Admission Control (CAC)** for dense WLAN environments. The simulation evaluates network performance under heterogeneous traffic conditions with variable client densities (25-50 clients).

## Research Objectives

- Implement novel airtime-based CAC mechanism for WiFi 6
- Evaluate performance in dense deployment scenarios
- Support heterogeneous traffic: VoIP, video streaming, bursty, and web browsing
- Generate comprehensive performance metrics for research publication

## Key Features

### Airtime-Based CAC
- Calculates airtime requirements based on WiFi 6 PHY parameters
- Admission threshold: 80% for dense environments
- QoS-aware admission (prioritizes VoIP traffic)
- Dynamic flow management

### Traffic Models
1. **VoIP**: G.711 codec (64 kbps, 160-byte packets, 20ms interval)
2. **Video Streaming**: Adaptive rate (2-5 Mbps, 1200-byte packets)
3. **Bursty Traffic**: Exponential on-off model (5 Mbps peak)
4. **Web Browsing**: Pareto distribution (1 Mbps average)

### WiFi 6 Configuration
- Standard: 802.11ax
- Frequency: 5 GHz
- Channel Width: 80 MHz
- MCS: 5 (64-QAM, 3/4 coding rate)
- Spatial Streams: 2
- Guard Interval: 800 ns

## Project Structure

```
wifi6-cac-research/
├── wifi6-cac-airtime.h          # CAC header file
├── wifi6-cac-airtime.cc         # CAC implementation
├── wifi6-cac-simulation.cc      # Main simulation script
├── analyze-results.py           # Python analysis script
├── run-simulation.sh            # Automated execution script
├── simulation-config.txt        # Configuration parameters
├── README.md                    # This file
└── results/                     # Output directory (created at runtime)
    ├── *.csv                    # Raw data files
    ├── *.png                    # Generated graphs
    └── summary_statistics.txt   # Statistical summary
```

## Installation

### Prerequisites
- NS-3 (already installed in `../ns-3`)
- Python 3.x with packages:
  - pandas
  - numpy
  - matplotlib
  - seaborn
  - scipy

### Install Python Dependencies
```bash
pip3 install pandas numpy matplotlib seaborn scipy
```

## Usage

### Quick Start
Run all simulations and generate results:
```bash
./run-simulation.sh
```

This script will:
1. Copy source files to NS-3 scratch directory
2. Build the simulation
3. Run simulations for 25, 30, 35, 40, 45, and 50 clients
4. Generate all graphs and statistics

### Manual Execution

#### 1. Copy Files to NS-3
```bash
cp wifi6-cac-airtime.h ../ns-3/scratch/
cp wifi6-cac-airtime.cc ../ns-3/scratch/
cp wifi6-cac-simulation.cc ../ns-3/scratch/
```

#### 2. Build Simulation
```bash
cd ../ns-3
./ns3 build
```

#### 3. Run Simulation
```bash
cd ../ns-3
./ns3 run "wifi6-cac-simulation --nStations=30 --nVoipFlows=12 --nVideoFlows=9 \
          --nBurstyFlows=6 --nWebFlows=3 --simTime=60 --threshold=0.80 \
          --enableCac=1 --channelWidth=80 --outputPrefix=wifi6-cac-30"
```

#### 4. Analyze Results
```bash
cd ../wifi6-cac-research
python3 analyze-results.py results/wifi6-cac-30
```

### Command-Line Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| `--nStations` | Number of WiFi stations | 30 |
| `--nVoipFlows` | Number of VoIP flows | 10 |
| `--nVideoFlows` | Number of video flows | 8 |
| `--nBurstyFlows` | Number of bursty flows | 6 |
| `--nWebFlows` | Number of web flows | 6 |
| `--simTime` | Simulation duration (seconds) | 60 |
| `--threshold` | CAC airtime threshold (0.0-1.0) | 0.80 |
| `--enableCac` | Enable CAC (1=yes, 0=no) | 1 |
| `--channelWidth` | Channel width (20/40/80/160 MHz) | 80 |
| `--outputPrefix` | Output file prefix | wifi6-cac |

## Output Files

### CSV Data Files
- `<prefix>-delay.csv`: Per-packet delay measurements
- `<prefix>-admission.csv`: Flow admission decisions
- `<prefix>-flowmon.csv`: FlowMonitor statistics
- `<prefix>-throughput.csv`: Throughput measurements

### Generated Graphs
1. **throughput_vs_flows.png**: Aggregate throughput vs number of offered flows
2. **delay_vs_flows.png**: Average delay vs number of flows (by traffic type)
3. **blocking_probability.png**: Blocking probability vs offered load
4. **voip_delay_cdf.png**: CDF of VoIP packet delays
5. **airtime_utilization.png**: Airtime breakdown by traffic type

### Statistics
- `summary_statistics.txt`: Detailed numerical results including:
  - Admission control statistics
  - Per-traffic-type delay statistics
  - Throughput and packet loss metrics
  - VoIP QoS compliance

## Research Metrics

### Primary Metrics
- **Aggregate Throughput**: Total network throughput (Mbps)
- **End-to-End Delay**: Per-packet delay by traffic type (ms)
- **Blocking Probability**: Ratio of blocked flows to total requests
- **VoIP QoS Compliance**: Percentage of VoIP packets with delay <150ms
- **Airtime Utilization**: Distribution of airtime across traffic types

### Performance Indicators
- Mean, median, std dev, min, max delay per traffic type
- 95th percentile delay
- Packet loss rate
- Per-flow throughput

## Research Novelty

This simulation contributes to the networking research field by:

1. **Airtime-based CAC for WiFi 6**: Novel adaptation of airtime fairness principles to admission control specifically for 802.11ax
2. **Dense WLAN Optimization**: Designed for high-density scenarios (25-50 clients)
3. **Heterogeneous Traffic Support**: Realistic mix of traffic types with different QoS requirements
4. **Comprehensive Evaluation Framework**: Publication-ready performance metrics and visualization

## Expected Results

### With CAC Enabled
- Lower blocking probability at moderate loads
- Maintained QoS for admitted flows
- VoIP delay consistently <150ms
- Graceful degradation under high load

### Without CAC (Baseline)
- Higher initial throughput
- Degraded performance at high loads
- Increased delays and packet loss
- Network collapse under saturation

## Citation

If you use this simulation in your research, please cite:

```
[Your research paper citation will go here]
```

## License

This project is released under the GNU General Public License v2.0, consistent with NS-3 licensing.

## Authors

- [Your Name]
- [Institution]
- [Contact Information]

## Acknowledgments

- NS-3 Network Simulator Project
- WiFi 6 (802.11ax) standardization work
- [Any funding sources or collaborators]

## Support

For questions or issues:
- Email: [your-email@domain.com]
- GitHub Issues: [repository-url]

## Version History

- **v1.0** (2024): Initial release with airtime-based CAC for WiFi 6
