/* WiFi 6 CAC Simulation - Simplified Demo Version */

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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wifi6CacDemo");

// Simple CAC statistics
struct CacStats {
    uint32_t totalRequests = 0;
    uint32_t admittedFlows = 0;
    uint32_t blockedFlows = 0;
    double currentAirtime = 0.0;
    double threshold = 0.80;
};

CacStats g_cacStats;
std::ofstream g_resultsFile;

// Simple airtime calculation
double CalculateAirtime(uint32_t packetSize, double dataRate) {
    double packetsPerSecond = dataRate / (packetSize * 8.0);
    double txTime = (packetSize + 50) * 8.0 / 20e6;
    double overhead = 150e-6;
    return packetsPerSecond * (txTime + overhead);
}

// Simple admission control
bool AdmitFlow(uint32_t packetSize, double dataRate, std::string type) {
    g_cacStats.totalRequests++;
    double requiredAirtime = CalculateAirtime(packetSize, dataRate);
    
    if (g_cacStats.currentAirtime + requiredAirtime <= g_cacStats.threshold) {
        g_cacStats.admittedFlows++;
        g_cacStats.currentAirtime += requiredAirtime;
        
        g_resultsFile << type << ",ADMITTED," << requiredAirtime << ","
                     << g_cacStats.currentAirtime << "\n";
        NS_LOG_INFO(type << " flow ADMITTED. Airtime: " << requiredAirtime);
        return true;
    } else {
        g_cacStats.blockedFlows++;
        g_resultsFile << type << ",BLOCKED," << requiredAirtime << ","
                     << g_cacStats.currentAirtime << "\n";
        NS_LOG_INFO(type << " flow BLOCKED.");
        return false;
    }
}

int main(int argc, char *argv[]) {
    uint32_t nStations = 30;
    double simTime = 10.0;
    bool enableCac = true;
    
    CommandLine cmd;
    cmd.AddValue("nStations", "Number of stations", nStations);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("enableCac", "Enable CAC", enableCac);
    cmd.Parse(argc, argv);
    
    LogComponentEnable("Wifi6CacDemo", LOG_LEVEL_INFO);
    
    NS_LOG_INFO("=== WiFi 6 CAC Demo ===");
    NS_LOG_INFO("Stations: " << nStations << ", CAC: " << (enableCac ? "ON" : "OFF"));
    
    g_resultsFile.open("wifi6-cac-demo-results.csv");
    g_resultsFile << "TrafficType,Status,RequiredAirtime,TotalAirtime\n";
    
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nStations);
    NodeContainer wifiApNode;
    wifiApNode.Create(1);
    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.Set("ChannelSettings", StringValue("{0, 80, BAND_5GHZ, 0}"));
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                  "DataMode", StringValue("HeMcs5"));
    
    WifiMacHelper mac;
    Ssid ssid = Ssid("wifi6-demo");
    
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevices = wifi.Install(phy, mac, wifiStaNodes);
    
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(phy, mac, wifiApNode);
    
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                  "X", DoubleValue(0.0),
                                  "Y", DoubleValue(0.0),
                                  "Rho", StringValue("ns3::UniformRandomVariable[Min=1.0|Max=15.0]"));
    mobility.Install(wifiStaNodes);
    
    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);
    
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
    Ipv4InterfaceContainer apInterface = address.Assign(apDevice);
    
    uint16_t port = 9;
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sinkHelper.Install(wifiApNode);
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simTime));
    
    NS_LOG_INFO("\n=== Generating Traffic ===");
    
    // VoIP flows
    uint32_t nVoip = nStations * 0.4;
    for (uint32_t i = 0; i < nVoip && i < nStations; i++) {
        bool admitted = enableCac ? AdmitFlow(160, 64000, "VoIP") : true;
        
        if (admitted) {
            OnOffHelper onoff("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), port));
            onoff.SetAttribute("PacketSize", UintegerValue(160));
            onoff.SetAttribute("DataRate", DataRateValue(DataRate("64kbps")));
            onoff.SetConstantRate(DataRate("64kbps"));
            
            ApplicationContainer app = onoff.Install(wifiStaNodes.Get(i));
            app.Start(Seconds(1.0));
            app.Stop(Seconds(simTime));
        }
    }
    
    // Video flows
    uint32_t nVideo = nStations * 0.3;
    for (uint32_t i = 0; i < nVideo && (nVoip + i) < nStations; i++) {
        uint32_t nodeIdx = nVoip + i;
        bool admitted = enableCac ? AdmitFlow(1200, 3000000, "Video") : true;
        
        if (admitted) {
            OnOffHelper onoff("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), port));
            onoff.SetAttribute("PacketSize", UintegerValue(1200));
            onoff.SetAttribute("DataRate", DataRateValue(DataRate("3Mbps")));
            onoff.SetConstantRate(DataRate("3Mbps"));
            
            ApplicationContainer app = onoff.Install(wifiStaNodes.Get(nodeIdx));
            app.Start(Seconds(2.0));
            app.Stop(Seconds(simTime));
        }
    }
    
    // Bursty flows
    uint32_t nBursty = nStations - nVoip - nVideo;
    for (uint32_t i = 0; i < nBursty; i++) {
        uint32_t nodeIdx = nVoip + nVideo + i;
        if (nodeIdx >= nStations) break;
        
        bool admitted = enableCac ? AdmitFlow(1400, 2500000, "Bursty") : true;
        
        if (admitted) {
            OnOffHelper onoff("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), port));
            onoff.SetAttribute("PacketSize", UintegerValue(1400));
            onoff.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps")));
            onoff.SetAttribute("OnTime", StringValue("ns3::ExponentialRandomVariable[Mean=1.0]"));
            onoff.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=1.0]"));
            
            ApplicationContainer app = onoff.Install(wifiStaNodes.Get(nodeIdx));
            app.Start(Seconds(3.0));
            app.Stop(Seconds(simTime));
        }
    }
    
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    // NetAnim setup
    AnimationInterface anim("wifi6-cac-animation.xml");
    anim.SetMaxPktsPerTraceFile(50000);
    anim.EnablePacketMetadata(true);
    
    // Set node descriptions
    anim.UpdateNodeDescription(wifiApNode.Get(0), "AP (WiFi 6)");
    anim.UpdateNodeColor(wifiApNode.Get(0), 0, 0, 255); // Blue AP
    
    for (uint32_t i = 0; i < nStations; ++i) {
        anim.UpdateNodeDescription(wifiStaNodes.Get(i), "STA " + std::to_string(i));
        // Color coding based on traffic type (approximate)
        if (i < nVoip) anim.UpdateNodeColor(wifiStaNodes.Get(i), 0, 255, 0); // Green (VoIP)
        else if (i < nVoip + nVideo) anim.UpdateNodeColor(wifiStaNodes.Get(i), 255, 165, 0); // Orange (Video)
        else anim.UpdateNodeColor(wifiStaNodes.Get(i), 128, 128, 128); // Grey (Bursty)
    }

    NS_LOG_INFO("\n=== Running Simulation ===");
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    
    NS_LOG_INFO("\n=== Results ===");
    NS_LOG_INFO("Total Requests: " << g_cacStats.totalRequests);
    NS_LOG_INFO("Admitted: " << g_cacStats.admittedFlows);
    NS_LOG_INFO("Blocked: " << g_cacStats.blockedFlows);
    NS_LOG_INFO("Blocking Prob: " 
                << (double)g_cacStats.blockedFlows / g_cacStats.totalRequests * 100 << "%");
    NS_LOG_INFO("Airtime Used: " << g_cacStats.currentAirtime);
    
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    
    double totalThroughput = 0.0;
    double totalDelay = 0.0;
    uint32_t flowCount = 0;
    
    for (auto const &flow : stats) {
        if (flow.second.rxPackets > 0) {
            double throughput = flow.second.rxBytes * 8.0 / simTime / 1e6;
            double delay = flow.second.delaySum.GetMilliSeconds() / flow.second.rxPackets;
            totalThroughput += throughput;
            totalDelay += delay;
            flowCount++;
        }
    }
    
    NS_LOG_INFO("Active Flows: " << flowCount);
    NS_LOG_INFO("Aggregate Throughput: " << totalThroughput << " Mbps");
    NS_LOG_INFO("Average Delay: " << (flowCount > 0 ? totalDelay / flowCount : 0) << " ms");
    
    std::ofstream summary("wifi6-cac-demo-summary.txt");
    summary << "WiFi 6 CAC Demo Results\n";
    summary << "=======================\n\n";
    summary << "Config: " << nStations << " stations, CAC " << (enableCac ? "ON" : "OFF") << "\n";
    summary << "Requests: " << g_cacStats.totalRequests << "\n";
    summary << "Admitted: " << g_cacStats.admittedFlows << "\n";
    summary << "Blocked: " << g_cacStats.blockedFlows << "\n";
    summary << "Blocking: " << (double)g_cacStats.blockedFlows / g_cacStats.totalRequests * 100 << "%\n";
    summary << "Airtime: " << g_cacStats.currentAirtime << "\n";
    summary << "Flows: " << flowCount << "\n";
    summary << "Throughput: " << totalThroughput << " Mbps\n";
    summary << "Avg Delay: " << (flowCount > 0 ? totalDelay / flowCount : 0) << " ms\n";
    summary.close();
    
    g_resultsFile.close();
    
    NS_LOG_INFO("\nResults saved!");
    
    Simulator::Destroy();
    return 0;
}
