# Research Paper Assets: WiFi 6 CAC Simulation

Here are the visual assets generated for your research paper, including the network topology visualization and key performance line plots.

## 1. Network Topology & Architecture (NS-3 GUI View)

This visualization represents the network architecture simulated in NS-3, similar to the NetAnim GUI view.

![Network Topology](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/network_topology_viz.png)

**Description:**
- **Central Node (Blue Triangle)**: WiFi 6 Access Point (AP)
- **Green Circles**: VoIP Clients (High Priority, AC_VO)
- **Orange Squares**: Video Clients (Medium Priority, AC_VI)
- **Grey Diamonds**: Bursty Clients (Low Priority, AC_BE)
- **Layout**: Random disc distribution within 15m radius

---

## 2. Performance Line Plots

### A. Admission Timeline
Shows the cumulative airtime utilization as new flow requests arrive.

![Admission Timeline](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/admission-timeline.png)

**Analysis:**
- The system accepts flows until the **80% threshold** is reached.
- **VoIP flows** (first 12) are all admitted, consuming ~18% airtime.
- **Video flows** begin admission but are cut off once the threshold is hit.
- Subsequent **Video** and all **Bursty** flows are blocked to protect the network.

### B. Traffic Type Comparison
Comparison of requested vs. admitted flows for each traffic category.

![Traffic Comparison](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/traffic-type-comparison.png)

**Analysis:**
- **VoIP**: 100% Admission Rate (12/12). Critical for QoS.
- **Video**: ~33% Admission Rate (3/9).
- **Bursty**: 0% Admission Rate (0/9). Lowest priority traffic is sacrificed.

### C. QoS Effectiveness
Visualizes the admission rate across different priority levels.

![QoS Effectiveness](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/qos-effectiveness.png)

**Analysis:**
- Demonstrates the strict enforcement of QoS policies.
- High-priority traffic is fully protected.
- Lower-priority traffic faces higher blocking probabilities to maintain aggregate performance.

---

## 3. Summary of Results

| Metric | Value | Note |
|--------|-------|------|
| **Aggregate Throughput** | 8.32 Mbps | Maintained stable |
| **Average Delay** | 1.47 ms | Excellent (<150ms target) |
| **Blocking Probability** | 50% | Necessary for QoS |
| **Airtime Utilization** | 74.98% | Safely below 80% limit |

These assets confirm that the Airtime-Based CAC mechanism effectively protects critical traffic in a dense WiFi 6 environment.
