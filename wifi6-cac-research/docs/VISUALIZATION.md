# WiFi 6 CAC Research - Visualization Guide

## 📊 Line Plots & Graphs

We have generated 7 publication-quality visualizations:

### 1. Admission Timeline (New!)
**File**: `graphs/admission-timeline.png`
- **Type**: Line plot
- **Shows**: Cumulative airtime utilization as flows arrive
- **Insight**: Visualizes exactly when the 80% threshold is reached and subsequent flows are blocked.

### 2. Traffic Comparison (New!)
**File**: `graphs/traffic-type-comparison.png`
- **Type**: Bar chart
- **Shows**: Requested vs Admitted vs Blocked flows per type
- **Insight**: Clear comparison showing VoIP priority (100% admitted) vs Bursty (100% blocked).

### 3. QoS Effectiveness (New!)
**File**: `graphs/qos-effectiveness.png`
- **Type**: Bar chart with color gradient
- **Shows**: Admission rate by priority level
- **Insight**: Demonstrates perfect QoS enforcement (Green/100% → Orange/33% → Red/0%).

### 4. Standard Metrics
- `admission-results.png`: Flow counts
- `airtime-utilization.png`: Airtime breakdown
- `blocking-probability.png`: Blocking %
- `performance-metrics.png`: Throughput & Delay

## 🎬 NetAnim Network Visualization

A NetAnim trace file has been generated to visualize the network topology and packet flows.

### How to View
1. **Locate the file**: `results/wifi6-cac-animation.xml`
2. **Open NetAnim**: Run the NetAnim executable (usually in `ns-3/netanim/NetAnim`)
3. **Load Trace**: Click file folder icon → Select `wifi6-cac-animation.xml`
4. **Play**: Click the Play button

### Visualization Features
- **AP Node**: Blue (Center)
- **VoIP Clients**: Green nodes
- **Video Clients**: Orange nodes
- **Bursty Clients**: Grey nodes
- **Packet Flows**: Visible packet transmissions between STA and AP

## 📁 File Locations

All visualization files are organized in:
```
wifi6-cac-research/
├── graphs/                  # PNG Images
│   ├── admission-timeline.png
│   ├── traffic-type-comparison.png
│   ├── qos-effectiveness.png
│   └── ... (4 others)
└── results/
    └── wifi6-cac-animation.xml  # NetAnim Trace
```
