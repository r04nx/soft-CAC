/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * WiFi 6 Call Admission Control Simulation
 * Dense WLAN Environment with Heterogeneous Traffic
 * 
 * This simulation implements airtime-based CAC for WiFi 6 networks
 * with multiple traffic types and variable client density.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "wifi6-cac-airtime.h"

#include <fstream>
#include <vector>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wifi6CacSimulation");

// Global variables for statistics
std::map<uint32_t, Time> g_txTimeMap;  // Track packet transmission times
std::map<uint32_t, uint32_t> g_flowIdMap;  // Map packet UID to flow ID
Ptr<AirtimeAdmissionControl> g_cac;
std::ofstream g_throughputFile;
std::ofstream g_delayFile;
std::ofstream g_admissionFile;

// Packet tagging for flow identification
class FlowIdTag : public Tag
{
public:
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual void Print(std::ostream &os) const;
    
    void SetFlowId(uint32_t flowId) { m_flowId = flowId; }
    uint32_t GetFlowId(void) const { return m_flowId; }
    
    void SetTrafficType(TrafficType type) { m_type = type; }
    TrafficType GetTrafficType(void) const { return m_type; }

private:
    uint32_t m_flowId;
    TrafficType m_type;
};

TypeId
FlowIdTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::FlowIdTag")
        .SetParent<Tag>()
        .AddConstructor<FlowIdTag>();
    return tid;
}

TypeId
FlowIdTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t
FlowIdTag::GetSerializedSize(void) const
{
    return 8;  // 4 bytes for flowId + 4 bytes for type
}

void
FlowIdTag::Serialize(TagBuffer i) const
{
    i.WriteU32(m_flowId);
    i.WriteU32(static_cast<uint32_t>(m_type));
}

void
FlowIdTag::Deserialize(TagBuffer i)
{
    m_flowId = i.ReadU32();
    m_type = static_cast<TrafficType>(i.ReadU32());
}

void
FlowIdTag::Print(std::ostream &os) const
{
    os << "FlowId=" << m_flowId << " Type=" << m_type;
}

// Packet transmission callback
void
TxCallback(Ptr<const Packet> packet)
{
    FlowIdTag tag;
    if (packet->PeekPacketTag(tag)) {
        g_txTimeMap[packet->GetUid()] = Simulator::Now();
        g_flowIdMap[packet->GetUid()] = tag.GetFlowId();
    }
}

// Packet reception callback
void
RxCallback(Ptr<const Packet> packet, const Address &address)
{
    FlowIdTag tag;
    if (packet->PeekPacketTag(tag)) {
        uint32_t uid = packet->GetUid();
        auto it = g_txTimeMap.find(uid);
        
        if (it != g_txTimeMap.end()) {
            Time delay = Simulator::Now() - it->second;
            double delayMs = delay.GetMilliSeconds();
            
            // Update CAC statistics
            g_cac->UpdateFlowStats(tag.GetFlowId(), packet->GetSize(), delay.GetSeconds());
            
            // Log delay for analysis
            g_delayFile << Simulator::Now().GetSeconds() << ","
                       << tag.GetFlowId() << ","
                       << tag.GetTrafficType() << ","
                       << delayMs << "\n";
            
            g_txTimeMap.erase(it);
        }
    }
}

// VoIP traffic generator (G.711 codec)
class VoipApplication : public Application
{
public:
    VoipApplication();
    virtual ~VoipApplication();
    
    void Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, 
               uint32_t nPackets, DataRate dataRate, uint32_t flowId);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    
    void ScheduleTx(void);
    void SendPacket(void);
    
    Ptr<Socket> m_socket;
    Address m_peer;
    uint32_t m_packetSize;
    uint32_t m_nPackets;
    DataRate m_dataRate;
    EventId m_sendEvent;
    bool m_running;
    uint32_t m_packetsSent;
    uint32_t m_flowId;
};

VoipApplication::VoipApplication()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_running(false),
      m_packetsSent(0),
      m_flowId(0)
{
}

VoipApplication::~VoipApplication()
{
    m_socket = 0;
}

void
VoipApplication::Setup(Ptr<Socket> socket, Address address, uint32_t packetSize,
                       uint32_t nPackets, DataRate dataRate, uint32_t flowId)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_flowId = flowId;
}

void
VoipApplication::StartApplication(void)
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
VoipApplication::StopApplication(void)
{
    m_running = false;
    
    if (m_sendEvent.IsRunning()) {
        Simulator::Cancel(m_sendEvent);
    }
    
    if (m_socket) {
        m_socket->Close();
    }
}

void
VoipApplication::SendPacket(void)
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    
    // Add flow ID tag
    FlowIdTag tag;
    tag.SetFlowId(m_flowId);
    tag.SetTrafficType(VOIP);
    packet->AddPacketTag(tag);
    
    m_socket->Send(packet);
    TxCallback(packet);
    
    if (++m_packetsSent < m_nPackets) {
        ScheduleTx();
    }
}

void
VoipApplication::ScheduleTx(void)
{
    if (m_running) {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &VoipApplication::SendPacket, this);
    }
}

// Video streaming application (CBR with variations)
class VideoStreamApplication : public Application
{
public:
    VideoStreamApplication();
    virtual ~VideoStreamApplication();
    
    void Setup(Ptr<Socket> socket, Address address, uint32_t basePacketSize,
               DataRate baseDataRate, uint32_t flowId);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    
    void SendPacket(void);
    
    Ptr<Socket> m_socket;
    Address m_peer;
    uint32_t m_basePacketSize;
    DataRate m_baseDataRate;
    EventId m_sendEvent;
    bool m_running;
    uint32_t m_flowId;
    Ptr<UniformRandomVariable> m_rateVariation;
};

VideoStreamApplication::VideoStreamApplication()
    : m_socket(0),
      m_peer(),
      m_basePacketSize(0),
      m_baseDataRate(0),
      m_running(false),
      m_flowId(0)
{
    m_rateVariation = CreateObject<UniformRandomVariable>();
    m_rateVariation->SetAttribute("Min", DoubleValue(0.8));
    m_rateVariation->SetAttribute("Max", DoubleValue(1.2));
}

VideoStreamApplication::~VideoStreamApplication()
{
    m_socket = 0;
}

void
VideoStreamApplication::Setup(Ptr<Socket> socket, Address address,
                              uint32_t basePacketSize, DataRate baseDataRate, uint32_t flowId)
{
    m_socket = socket;
    m_peer = address;
    m_basePacketSize = basePacketSize;
    m_baseDataRate = baseDataRate;
    m_flowId = flowId;
}

void
VideoStreamApplication::StartApplication(void)
{
    m_running = true;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
VideoStreamApplication::StopApplication(void)
{
    m_running = false;
    
    if (m_sendEvent.IsRunning()) {
        Simulator::Cancel(m_sendEvent);
    }
    
    if (m_socket) {
        m_socket->Close();
    }
}

void
VideoStreamApplication::SendPacket(void)
{
    // Vary packet size slightly to simulate video encoding variations
    double variation = m_rateVariation->GetValue();
    uint32_t packetSize = static_cast<uint32_t>(m_basePacketSize * variation);
    
    Ptr<Packet> packet = Create<Packet>(packetSize);
    
    FlowIdTag tag;
    tag.SetFlowId(m_flowId);
    tag.SetTrafficType(VIDEO_STREAM);
    packet->AddPacketTag(tag);
    
    m_socket->Send(packet);
    TxCallback(packet);
    
    if (m_running) {
        Time tNext(Seconds(packetSize * 8 / static_cast<double>(m_baseDataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &VideoStreamApplication::SendPacket, this);
    }
}

int
main(int argc, char *argv[])
{
    // Simulation parameters
    uint32_t nStations = 30;           // Number of WiFi stations (25-50)
    uint32_t nVoipFlows = 10;          // Number of VoIP flows
    uint32_t nVideoFlows = 8;          // Number of video streaming flows
    uint32_t nBurstyFlows = 6;         // Number of bursty flows
    uint32_t nWebFlows = 6;            // Number of web browsing flows
    double simulationTime = 60.0;      // Simulation time in seconds
    double airtimeThreshold = 0.80;    // CAC threshold (80%)
    bool enableCac = true;             // Enable/disable CAC
    uint32_t channelWidth = 80;        // Channel width in MHz
    std::string outputPrefix = "wifi6-cac";
    
    // Command line arguments
    CommandLine cmd;
    cmd.AddValue("nStations", "Number of WiFi stations", nStations);
    cmd.AddValue("nVoipFlows", "Number of VoIP flows", nVoipFlows);
    cmd.AddValue("nVideoFlows", "Number of video flows", nVideoFlows);
    cmd.AddValue("nBurstyFlows", "Number of bursty flows", nBurstyFlows);
    cmd.AddValue("nWebFlows", "Number of web flows", nWebFlows);
    cmd.AddValue("simTime", "Simulation time (seconds)", simulationTime);
    cmd.AddValue("threshold", "Airtime threshold for CAC", airtimeThreshold);
    cmd.AddValue("enableCac", "Enable CAC (1=yes, 0=no)", enableCac);
    cmd.AddValue("channelWidth", "Channel width (20/40/80/160 MHz)", channelWidth);
    cmd.AddValue("outputPrefix", "Output file prefix", outputPrefix);
    cmd.Parse(argc, argv);
    
    // Enable logging
    LogComponentEnable("Wifi6CacSimulation", LOG_LEVEL_INFO);
    
    NS_LOG_INFO("=== WiFi 6 CAC Simulation ===");
    NS_LOG_INFO("Stations: " << nStations);
    NS_LOG_INFO("VoIP flows: " << nVoipFlows);
    NS_LOG_INFO("Video flows: " << nVideoFlows);
    NS_LOG_INFO("Bursty flows: " << nBurstyFlows);
    NS_LOG_INFO("Web flows: " << nWebFlows);
    NS_LOG_INFO("CAC enabled: " << (enableCac ? "YES" : "NO"));
    NS_LOG_INFO("Airtime threshold: " << airtimeThreshold);
    
    // Open output files
    g_throughputFile.open(outputPrefix + "-throughput.csv");
    g_delayFile.open(outputPrefix + "-delay.csv");
    g_admissionFile.open(outputPrefix + "-admission.csv");
    
    g_throughputFile << "Time,FlowId,TrafficType,Throughput\n";
    g_delayFile << "Time,FlowId,TrafficType,Delay\n";
    g_admissionFile << "FlowId,TrafficType,Admitted,RequiredAirtime\n";
    
    // Create CAC controller
    g_cac = CreateObject<AirtimeAdmissionControl>();
    g_cac->SetAirtimeThreshold(airtimeThreshold);
    g_cac->SetWifiPhyParameters(channelWidth, 800, 2);  // 80 MHz, 800ns GI, 2 SS
    
    // Create nodes
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nStations);
    NodeContainer wifiApNode;
    wifiApNode.Create(1);
    
    // Create WiFi 6 (802.11ax) channel
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
                               "Exponent", DoubleValue(3.0),
                               "ReferenceDistance", DoubleValue(1.0));
    
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    // Configure WiFi 6 parameters
    phy.Set("ChannelSettings", StringValue("{0, " + std::to_string(channelWidth) + ", BAND_5GHZ, 0}"));
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                  "DataMode", StringValue("HeMcs5"),
                                  "ControlMode", StringValue("HeMcs0"));
    
    WifiMacHelper mac;
    Ssid ssid = Ssid("wifi6-cac-network");
    
    // Configure QoS
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "QosSupported", BooleanValue(true));
    
    NetDeviceContainer staDevices = wifi.Install(phy, mac, wifiStaNodes);
    
    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid),
                "QosSupported", BooleanValue(true));
    
    NetDeviceContainer apDevice = wifi.Install(phy, mac, wifiApNode);
    
    // Mobility model - dense deployment
    MobilityHelper mobility;
    
    // AP at center
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(5.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    
    // Stations in dense grid around AP
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                  "X", DoubleValue(0.0),
                                  "Y", DoubleValue(0.0),
                                  "Rho", StringValue("ns3::UniformRandomVariable[Min=1.0|Max=15.0]"));
    
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);
    
    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);
    
    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
    Ipv4InterfaceContainer apInterface = address.Assign(apDevice);
    
    NS_LOG_INFO("Network setup complete. Starting traffic generation...");
    
    // Create packet sinks on AP for receiving traffic
    uint16_t port = 9;
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sinkHelper.Install(wifiApNode);
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(simulationTime));
    
    // Connect RX callback
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
    sink->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallback));
    
    // Traffic generation with CAC admission control
    uint32_t flowCounter = 0;
    ApplicationContainer clientApps;
    
    // VoIP Traffic (G.711: 64 kbps, 160 bytes every 20ms)
    NS_LOG_INFO("Generating VoIP flows...");
    for (uint32_t i = 0; i < nVoipFlows && i < nStations; i++) {
        FlowDescriptor flow;
        flow.type = VOIP;
        flow.packetSize = 160;  // G.711 codec
        flow.dataRate = 64000;  // 64 kbps
        flow.source = Mac48Address::ConvertFrom(staDevices.Get(i)->GetAddress());
        flow.destination = Mac48Address::ConvertFrom(apDevice.Get(0)->GetAddress());
        
        bool admitted = true;
        if (enableCac) {
            admitted = g_cac->RequestAdmission(flow);
        } else {
            flow.flowId = ++flowCounter;
            flow.admitted = true;
        }
        
        g_admissionFile << flow.flowId << ",VOIP," << admitted << "," 
                       << flow.requiredAirtime << "\n";
        
        if (admitted) {
            Ptr<Socket> socket = Socket::CreateSocket(wifiStaNodes.Get(i), UdpSocketFactory::GetTypeId());
            Ptr<VoipApplication> app = CreateObject<VoipApplication>();
            app->Setup(socket, InetSocketAddress(apInterface.GetAddress(0), port),
                      160, 3000, DataRate("64kbps"), flow.flowId);
            wifiStaNodes.Get(i)->AddApplication(app);
            app->SetStartTime(Seconds(1.0 + i * 0.1));
            app->SetStopTime(Seconds(simulationTime));
            clientApps.Add(app);
        }
    }
    
    // Video Streaming Traffic (2-5 Mbps, 1200 bytes packets)
    NS_LOG_INFO("Generating video streaming flows...");
    for (uint32_t i = 0; i < nVideoFlows && (nVoipFlows + i) < nStations; i++) {
        uint32_t nodeIdx = nVoipFlows + i;
        
        FlowDescriptor flow;
        flow.type = VIDEO_STREAM;
        flow.packetSize = 1200;
        flow.dataRate = 3000000;  // 3 Mbps average
        flow.source = Mac48Address::ConvertFrom(staDevices.Get(nodeIdx)->GetAddress());
        flow.destination = Mac48Address::ConvertFrom(apDevice.Get(0)->GetAddress());
        
        bool admitted = true;
        if (enableCac) {
            admitted = g_cac->RequestAdmission(flow);
        } else {
            flow.flowId = ++flowCounter;
            flow.admitted = true;
        }
        
        g_admissionFile << flow.flowId << ",VIDEO," << admitted << "," 
                       << flow.requiredAirtime << "\n";
        
        if (admitted) {
            Ptr<Socket> socket = Socket::CreateSocket(wifiStaNodes.Get(nodeIdx), UdpSocketFactory::GetTypeId());
            Ptr<VideoStreamApplication> app = CreateObject<VideoStreamApplication>();
            app->Setup(socket, InetSocketAddress(apInterface.GetAddress(0), port),
                      1200, DataRate("3Mbps"), flow.flowId);
            wifiStaNodes.Get(nodeIdx)->AddApplication(app);
            app->SetStartTime(Seconds(2.0 + i * 0.2));
            app->SetStopTime(Seconds(simulationTime));
            clientApps.Add(app);
        }
    }
    
    // Bursty Traffic (On-Off application with exponential distribution)
    NS_LOG_INFO("Generating bursty flows...");
    for (uint32_t i = 0; i < nBurstyFlows && (nVoipFlows + nVideoFlows + i) < nStations; i++) {
        uint32_t nodeIdx = nVoipFlows + nVideoFlows + i;
        
        FlowDescriptor flow;
        flow.type = BURSTY;
        flow.packetSize = 1400;
        flow.dataRate = 5000000;  // 5 Mbps during ON period
        flow.source = Mac48Address::ConvertFrom(staDevices.Get(nodeIdx)->GetAddress());
        flow.destination = Mac48Address::ConvertFrom(apDevice.Get(0)->GetAddress());
        
        bool admitted = true;
        if (enableCac) {
            // For bursty traffic, use average rate (50% duty cycle)
            flow.dataRate = 2500000;  // Average 2.5 Mbps
            admitted = g_cac->RequestAdmission(flow);
            flow.dataRate = 5000000;  // Restore peak rate
        } else {
            flow.flowId = ++flowCounter;
            flow.admitted = true;
        }
        
        g_admissionFile << flow.flowId << ",BURSTY," << admitted << "," 
                       << flow.requiredAirtime << "\n";
        
        if (admitted) {
            OnOffHelper onoff("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), port));
            onoff.SetAttribute("PacketSize", UintegerValue(1400));
            onoff.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps")));
            onoff.SetAttribute("OnTime", StringValue("ns3::ExponentialRandomVariable[Mean=1.0]"));
            onoff.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=1.0]"));
            
            ApplicationContainer app = onoff.Install(wifiStaNodes.Get(nodeIdx));
            app.Start(Seconds(3.0 + i * 0.2));
            app.Stop(Seconds(simulationTime));
            clientApps.Add(app);
        }
    }
    
    // Web Browsing Traffic (HTTP-like with Pareto distribution)
    NS_LOG_INFO("Generating web browsing flows...");
    for (uint32_t i = 0; i < nWebFlows && (nVoipFlows + nVideoFlows + nBurstyFlows + i) < nStations; i++) {
        uint32_t nodeIdx = nVoipFlows + nVideoFlows + nBurstyFlows + i;
        
        FlowDescriptor flow;
        flow.type = WEB_BROWSING;
        flow.packetSize = 1000;
        flow.dataRate = 1000000;  // 1 Mbps average
        flow.source = Mac48Address::ConvertFrom(staDevices.Get(nodeIdx)->GetAddress());
        flow.destination = Mac48Address::ConvertFrom(apDevice.Get(0)->GetAddress());
        
        bool admitted = true;
        if (enableCac) {
            admitted = g_cac->RequestAdmission(flow);
        } else {
            flow.flowId = ++flowCounter;
            flow.admitted = true;
        }
        
        g_admissionFile << flow.flowId << ",WEB," << admitted << "," 
                       << flow.requiredAirtime << "\n";
        
        if (admitted) {
            OnOffHelper onoff("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), port));
            onoff.SetAttribute("PacketSize", UintegerValue(1000));
            onoff.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
            onoff.SetAttribute("OnTime", StringValue("ns3::ParetoRandomVariable[Mean=0.5|Shape=1.5]"));
            onoff.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=2.0]"));
            
            ApplicationContainer app = onoff.Install(wifiStaNodes.Get(nodeIdx));
            app.Start(Seconds(4.0 + i * 0.2));
            app.Stop(Seconds(simulationTime));
            clientApps.Add(app);
        }
    }
    
    NS_LOG_INFO("Traffic generation complete.");
    NS_LOG_INFO("Admitted flows: " << g_cac->GetAdmittedFlowCount());
    NS_LOG_INFO("Blocked flows: " << g_cac->GetBlockedFlowCount());
    NS_LOG_INFO("Blocking probability: " << g_cac->GetBlockingProbability());
    
    // Install FlowMonitor for additional metrics
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    // Run simulation
    NS_LOG_INFO("Starting simulation for " << simulationTime << " seconds...");
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
    // Print final statistics
    NS_LOG_INFO("\n=== Simulation Complete ===");
    g_cac->PrintStatistics(std::cout);
    
    // FlowMonitor statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    
    std::ofstream flowmonFile(outputPrefix + "-flowmon.csv");
    flowmonFile << "FlowId,TxPackets,RxPackets,TxBytes,RxBytes,Throughput,MeanDelay,PacketLoss\n";
    
    for (auto const &flow : stats) {
        double throughput = flow.second.rxBytes * 8.0 / simulationTime / 1e6;  // Mbps
        double meanDelay = 0.0;
        if (flow.second.rxPackets > 0) {
            meanDelay = flow.second.delaySum.GetSeconds() / flow.second.rxPackets * 1000;  // ms
        }
        double packetLoss = 0.0;
        if (flow.second.txPackets > 0) {
            packetLoss = (flow.second.txPackets - flow.second.rxPackets) * 100.0 / flow.second.txPackets;
        }
        
        flowmonFile << flow.first << ","
                   << flow.second.txPackets << ","
                   << flow.second.rxPackets << ","
                   << flow.second.txBytes << ","
                   << flow.second.rxBytes << ","
                   << throughput << ","
                   << meanDelay << ","
                   << packetLoss << "\n";
    }
    flowmonFile.close();
    
    // Close output files
    g_throughputFile.close();
    g_delayFile.close();
    g_admissionFile.close();
    
    NS_LOG_INFO("Results saved to " << outputPrefix << "-*.csv files");
    
    Simulator::Destroy();
    return 0;
}
