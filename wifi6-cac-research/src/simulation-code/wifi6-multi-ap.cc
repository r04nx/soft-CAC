#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WiFi6MultiApCAC");

// Traffic Types
enum TrafficType {
    VOIP,
    VIDEO,
    BURSTY,
    WEB
};

struct FlowDescriptor {
    uint32_t flowId;
    TrafficType type;
    uint32_t sourceNodeId;
    uint32_t destNodeId;
    uint32_t packetSize;
    double dataRate; // bps
    double requiredAirtime;
    bool admitted;
    Time admissionTime;
    uint32_t apId; // Which AP this flow belongs to
};

// Soft CAC Class
class SoftAirtimeAdmissionControl {
public:
    SoftAirtimeAdmissionControl() 
        : m_voipThreshold(0.90),  // 90% for VoIP (High Priority)
          m_videoThreshold(0.80), // 80% for Video (Medium Priority)
          m_burstyThreshold(0.95), // 95% for Bursty (Low Priority - Fill the gaps)
          m_currentUtilization(0.0) 
    {}

    void SetApId(uint32_t apId) { m_apId = apId; }

    double CalculateRequiredAirtime(uint32_t packetSize, double dataRate, TrafficType type) {
        // Simplified Airtime Calculation for WiFi 6 (802.11ax)
        // Considering overheads, MCS, etc.
        
        // Basic transmission time = PacketSize / DataRate
        // But we need to account for overhead. 
        // 802.11ax overhead is significant but efficiency is high.
        
        double phyRate = 0;
        // Assuming HeMcs5 (approx 50-100 Mbps depending on width/streams)
        // Let's use a conservative estimate for airtime calculation
        // 80 MHz, 1 SS, MCS 5 ~ 250 Mbps raw PHY
        
        // Effective throughput is lower.
        // Let's estimate based on packet size and overhead.
        
        double overhead = 100e-6; // 100us overhead (preamble, IFS, ACK)
        double txTime = (packetSize * 8.0) / 100e6; // Assuming 100Mbps effective link rate
        
        double timePerPacket = txTime + overhead;
        
        double packetsPerSecond = dataRate / (packetSize * 8.0);
        
        double airtime = packetsPerSecond * timePerPacket;
        
        // Safety margin
        airtime *= 1.1; 
        
        return airtime;
    }

    // --- AS-CAC+ ADAPTIVE LOGIC (Simplified) ---
    void AdaptThresholds() {
        // Simplified adaptive logic - called on each admission request
        // Simulate PER based on current utilization
        double simulatedPER = 0.0;
        if (m_currentUtilization > 0.95) simulatedPER = 0.15;
        else if (m_currentUtilization > 0.90) simulatedPER = 0.05;
        else if (m_currentUtilization > 0.80) simulatedPER = 0.01;
        else simulatedPER = 0.001;
        
        // Adaptive adjustment
        if (simulatedPER > 0.05) {
            m_burstyThreshold = std::max(0.80, m_burstyThreshold - 0.01);
        } else if (simulatedPER < 0.02 && m_currentUtilization > 0.70) {
            m_burstyThreshold = std::min(0.98, m_burstyThreshold + 0.01);
        }
    }

    bool RequestAdmission(FlowDescriptor& flow) {
        // Adapt thresholds based on current state
        AdaptThresholds();
    
        flow.requiredAirtime = CalculateRequiredAirtime(flow.packetSize, flow.dataRate, flow.type);
        
        double threshold = 0.0;
        std::string typeStr;
        
        switch(flow.type) {
            case VOIP: 
                threshold = m_voipThreshold; 
                typeStr = "VOIP";
                break;
            case VIDEO: 
                threshold = m_videoThreshold; 
                typeStr = "VIDEO";
                break;
            case BURSTY: 
                threshold = m_burstyThreshold; // Dynamic!
                typeStr = "BURSTY";
                break;
            default: 
                threshold = 0.80;
                typeStr = "OTHER";
        }

        NS_LOG_UNCOND("AP " << m_apId << " Traffic ID: Flow " << flow.flowId << " (" << typeStr << ") requesting " << flow.requiredAirtime << " airtime. Threshold: " << threshold);

        if (m_currentUtilization + flow.requiredAirtime <= threshold) {
            m_currentUtilization += flow.requiredAirtime;
            flow.admitted = true;
            NS_LOG_UNCOND("  -> ADMITTED (AS-CAC+). New AP " << m_apId << " Util: " << m_currentUtilization << " (Threshold: " << threshold << ")");
            return true;
        } else {
            flow.admitted = false;
            NS_LOG_UNCOND("  -> BLOCKED (AS-CAC+). AP " << m_apId << " Util: " << m_currentUtilization << " + " << flow.requiredAirtime << " > " << threshold);
            return false;
        }
    }

    double GetUtilization() const { return m_currentUtilization; }

private:
    double m_voipThreshold;
    double m_videoThreshold;
    double m_burstyThreshold;
    double m_currentUtilization;
    uint32_t m_apId;
};

// Global CAC objects (one per AP)
SoftAirtimeAdmissionControl g_cacAp1;
SoftAirtimeAdmissionControl g_cacAp2;

// Application Helpers
void SetupVoipApp(Ptr<Node> node, Ipv4Address destAddr, uint32_t flowId, uint32_t apId) {
    FlowDescriptor flow;
    flow.flowId = flowId;
    flow.type = VOIP;
    flow.packetSize = 160; // bytes
    flow.dataRate = 64000; // 64 kbps
    flow.apId = apId;

    SoftAirtimeAdmissionControl* cac = (apId == 1) ? &g_cacAp1 : &g_cacAp2;

    if (cac->RequestAdmission(flow)) {
        uint16_t port = 9000 + flowId;
        OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(destAddr, port));
        onoff.SetConstantRate(DataRate("64kbps"), 160);
        onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
        onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
        
        ApplicationContainer app = onoff.Install(node);
        app.Start(Seconds(1.0 + (flowId * 0.01))); // Stagger start
        app.Stop(Seconds(10.0));
        
        // Packet Sink at AP
        PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        // We need to install sink on the correct AP node, but we don't have reference here easily
        // For simplicity in this script structure, we'll install sinks in main
    }
}

void SetupVideoApp(Ptr<Node> node, Ipv4Address destAddr, uint32_t flowId, uint32_t apId) {
    FlowDescriptor flow;
    flow.flowId = flowId;
    flow.type = VIDEO;
    flow.packetSize = 1400;
    flow.dataRate = 3000000; // 3 Mbps
    flow.apId = apId;

    SoftAirtimeAdmissionControl* cac = (apId == 1) ? &g_cacAp1 : &g_cacAp2;

    if (cac->RequestAdmission(flow)) {
        uint16_t port = 9000 + flowId;
        OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(destAddr, port));
        onoff.SetConstantRate(DataRate("3Mbps"), 1400);
        
        ApplicationContainer app = onoff.Install(node);
        app.Start(Seconds(1.0 + (flowId * 0.05)));
        app.Stop(Seconds(10.0));
    }
}

void SetupBurstyApp(Ptr<Node> node, Ipv4Address destAddr, uint32_t flowId, uint32_t apId) {
    FlowDescriptor flow;
    flow.flowId = flowId;
    flow.type = BURSTY;
    flow.packetSize = 1400;
    flow.dataRate = 5000000; // 5 Mbps peak
    flow.apId = apId;

    SoftAirtimeAdmissionControl* cac = (apId == 1) ? &g_cacAp1 : &g_cacAp2;

    if (cac->RequestAdmission(flow)) {
        uint16_t port = 9000 + flowId;
        OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(destAddr, port));
        onoff.SetAttribute("DataRate", StringValue("5Mbps"));
        onoff.SetAttribute("PacketSize", UintegerValue(1400));
        onoff.SetAttribute("OnTime", StringValue("ns3::ExponentialRandomVariable[Mean=0.2]"));
        onoff.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=0.8]"));
        
        ApplicationContainer app = onoff.Install(node);
        app.Start(Seconds(1.0 + (flowId * 0.1)));
        app.Stop(Seconds(10.0));
    }
}

int main(int argc, char *argv[]) {
    uint32_t nStationsPerAp = 20;
    bool useCci = true; // Co-Channel Interference (Same channel)
    
    CommandLine cmd;
    cmd.AddValue("nStationsPerAp", "Number of stations per AP", nStationsPerAp);
    cmd.AddValue("useCci", "Enable Co-Channel Interference (true=Same Channel, false=Different)", useCci);
    cmd.Parse(argc, argv);

    g_cacAp1.SetApId(1);
    g_cacAp2.SetApId(2);

    NodeContainer wifiApNodes;
    wifiApNodes.Create(2); // 2 APs

    NodeContainer wifiStaNodesAp1;
    wifiStaNodesAp1.Create(nStationsPerAp);
    
    NodeContainer wifiStaNodesAp2;
    wifiStaNodesAp2.Create(nStationsPerAp);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper();
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    
    WifiMacHelper mac;
    Ssid ssid1 = Ssid("ns-3-ssid-ap1");
    Ssid ssid2 = Ssid("ns-3-ssid-ap2");

    // Setup AP 1
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid1));
    NetDeviceContainer apDevice1 = wifi.Install(phy, mac, wifiApNodes.Get(0));

    // Setup AP 2
    // If NOT CCI (ACI/Clean), we would ideally use a different channel object or configure channel number
    // But YansWifiChannel is shared medium. To simulate ACI properly in simple Yans, 
    // we usually need different channel objects or use SpectrumWifiPhy.
    // For simplicity in this demo, if useCci is false, we will create a SECOND channel object
    // effectively isolating them (perfect separation/clean channels).
    
    NetDeviceContainer apDevice2;
    if (!useCci) {
        YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
        YansWifiPhyHelper phy2 = YansWifiPhyHelper();
        phy2.SetChannel(channel2.Create());
        mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid2));
        apDevice2 = wifi.Install(phy2, mac, wifiApNodes.Get(1));
    } else {
        // Same channel object = Interference
        mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid2));
        apDevice2 = wifi.Install(phy, mac, wifiApNodes.Get(1));
    }

    // Setup STAs for AP 1
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid1));
    NetDeviceContainer staDevices1 = wifi.Install(phy, mac, wifiStaNodesAp1);

    // Setup STAs for AP 2
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid2));
    NetDeviceContainer staDevices2;
    if (!useCci) {
        // Need to use the second phy/channel helper
        YansWifiPhyHelper phy2 = YansWifiPhyHelper(); 
        // Note: In a real script we'd refactor this, but for brevity:
        // We need to grab the channel created above. 
        // For now, let's just assume if !useCci, we use a new default channel/phy for AP2 clients too.
        YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
        phy2.SetChannel(channel2.Create());
        staDevices2 = wifi.Install(phy2, mac, wifiStaNodesAp2);
    } else {
        staDevices2 = wifi.Install(phy, mac, wifiStaNodesAp2);
    }

    // Mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
        "MinX", DoubleValue(0.0), "MinY", DoubleValue(0.0),
        "DeltaX", DoubleValue(5.0), "DeltaY", DoubleValue(5.0),
        "GridWidth", UintegerValue(10), "LayoutType", StringValue("RowFirst"));

    // AP 1 at (10, 10), AP 2 at (30, 10) - Overlapping if range > 10m
    Ptr<ListPositionAllocator> apPos = CreateObject<ListPositionAllocator>();
    apPos->Add(Vector(10.0, 10.0, 0.0));
    apPos->Add(Vector(30.0, 10.0, 0.0));
    
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(apPos);
    mobility.Install(wifiApNodes);

    // STAs around APs
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", DoubleValue(10.0), "Y", DoubleValue(10.0), "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=15]"));
    mobility.Install(wifiStaNodesAp1);

    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", DoubleValue(30.0), "Y", DoubleValue(10.0), "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=15]"));
    mobility.Install(wifiStaNodesAp2);

    // Internet Stack
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodesAp1);
    stack.Install(wifiStaNodesAp2);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ap1Interface = address.Assign(apDevice1);
    Ipv4InterfaceContainer sta1Interfaces = address.Assign(staDevices1);

    address.SetBase("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ap2Interface = address.Assign(apDevice2);
    Ipv4InterfaceContainer sta2Interfaces = address.Assign(staDevices2);

    // Install Applications
    // Mix of traffic for both APs
    uint32_t flowId = 0;
    
    // AP 1 Traffic
    for (uint32_t i = 0; i < nStationsPerAp; ++i) {
        if (i % 3 == 0) SetupVoipApp(wifiStaNodesAp1.Get(i), ap1Interface.GetAddress(0), flowId++, 1);
        else if (i % 3 == 1) SetupVideoApp(wifiStaNodesAp1.Get(i), ap1Interface.GetAddress(0), flowId++, 1);
        else SetupBurstyApp(wifiStaNodesAp1.Get(i), ap1Interface.GetAddress(0), flowId++, 1);
    }

    // AP 2 Traffic
    for (uint32_t i = 0; i < nStationsPerAp; ++i) {
        if (i % 3 == 0) SetupVoipApp(wifiStaNodesAp2.Get(i), ap2Interface.GetAddress(0), flowId++, 2);
        else if (i % 3 == 1) SetupVideoApp(wifiStaNodesAp2.Get(i), ap2Interface.GetAddress(0), flowId++, 2);
        else SetupBurstyApp(wifiStaNodesAp2.Get(i), ap2Interface.GetAddress(0), flowId++, 2);
    }
    
    // Install Packet Sinks (Catch-all)
    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9000));
    // We need to install sinks on APs for all possible ports. 
    // For simplicity, we assume port range 9000-9200
    for (uint16_t p = 9000; p < 9200; ++p) {
         PacketSinkHelper s("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), p));
         s.Install(wifiApNodes.Get(0));
         s.Install(wifiApNodes.Get(1));
    }

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // NetAnim (Disabled for speed)
    // Enable NetAnim for topology visualization
    AnimationInterface anim("wifi6-multi-ap-topology.xml");
    
    // Set node descriptions for better visualization
    for (uint32_t i = 0; i < wifiApNodes.GetN(); ++i) {
        anim.UpdateNodeDescription(wifiApNodes.Get(i), "AP" + std::to_string(i+1));
        anim.UpdateNodeColor(wifiApNodes.Get(i), 0, 0, 255); // Blue for APs
        anim.UpdateNodeSize(wifiApNodes.Get(i)->GetId(), 5, 5); // Larger size for APs
    }
    
    for (uint32_t i = 0; i < wifiStaNodesAp1.GetN(); ++i) {
        anim.UpdateNodeDescription(wifiStaNodesAp1.Get(i), "STA1-" + std::to_string(i+1));
        anim.UpdateNodeColor(wifiStaNodesAp1.Get(i), 0, 255, 0); // Green for stations
    }
    
    for (uint32_t i = 0; i < wifiStaNodesAp2.GetN(); ++i) {
        anim.UpdateNodeDescription(wifiStaNodesAp2.Get(i), "STA2-" + std::to_string(i+1));
        anim.UpdateNodeColor(wifiStaNodesAp2.Get(i), 0, 255, 0); // Green for stations
    }
    // anim.SetMaxPktsPerTraceFile(500000);
    // anim.UpdateNodeDescription(wifiApNodes.Get(0), "AP1 (CCI)");
    // anim.UpdateNodeDescription(wifiApNodes.Get(1), "AP2 (CCI)");
    // anim.UpdateNodeColor(wifiApNodes.Get(0), 0, 0, 255);
    // anim.UpdateNodeColor(wifiApNodes.Get(1), 0, 0, 255);

    NS_LOG_UNCOND("Starting Multi-AP Simulation (CCI=" << useCci << ")...");
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalThroughput = 0;
    double totalDelay = 0;
    uint32_t flowCount = 0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i) {
        if (i->second.rxBytes > 0) {
            totalThroughput += i->second.rxBytes * 8.0 / 10.0 / 1000 / 1000; // Mbps
            totalDelay += i->second.delaySum.GetSeconds() / i->second.rxPackets;
            flowCount++;
        }
    }

    NS_LOG_UNCOND("Total Throughput: " << totalThroughput << " Mbps");
    NS_LOG_UNCOND("Avg Delay: " << (flowCount > 0 ? totalDelay/flowCount : 0) << " s");
    NS_LOG_UNCOND("AP1 Utilization: " << g_cacAp1.GetUtilization());
    NS_LOG_UNCOND("AP2 Utilization: " << g_cacAp2.GetUtilization());

    Simulator::Destroy();
    return 0;
}
