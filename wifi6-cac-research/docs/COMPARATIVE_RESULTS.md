# Research Paper Results: Multi-AP & Comparative Analysis

## 1. Analytical Model Validation
We developed a **Multi-rate Erlang Loss Model** to theoretically predict blocking probability.

![Analytical Model](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/analytical_model_blocking.png)

**Insight**: The analytical model (red line) closely matches the simulation trends, validating the theoretical basis of our Airtime CAC.

---

## 2. Impact of Admission Control (With vs Without)
Comparison of network performance under heavy load (30 stations).

![CAC vs No CAC](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/cac_vs_no_cac.png)

| Metric | With CAC | Without CAC | Improvement |
|--------|----------|-------------|-------------|
| **Throughput** | 8.32 Mbps | ~9.5 Mbps | Stable |
| **Delay** | **1.47 ms** | **>45 ms** | **30x Lower Delay** |
| **State** | Stable | Congested | **Congestion Prevented** |

**Key Finding**: While "No CAC" allows slightly more throughput (by saturating buffers), it causes **massive delay spikes**, making VoIP unusable. CAC maintains low delay (1.47ms) essential for real-time apps.

---

## 3. Multi-AP Interference Analysis (Soft CAC)
Performance comparison between Co-Channel Interference (CCI) and Adjacent Channel Interference (ACI) scenarios with **Soft CAC**.

![Multi-AP Interference](file:///home/rohan/Public/CAC-ct/wifi6-cac-research/graphs/multi_ap_interference.png)

| Scenario | Throughput | Delay | Note |
|----------|------------|-------|------|
| **CCI (Same Channel)** | Lower | Higher | High contention between APs |
| **ACI (Clean Channels)** | **Higher** | **Lower** | Ideal deployment |

**Soft CAC Feature**:
- **Bursty Traffic** was allowed up to **95%** airtime (vs 80% for Video).
- This ensured better utilization while still protecting VoIP (which has 90% priority threshold).

---

## 4. Conclusion for Paper
The simulation results confirm:
1.  **Airtime CAC** effectively protects VoIP latency (1.47ms vs >45ms).
2.  **Soft CAC** allows higher utilization for bursty traffic without breaking QoS.
3.  **Multi-AP** scenarios demonstrate the impact of interference, which CAC helps mitigate by regulating load.

**Status**: All plots generated and ready for the paper!
