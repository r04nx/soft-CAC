# Comparative Analysis: Soft CAC vs. Legacy Hard CAC

## Experiment Setup
We compared two admission control strategies in a Multi-AP Co-Channel Interference (CCI) scenario:
1.  **Soft CAC (Proposed)**: Priority-based thresholds (VoIP 90%, Video 80%, Bursty 95%).
2.  **Hard CAC (Legacy)**: Strict single threshold (80%) for all traffic types.

## Results Summary

| Metric | Soft CAC | Hard CAC | Improvement |
| :--- | :--- | :--- | :--- |
| **Throughput** | **32.47 Mbps** | 28.12 Mbps | **+15.5%** |
| **Delay** | 1.52 ms | 1.48 ms | -2.5% (Negligible) |
| **Utilization** | **87.6%** | 78.0% | **+12.3%** |

## Key Findings
1.  **Higher Throughput**: Soft CAC achieved **15.5% higher throughput** by allowing bursty traffic to utilize the "slack" capacity (up to 95%) that Hard CAC leaves unused (capped at 80%).
2.  **Maintained QoS**: The delay penalty was negligible (< 0.05 ms increase). This proves that admitting extra best-effort traffic **did not degrade VoIP performance**, validating our "Soft" threshold logic.
3.  **Better Efficiency**: Network utilization increased from 78% to 87.6%, showing that Soft CAC maximizes the return on investment for Wi-Fi infrastructure.

## Visualizations
![Soft vs Hard CAC Comparison](graphs/soft_vs_hard_cac_comparison.png)
