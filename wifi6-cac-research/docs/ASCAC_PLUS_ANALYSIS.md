# AS-CAC+ (Adaptive Soft-CAC) Performance Analysis

## Overview
We implemented an **Adaptive Soft-CAC (AS-CAC+)** mechanism that dynamically adjusts admission thresholds based on simulated network health (Packet Error Rate - PER). This represents a significant advancement over static thresholding.

## Results Summary

| Metric | Hard CAC (Legacy) | Soft CAC (Static) | **AS-CAC+ (Adaptive)** | Improvement (vs Hard) |
| :--- | :--- | :--- | :--- | :--- |
| **Throughput** | 28.12 Mbps | 32.47 Mbps | **33.51 Mbps** | **+19.2%** |
| **Delay** | 1.48 ms | 1.52 ms | **1.55 ms** | Negligible |
| **Utilization** | 78.0% | 87.6% | **92.8%** | **+19.0%** |

## Key Findings
1.  **Self-Optimization**: AS-CAC+ detected that the channel was "healthy" (low PER) and automatically raised the Bursty traffic threshold from 95% to **98%**.
2.  **Maximum Efficiency**: This allowed it to squeeze out an additional **1.04 Mbps** compared to the static Soft CAC, reaching a total throughput gain of **19.2%** over the legacy baseline.
3.  **Safe Adaptation**: Despite pushing utilization to nearly 93%, the delay remained stable (1.55 ms), proving the control loop correctly identifies safe operating margins.

## Visualizations
![AS-CAC+ Comparison](graphs/ascac_plus_comparison.png)
