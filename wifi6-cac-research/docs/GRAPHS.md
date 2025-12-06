# WiFi 6 CAC Simulation - Visualization Graphs

## Generated Graphs

All graphs created using **gnuplot** from simulation results.

### 1. Flow Admission by Traffic Type
**File**: `admission-results.png`

![Admission Results](file:///home/rohan/Public/CAC-ct/ns-3/admission-results.png)

**Shows**:
- VoIP: 12 admitted, 0 blocked (100% success)
- Video: 3 admitted, 6 blocked (33% success)
- Bursty: 0 admitted, 9 blocked (0% success)

**Key Insight**: CAC prioritizes VoIP traffic - all VoIP flows admitted while lower-priority traffic is selectively blocked.

---

### 2. Airtime Utilization
**File**: `airtime-utilization.png`

![Airtime Utilization](file:///home/rohan/Public/CAC-ct/ns-3/airtime-utilization.png)

**Shows**:
- VoIP: 0.1404 (18.7% of total)
- Video: 0.6094 (81.3% of total)
- Total: 0.7498 (74.98%)
- CAC Threshold: 0.80 (80%) - shown as red dashed line

**Key Insight**: Airtime utilization stopped at 75%, safely below the 80% threshold, preventing network overload.

---

### 3. Blocking Probability by Traffic Type
**File**: `blocking-probability.png`

![Blocking Probability](file:///home/rohan/Public/CAC-ct/ns-3/blocking-probability.png)

**Shows**:
- VoIP: 0% blocking
- Video: 66.7% blocking
- Bursty: 100% blocking
- Overall: 50% blocking

**Key Insight**: Clear QoS differentiation - critical VoIP traffic never blocked, while best-effort traffic blocked to maintain quality.

---

### 4. Performance Metrics
**File**: `performance-metrics.png`

![Performance Metrics](file:///home/rohan/Public/CAC-ct/ns-3/performance-metrics.png)

**Shows**:
- **Left**: Aggregate Throughput = 8.32 Mbps
- **Right**: Average Delay = 1.47 ms (well below 150ms VoIP threshold)

**Key Insight**: Excellent performance maintained for admitted flows - very low delay demonstrates effective CAC.

---

## Graph Interpretation

### QoS Prioritization Success
The graphs clearly show the CAC mechanism working as designed:
1. **Priority Enforcement**: VoIP (highest priority) → 100% admission
2. **Selective Blocking**: Video (medium priority) → 33% admission
3. **Best-Effort Rejection**: Bursty (lowest priority) → 0% admission

### Network Protection
- Airtime utilization capped at 75% (below 80% threshold)
- Low average delay (1.47ms) proves network not congested
- Aggregate throughput maintained at healthy level

### Research Validation
These graphs demonstrate:
✅ Airtime-based CAC works effectively  
✅ QoS differentiation successful  
✅ Network protection achieved  
✅ Low delay maintained for admitted flows  

---

## Files Location

All graphs are in: `/home/rohan/Public/CAC-ct/ns-3/`

- `admission-results.png` (24 KB)
- `airtime-utilization.png` (22 KB)
- `blocking-probability.png` (28 KB)
- `performance-metrics.png` (37 KB)

## Gnuplot Scripts

The gnuplot scripts used to generate these graphs are also available:
- `plot-admission.gnu`
- `plot-airtime.gnu`
- `plot-blocking-prob.gnu`
- `plot-performance.gnu`

You can regenerate or modify graphs by editing these scripts and running:
```bash
gnuplot plot-<name>.gnu
```

---

**Status**: ✅ All graphs generated successfully!
