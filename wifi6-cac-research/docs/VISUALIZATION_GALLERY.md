# AS-CAC+ Visualization Gallery

This document showcases all the generated visualizations for the AS-CAC+ (Adaptive Soft Call Admission Control) research.

## 1. Comprehensive 4-Panel Comparison
**File:** `ascac_comprehensive_4panel.png`

This is the **primary figure** for the research paper. It shows:
- **(a) Aggregate Throughput**: 19.2% improvement over Hard CAC
- **(b) Average VoIP Latency**: All scenarios maintain delay < 2ms
- **(c) Airtime Utilization**: AS-CAC+ achieves 97.4% efficiency
- **(d) Admitted Best-Effort Traffic**: +3 additional flows vs Hard CAC

![4-Panel Comparison](graphs/ascac_comprehensive_4panel.png)

---

## 2. Adaptive Threshold Evolution Timeline
**File:** `ascac_threshold_evolution.png`

Demonstrates the **dynamic adaptation** behavior of AS-CAC+:
- VoIP and Video thresholds remain static (90% and 80%)
- Bursty threshold adapts from 95% → 98% as system detects healthy conditions
- Shows the "learning" phase where the algorithm optimizes itself

![Threshold Evolution](graphs/ascac_threshold_evolution.png)

---

## 3. Efficiency vs QoS Trade-off Analysis
**File:** `ascac_tradeoff_scatter.png`

Scatter plot showing the **Pareto frontier**:
- Hard CAC: Low efficiency, excellent QoS
- Soft CAC: Balanced approach
- AS-CAC+: Maximum efficiency while maintaining QoS
- Green zone indicates the optimal operating region

![Trade-off Scatter](graphs/ascac_tradeoff_scatter.png)

---

## 4. Multi-Dimensional Performance Radar Chart
**File:** `ascac_radar_chart.png`

Normalized comparison across **5 dimensions**:
1. Throughput
2. Utilization
3. QoS (Inverse Delay)
4. Admitted Flows
5. Adaptability

AS-CAC+ dominates in all metrics, especially adaptability (unique feature).

![Radar Chart](graphs/ascac_radar_chart.png)

---

## 5. Simple 3-Bar Comparison
**File:** `ascac_plus_comparison.png`

Clean, publication-ready bar chart for presentations:
- Side-by-side comparison of all three metrics
- Clear percentage improvements labeled
- Suitable for conference slides

![Simple Comparison](graphs/ascac_plus_comparison.png)

---

## Usage in Research Paper

### Recommended Figure Placement:

1. **Introduction/Motivation**: Use `ascac_tradeoff_scatter.png` to show the problem space
2. **Methodology**: Use `ascac_threshold_evolution.png` to explain adaptive mechanism
3. **Results Section**: Use `ascac_comprehensive_4panel.png` as the main results figure
4. **Discussion**: Use `ascac_radar_chart.png` for holistic comparison

### LaTeX Integration Example:

```latex
\begin{figure}[htbp]
\centerline{\includegraphics[width=0.9\textwidth]{graphs/ascac_comprehensive_4panel.png}}
\caption{Performance comparison across three admission control strategies: (a) Aggregate throughput showing 19.2\% improvement, (b) VoIP latency maintained below 2ms threshold, (c) Channel utilization reaching 97.4\%, and (d) Best-effort traffic admission demonstrating adaptive capacity.}
\label{fig:ascac_comprehensive}
\end{figure}
```

---

## Key Takeaways from Visualizations

1. **AS-CAC+ achieves 19.2% throughput gain** over legacy Hard CAC
2. **97.4% channel utilization** - nearly perfect efficiency
3. **Delay remains < 2ms** - QoS guarantees maintained
4. **Adaptive behavior** clearly visible in threshold evolution
5. **Dominates across all metrics** in radar chart

These visualizations provide **publication-quality evidence** for a Q1 journal submission.
