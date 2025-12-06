/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2024
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * WiFi 6 Airtime-Based Call Admission Control
 * Novel CAC mechanism for dense WLAN environments
 */

#ifndef WIFI6_CAC_AIRTIME_H
#define WIFI6_CAC_AIRTIME_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include <map>
#include <vector>

namespace ns3 {

/**
 * \brief Traffic type enumeration
 */
enum TrafficType {
    VOIP,           // Voice over IP
    VIDEO_STREAM,   // Video streaming
    BURSTY,         // Bursty file transfers
    WEB_BROWSING    // HTTP web browsing
};

/**
 * \brief Flow descriptor for CAC
 */
struct FlowDescriptor {
    uint32_t flowId;
    TrafficType type;
    Mac48Address source;
    Mac48Address destination;
    uint32_t packetSize;      // Average packet size in bytes
    double dataRate;          // Data rate in bps
    double requiredAirtime;   // Required airtime fraction
    Time admissionTime;       // When flow was admitted
    bool admitted;            // Admission status
    
    // QoS parameters
    AcIndex accessCategory;   // WiFi QoS access category
    uint32_t priority;        // Priority level (higher = more important)
};

/**
 * \brief Statistics for admitted flows
 */
struct FlowStats {
    uint64_t txPackets;
    uint64_t rxPackets;
    uint64_t txBytes;
    uint64_t rxBytes;
    double totalDelay;        // Sum of all packet delays
    uint32_t delayPackets;    // Number of packets with delay measurements
    double maxDelay;
    double minDelay;
    Time firstPacketTime;
    Time lastPacketTime;
    std::vector<double> delayVector; // For detailed analysis
};

/**
 * \brief Airtime-based Call Admission Control for WiFi 6
 * 
 * This class implements an airtime-based admission control mechanism
 * specifically designed for dense WiFi 6 (802.11ax) environments.
 * It calculates the airtime requirements for each flow and admits
 * flows only if sufficient airtime is available.
 */
class AirtimeAdmissionControl : public Object
{
public:
    /**
     * \brief Get the type ID
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    
    /**
     * \brief Constructor
     */
    AirtimeAdmissionControl();
    
    /**
     * \brief Destructor
     */
    virtual ~AirtimeAdmissionControl();
    
    /**
     * \brief Set the airtime threshold for admission control
     * \param threshold Maximum allowed airtime utilization (0.0 to 1.0)
     */
    void SetAirtimeThreshold(double threshold);
    
    /**
     * \brief Get current airtime threshold
     * \return Current threshold value
     */
    double GetAirtimeThreshold() const;
    
    /**
     * \brief Request admission for a new flow
     * \param flow Flow descriptor
     * \return true if flow is admitted, false otherwise
     */
    bool RequestAdmission(FlowDescriptor& flow);
    
    /**
     * \brief Release a flow (flow termination)
     * \param flowId ID of the flow to release
     */
    void ReleaseFlow(uint32_t flowId);
    
    /**
     * \brief Calculate required airtime for a flow
     * \param packetSize Average packet size in bytes
     * \param dataRate Data rate in bps
     * \param type Traffic type
     * \return Required airtime fraction (0.0 to 1.0)
     */
    double CalculateRequiredAirtime(uint32_t packetSize, double dataRate, TrafficType type);
    
    /**
     * \brief Get current total airtime utilization
     * \return Current airtime utilization (0.0 to 1.0)
     */
    double GetCurrentAirtimeUtilization() const;
    
    /**
     * \brief Get number of admitted flows
     * \return Number of currently admitted flows
     */
    uint32_t GetAdmittedFlowCount() const;
    
    /**
     * \brief Get number of blocked flows
     * \return Total number of blocked flow requests
     */
    uint32_t GetBlockedFlowCount() const;
    
    /**
     * \brief Get blocking probability
     * \return Blocking probability (blocked / total requests)
     */
    double GetBlockingProbability() const;
    
    /**
     * \brief Get flow statistics
     * \param flowId Flow ID
     * \return Flow statistics structure
     */
    FlowStats GetFlowStats(uint32_t flowId) const;
    
    /**
     * \brief Update flow statistics
     * \param flowId Flow ID
     * \param packetSize Packet size
     * \param delay End-to-end delay
     */
    void UpdateFlowStats(uint32_t flowId, uint32_t packetSize, double delay);
    
    /**
     * \brief Get all admitted flows
     * \return Vector of admitted flow descriptors
     */
    std::vector<FlowDescriptor> GetAdmittedFlows() const;
    
    /**
     * \brief Print admission control statistics
     */
    void PrintStatistics(std::ostream& os) const;
    
    /**
     * \brief Set WiFi PHY parameters for airtime calculation
     * \param channelWidth Channel width in MHz
     * \param guardInterval Guard interval in nanoseconds
     * \param nss Number of spatial streams
     */
    void SetWifiPhyParameters(uint16_t channelWidth, uint16_t guardInterval, uint8_t nss);

private:
    double m_airtimeThreshold;              ///< Maximum airtime threshold
    double m_currentAirtimeUtilization;     ///< Current total airtime usage
    uint32_t m_nextFlowId;                  ///< Next flow ID to assign
    uint32_t m_totalFlowRequests;           ///< Total admission requests
    uint32_t m_blockedFlows;                ///< Number of blocked flows
    
    std::map<uint32_t, FlowDescriptor> m_admittedFlows;  ///< Currently admitted flows
    std::map<uint32_t, FlowStats> m_flowStats;           ///< Flow statistics
    
    // WiFi PHY parameters for airtime calculation
    uint16_t m_channelWidth;     ///< Channel width in MHz
    uint16_t m_guardInterval;    ///< Guard interval in ns
    uint8_t m_nss;               ///< Number of spatial streams
    
    /**
     * \brief Calculate PHY transmission time
     * \param packetSize Packet size in bytes
     * \return Transmission time in seconds
     */
    double CalculatePhyTxTime(uint32_t packetSize);
    
    /**
     * \brief Get MAC overhead for traffic type
     * \param type Traffic type
     * \return MAC overhead in bytes
     */
    uint32_t GetMacOverhead(TrafficType type);
    
    /**
     * \brief Get access category for traffic type
     * \param type Traffic type
     * \return WiFi QoS access category
     */
    AcIndex GetAccessCategory(TrafficType type);
    
    /**
     * \brief Get priority for traffic type
     * \param type Traffic type
     * \return Priority value (higher = more important)
     */
    uint32_t GetPriority(TrafficType type);
};

} // namespace ns3

#endif /* WIFI6_CAC_AIRTIME_H */
