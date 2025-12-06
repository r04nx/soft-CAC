# Multi-AP Network Topology Documentation

## 2-AP Configuration Overview

### Network Architecture

**Deployment Type:** Dense Multi-AP Wi-Fi 6 (IEEE 802.11ax)  
**Number of APs:** 2  
**Stations per AP:** 30  
**Total Stations:** 60  
**Operating Band:** 5 GHz  
**Channel Bandwidth:** 80 MHz

---

## AP Placement

### Spatial Configuration

- **AP Separation:** 50 meters
- **Coverage Radius:** ~30 meters per AP
- **Interference Zone:** 10-meter overlap in the middle

### Channel Assignment

**Co-Channel Interference (CCI) Scenario:**
- **AP 1:** Channel 36 (5.180 GHz)
- **AP 2:** Channel 36 (5.180 GHz)
- **Result:** Full co-channel interference in overlapping area

**Adjacent Channel Interference (ACI) Scenario:**
- **AP 1:** Channel 36 (5.180 GHz)
- **AP 2:** Channel 40 (5.200 GHz)
- **Result:** Reduced interference, better isolation

---

## Station Distribution (Per AP)

### Traffic Type Breakdown

| Traffic Type | Count per AP | Access Category | Application | Bitrate | Delay Requirement |
|:-------------|-------------:|:----------------|:------------|:--------|:------------------|
| **VoIP** | 12-13 | AC_VO | G.711 codec | 64 kbps CBR | < 30 ms |
| **Video** | 9-11 | AC_VI | Streaming | 3 Mbps VBR | < 100 ms |
| **Bursty** | 9-12 | AC_BE | Best-effort | 5 Mbps peak | Delay-tolerant |
| **Total** | **30** | - | - | - | - |

### Spatial Distribution

Stations are distributed in concentric rings around each AP:

- **Inner Ring (5-10m):** ~10 stations (mixed types)
- **Middle Ring (10-20m):** ~12 stations (mixed types)
- **Outer Ring (20-30m):** ~8 stations (mixed types)

This distribution simulates realistic indoor deployment with varying signal strengths and path loss.

---

## Network Topology Diagrams

### Figure 1: Multi-AP Topology (CCI Scenario)

![Multi-AP Topology](graphs/multi_ap_topology_2ap.png)

**Key Features:**
- Two APs separated by 50m
- Both operating on Channel 36 (CCI)
- Coverage areas overlap in the middle
- 30 stations per AP color-coded by traffic type
- Red = VoIP, Orange = Video, Green = Bursty

### Figure 2: Single AP Station Distribution

![AP Station Distribution](graphs/ap_station_distribution.png)

**Key Features:**
- Detailed view of one AP's coverage
- Concentric ring distribution
- Traffic flow indicators
- Technical specifications annotated

---

## Simulation Parameters

### Physical Layer
- **Standard:** IEEE 802.11ax (Wi-Fi 6)
- **Frequency:** 5 GHz
- **Channel Width:** 80 MHz
- **MCS:** Adaptive (MCS 0-11)
- **Max PHY Rate:** ~866 Mbps per stream

### MAC Layer
- **EDCA:** Enabled (4 Access Categories)
- **OFDMA:** Supported
- **MU-MIMO:** Enabled
- **Frame Aggregation:** A-MPDU/A-MSDU

### Traffic Characteristics

**VoIP (G.711):**
- Packet Size: 160 bytes
- Inter-packet Interval: 20 ms
- Constant Bitrate: 64 kbps
- Priority: Highest (AC_VO)

**Video (Streaming):**
- Packet Size: 1200 bytes (average)
- Variable Bitrate: 3 Mbps average
- Frame Rate: ~30 fps
- Priority: High (AC_VI)

**Bursty (Best-Effort):**
- Packet Size: 1500 bytes
- On-Off Pattern: Exponential
- Peak Rate: 5 Mbps during ON
- Priority: Low (AC_BE)

---

## Interference Scenarios

### Co-Channel Interference (CCI)

**Configuration:**
- Both APs on Channel 36
- Full spectrum overlap
- Maximum interference in overlap zone

**Impact:**
- Stations in overlap zone experience high contention
- Collision probability increases
- Throughput degradation in interference zone
- AS-CAC must manage admission carefully

### Adjacent Channel Interference (ACI)

**Configuration:**
- AP1 on Channel 36, AP2 on Channel 40
- Partial spectrum overlap (guard bands)
- Reduced interference

**Impact:**
- Better isolation between APs
- Lower collision probability
- Higher aggregate throughput
- Easier for AS-CAC to maintain QoS

---

## AS-CAC Operation in Multi-AP

### Per-AP Admission Control

Each AP runs independent AS-CAC instance:
- Monitors own airtime utilization
- Applies priority-based thresholds
- AS-CAC+ adapts thresholds based on local PER

### Interference Handling

**CCI Scenario:**
- Higher effective utilization due to collisions
- AS-CAC+ detects increased PER
- Dynamically reduces bursty threshold
- Protects VoIP/Video QoS

**ACI Scenario:**
- Lower interference impact
- AS-CAC+ detects healthy conditions
- Increases bursty threshold
- Maximizes channel utilization

---

## Expected Performance

### CCI Scenario (Channel 36 + Channel 36)

| Metric | No CAC | Hard CAC | Soft CAC | AS-CAC+ |
|:-------|-------:|---------:|---------:|--------:|
| Aggregate Throughput | ~45 Mbps | ~35 Mbps | ~42 Mbps | ~44 Mbps |
| VoIP Delay | >50 ms | <2 ms | <2 ms | <2 ms |
| Utilization | 95%+ | 70% | 85% | 92% |

### ACI Scenario (Channel 36 + Channel 40)

| Metric | No CAC | Hard CAC | Soft CAC | AS-CAC+ |
|:-------|-------:|---------:|---------:|--------:|
| Aggregate Throughput | ~55 Mbps | ~42 Mbps | ~50 Mbps | ~52 Mbps |
| VoIP Delay | >40 ms | <2 ms | <2 ms | <2 ms |
| Utilization | 95%+ | 75% | 88% | 94% |

---

## Key Insights

1. **Multi-AP Complexity:** Overlapping coverage creates interference that must be managed
2. **CCI Challenge:** Co-channel deployment is most challenging for admission control
3. **AS-CAC+ Advantage:** Adaptive thresholds handle varying interference conditions
4. **Scalability:** Framework scales to multiple APs with independent control
5. **Realistic Deployment:** 50m separation and 30m coverage simulate enterprise WLAN

---

## Files Generated

- `graphs/multi_ap_topology_2ap.png` - Full 2-AP topology diagram
- `graphs/ap_station_distribution.png` - Detailed single-AP view
- `NETWORK_TOPOLOGY.md` - This documentation

**Status:** Ready for inclusion in research paper Section III (System Model)
