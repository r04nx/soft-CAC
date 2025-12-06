/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2024
 *
 * WiFi 6 Airtime-Based Call Admission Control Implementation
 */

#include "wifi6-cac-airtime.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <algorithm>
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AirtimeAdmissionControl");
NS_OBJECT_ENSURE_REGISTERED(AirtimeAdmissionControl);

TypeId
AirtimeAdmissionControl::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::AirtimeAdmissionControl")
        .SetParent<Object>()
        .SetGroupName("Wifi")
        .AddConstructor<AirtimeAdmissionControl>();
    return tid;
}

AirtimeAdmissionControl::AirtimeAdmissionControl()
    : m_airtimeThreshold(0.80),  // 80% threshold for dense environments
      m_currentAirtimeUtilization(0.0),
      m_nextFlowId(1),
      m_totalFlowRequests(0),
      m_blockedFlows(0),
      m_channelWidth(80),        // 80 MHz for WiFi 6
      m_guardInterval(800),      // 800 ns GI
      m_nss(2)                   // 2 spatial streams
{
    NS_LOG_FUNCTION(this);
}

AirtimeAdmissionControl::~AirtimeAdmissionControl()
{
    NS_LOG_FUNCTION(this);
}

void
AirtimeAdmissionControl::SetAirtimeThreshold(double threshold)
{
    NS_LOG_FUNCTION(this << threshold);
    NS_ASSERT(threshold > 0.0 && threshold <= 1.0);
    m_airtimeThreshold = threshold;
}

double
AirtimeAdmissionControl::GetAirtimeThreshold() const
{
    return m_airtimeThreshold;
}

void
AirtimeAdmissionControl::SetWifiPhyParameters(uint16_t channelWidth, uint16_t guardInterval, uint8_t nss)
{
    NS_LOG_FUNCTION(this << channelWidth << guardInterval << (uint16_t)nss);
    m_channelWidth = channelWidth;
    m_guardInterval = guardInterval;
    m_nss = nss;
}

AcIndex
AirtimeAdmissionControl::GetAccessCategory(TrafficType type)
{
    switch (type) {
        case VOIP:
            return AC_VO;  // Voice - highest priority
        case VIDEO_STREAM:
            return AC_VI;  // Video
        case BURSTY:
        case WEB_BROWSING:
        default:
            return AC_BE;  // Best effort
    }
}

uint32_t
AirtimeAdmissionControl::GetPriority(TrafficType type)
{
    switch (type) {
        case VOIP:
            return 4;  // Highest priority
        case VIDEO_STREAM:
            return 3;
        case BURSTY:
            return 2;
        case WEB_BROWSING:
            return 1;
        default:
            return 0;
    }
}

uint32_t
AirtimeAdmissionControl::GetMacOverhead(TrafficType type)
{
    // MAC header (30 bytes) + LLC/SNAP (8 bytes) + FCS (4 bytes)
    uint32_t baseOverhead = 42;
    
    // Add QoS overhead for voice and video
    if (type == VOIP || type == VIDEO_STREAM) {
        baseOverhead += 2;  // QoS control field
    }
    
    return baseOverhead;
}

double
AirtimeAdmissionControl::CalculatePhyTxTime(uint32_t packetSize)
{
    // WiFi 6 (802.11ax) PHY parameters
    const double symbolDuration = 13.6e-6;  // 13.6 μs symbol duration (with 0.8 μs GI)
    const uint32_t phyPreamble = 40;        // 40 μs preamble for HE format
    
    // Calculate data rate based on channel width and NSS
    // Using conservative MCS 5 for reliability in dense environment
    // MCS 5: 64-QAM, coding rate 3/4
    double bitsPerSymbol = 0;
    
    if (m_channelWidth == 20) {
        bitsPerSymbol = 234 * m_nss;  // 234 bits per symbol per stream for 20 MHz
    } else if (m_channelWidth == 40) {
        bitsPerSymbol = 468 * m_nss;  // 468 bits per symbol per stream for 40 MHz
    } else if (m_channelWidth == 80) {
        bitsPerSymbol = 980 * m_nss;  // 980 bits per symbol per stream for 80 MHz
    } else if (m_channelWidth == 160) {
        bitsPerSymbol = 1960 * m_nss; // 1960 bits per symbol per stream for 160 MHz
    }
    
    // Calculate number of symbols needed
    uint32_t totalBits = packetSize * 8;
    uint32_t numSymbols = std::ceil(static_cast<double>(totalBits) / bitsPerSymbol);
    
    // Total transmission time = preamble + (symbols * symbol duration)
    double txTime = (phyPreamble * 1e-6) + (numSymbols * symbolDuration);
    
    return txTime;
}

double
AirtimeAdmissionControl::CalculateRequiredAirtime(uint32_t packetSize, double dataRate, TrafficType type)
{
    NS_LOG_FUNCTION(this << packetSize << dataRate << type);
    
    // Add MAC overhead
    uint32_t totalPacketSize = packetSize + GetMacOverhead(type);
    
    // Calculate PHY transmission time
    double txTime = CalculatePhyTxTime(totalPacketSize);
    
    // Add MAC protocol overhead
    const double difs = 34e-6;           // DIFS: 34 μs
    const double sifs = 16e-6;           // SIFS: 16 μs
    const double ackTime = 44e-6;        // ACK transmission time
    const double avgBackoff = 67.5e-6;   // Average backoff time (CW_min = 15)
    
    // Total time per packet transmission
    double timePerPacket = difs + avgBackoff + txTime + sifs + ackTime;
    
    // Calculate packet rate from data rate
    double packetRate = dataRate / (packetSize * 8.0);
    
    // Required airtime = packets per second * time per packet
    double requiredAirtime = packetRate * timePerPacket;
    
    // Add safety margin for retransmissions and contention (10%)
    requiredAirtime *= 1.10;
    
    NS_LOG_DEBUG("Flow airtime calculation: packetSize=" << packetSize 
                 << " dataRate=" << dataRate 
                 << " packetRate=" << packetRate
                 << " timePerPacket=" << timePerPacket
                 << " requiredAirtime=" << requiredAirtime);
    
    return requiredAirtime;
}

bool
AirtimeAdmissionControl::RequestAdmission(FlowDescriptor& flow)
{
    NS_LOG_FUNCTION(this << flow.flowId);
    
    m_totalFlowRequests++;
    
    // Calculate required airtime for this flow
    flow.requiredAirtime = CalculateRequiredAirtime(flow.packetSize, flow.dataRate, flow.type);
    
    // Set QoS parameters
    flow.accessCategory = GetAccessCategory(flow.type);
    flow.priority = GetPriority(flow.type);
    
    // Check if admission is possible
    double newUtilization = m_currentAirtimeUtilization + flow.requiredAirtime;
    
    if (newUtilization <= m_airtimeThreshold) {
        // Admit the flow
        flow.flowId = m_nextFlowId++;
        flow.admitted = true;
        flow.admissionTime = Simulator::Now();
        
        m_admittedFlows[flow.flowId] = flow;
        m_currentAirtimeUtilization = newUtilization;
        
        // Initialize flow statistics
        FlowStats stats;
        stats.txPackets = 0;
        stats.rxPackets = 0;
        stats.txBytes = 0;
        stats.rxBytes = 0;
        stats.totalDelay = 0.0;
        stats.delayPackets = 0;
        stats.maxDelay = 0.0;
        stats.minDelay = std::numeric_limits<double>::max();
        stats.firstPacketTime = Simulator::Now();
        stats.lastPacketTime = Simulator::Now();
        m_flowStats[flow.flowId] = stats;
        
        NS_LOG_INFO("Flow " << flow.flowId << " ADMITTED. Type=" << flow.type 
                    << " RequiredAirtime=" << flow.requiredAirtime 
                    << " NewUtilization=" << newUtilization);
        
        return true;
    } else {
        // Reject the flow
        flow.admitted = false;
        m_blockedFlows++;
        
        NS_LOG_INFO("Flow BLOCKED. Type=" << flow.type 
                    << " RequiredAirtime=" << flow.requiredAirtime 
                    << " CurrentUtilization=" << m_currentAirtimeUtilization
                    << " WouldBe=" << newUtilization
                    << " Threshold=" << m_airtimeThreshold);
        
        return false;
    }
}

void
AirtimeAdmissionControl::ReleaseFlow(uint32_t flowId)
{
    NS_LOG_FUNCTION(this << flowId);
    
    auto it = m_admittedFlows.find(flowId);
    if (it != m_admittedFlows.end()) {
        m_currentAirtimeUtilization -= it->second.requiredAirtime;
        m_admittedFlows.erase(it);
        
        NS_LOG_INFO("Flow " << flowId << " released. New utilization=" << m_currentAirtimeUtilization);
    }
}

double
AirtimeAdmissionControl::GetCurrentAirtimeUtilization() const
{
    return m_currentAirtimeUtilization;
}

uint32_t
AirtimeAdmissionControl::GetAdmittedFlowCount() const
{
    return m_admittedFlows.size();
}

uint32_t
AirtimeAdmissionControl::GetBlockedFlowCount() const
{
    return m_blockedFlows;
}

double
AirtimeAdmissionControl::GetBlockingProbability() const
{
    if (m_totalFlowRequests == 0) {
        return 0.0;
    }
    return static_cast<double>(m_blockedFlows) / static_cast<double>(m_totalFlowRequests);
}

void
AirtimeAdmissionControl::UpdateFlowStats(uint32_t flowId, uint32_t packetSize, double delay)
{
    auto it = m_flowStats.find(flowId);
    if (it != m_flowStats.end()) {
        it->second.rxPackets++;
        it->second.rxBytes += packetSize;
        it->second.totalDelay += delay;
        it->second.delayPackets++;
        it->second.maxDelay = std::max(it->second.maxDelay, delay);
        it->second.minDelay = std::min(it->second.minDelay, delay);
        it->second.lastPacketTime = Simulator::Now();
        it->second.delayVector.push_back(delay);
    }
}

FlowStats
AirtimeAdmissionControl::GetFlowStats(uint32_t flowId) const
{
    auto it = m_flowStats.find(flowId);
    if (it != m_flowStats.end()) {
        return it->second;
    }
    return FlowStats();
}

std::vector<FlowDescriptor>
AirtimeAdmissionControl::GetAdmittedFlows() const
{
    std::vector<FlowDescriptor> flows;
    for (const auto& pair : m_admittedFlows) {
        flows.push_back(pair.second);
    }
    return flows;
}

void
AirtimeAdmissionControl::PrintStatistics(std::ostream& os) const
{
    os << "\n=== Airtime-Based CAC Statistics ===\n";
    os << "Airtime Threshold: " << m_airtimeThreshold << "\n";
    os << "Current Airtime Utilization: " << m_currentAirtimeUtilization << "\n";
    os << "Total Flow Requests: " << m_totalFlowRequests << "\n";
    os << "Admitted Flows: " << m_admittedFlows.size() << "\n";
    os << "Blocked Flows: " << m_blockedFlows << "\n";
    os << "Blocking Probability: " << GetBlockingProbability() << "\n";
    os << "\n=== Per-Flow Statistics ===\n";
    
    for (const auto& pair : m_flowStats) {
        uint32_t flowId = pair.first;
        const FlowStats& stats = pair.second;
        
        double avgDelay = (stats.delayPackets > 0) ? (stats.totalDelay / stats.delayPackets) : 0.0;
        double throughput = 0.0;
        
        if (stats.lastPacketTime > stats.firstPacketTime) {
            double duration = (stats.lastPacketTime - stats.firstPacketTime).GetSeconds();
            throughput = (stats.rxBytes * 8.0) / duration / 1e6;  // Mbps
        }
        
        os << "Flow " << flowId << ":\n";
        os << "  RX Packets: " << stats.rxPackets << "\n";
        os << "  RX Bytes: " << stats.rxBytes << "\n";
        os << "  Throughput: " << throughput << " Mbps\n";
        os << "  Avg Delay: " << avgDelay * 1000 << " ms\n";
        os << "  Min Delay: " << stats.minDelay * 1000 << " ms\n";
        os << "  Max Delay: " << stats.maxDelay * 1000 << " ms\n";
    }
}

} // namespace ns3
