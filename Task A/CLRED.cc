/** Network topology
 *
 *    10Mb/s, 1ms  
 * n0--------------|
 *                 |   1Mbps/s, 50ms     |  10Mbp/s,1ms 
 *                 n3------------------n4|-------------n5
 *    10Mb/s, 1ms  |
 * n1--------------|
 *                 |
 *    10Mb/s, 1 ms |
 * n2--------------|
 
 *  
 *
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/point-to-point-layout-module.h"

#include <iostream>
#include <iomanip>
#include <map>

using namespace ns3;
using namespace std;
using namespace Config;

string dir = "./scratch";
uint32_t previous = 0;
Time prevTime = Seconds (0);



NS_LOG_COMPONENT_DEFINE ("CLRED_TEST");


int main(int argc,char *argv[]){
    uint32_t    nLeaf = 10;
    uint32_t    maxPackets = 100;
    bool        modeBytes  = false;
    bool        tracing = false;
    uint32_t    queueDiscLimitPackets = 1000;
    double      minTh = 10;
    double      maxTh = 30;
    double      minTh2 = 20; 
    uint32_t    pktSize = 512;

    string appDataRate = "10Mbps";
    string appDelay = "1ms";

    string queueDiscType = "CLRED";
    uint16_t port = 5001;

    string bottleNeckLinkBw = "10Mbps";
    string bottleNeckLinkDelay = "10ms";
    Time stopTime = Seconds (100);


    CommandLine cmd (__FILE__);

    cmd.AddValue ("nLeaf",     "Number of left side leaf nodes", nLeaf);

    cmd.AddValue ("maxPackets","Max Packets allowed in the device queue", maxPackets);
    cmd.AddValue ("queueDiscLimitPackets","Max Packets allowed in the queue disc", queueDiscLimitPackets);

    cmd.AddValue ("queueDiscType", "Set Queue disc type to RED or ARED", queueDiscType);
    cmd.AddValue ("appPktSize", "Set OnOff App Packet Size", pktSize);

    cmd.AddValue ("appDataRate", "Set OnOff App DataRate", appDataRate);
    cmd.AddValue ("appDelay", "Set OnOff App Delay", appDelay);

    cmd.AddValue ("bottleNeckLinkBw", "Set OnOff bottleneck link bandwidth", bottleNeckLinkBw);
    cmd.AddValue ("bottleNeckLinkDelay", "Set OnOff bottleneck link Delay", bottleNeckLinkDelay);

    cmd.AddValue ("modeBytes", "Set Queue disc mode to Packets (false) or bytes (true)", modeBytes);
    cmd.AddValue("tracing", "<true/false>Enables tracing", tracing);
    cmd.AddValue ("redMinTh", "RED queue minimum threshold 1", minTh);
    cmd.AddValue ("redMaxTh", "RED queue maximum threshold", maxTh);
    cmd.AddValue ("redMinTh2", "RED queue minimum threshold 2", minTh2);
    cmd.Parse (argc,argv);

  if ((queueDiscType != "RED") && (queueDiscType != "CLRED"))
    {
      cout << "Invalid queue disc type: Use --queueDiscType=RED or --queueDiscType=CLRED" << std::endl;
      exit (1);
    }

    if(queueDiscType == "CLRED"){
        SetDefault ("ns3::RedQueueDisc::MinTh2", DoubleValue (minTh2));
        SetDefault ("ns3::RedQueueDisc::CLRED",BooleanValue(true));
    }

   SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (pktSize));
   SetDefault ("ns3::OnOffApplication::DataRate", StringValue (appDataRate));
   SetDefault ("ns3::DropTailQueue<Packet>::MaxSize", StringValue (to_string (maxPackets) + "p"));

  //setting RedQueueDisc
   if (!modeBytes)
    {
      SetDefault ("ns3::RedQueueDisc::MaxSize",
                          QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, queueDiscLimitPackets)));
    }
    else
      {
        SetDefault ("ns3::RedQueueDisc::MaxSize",
                            QueueSizeValue (QueueSize (QueueSizeUnit::BYTES, queueDiscLimitPackets * pktSize)));
        minTh *= pktSize;
        maxTh *= pktSize;
      }

    SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (minTh));
    SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (maxTh));    

    SetDefault ("ns3::RedQueueDisc::LinkBandwidth", StringValue (bottleNeckLinkBw));
    SetDefault ("ns3::RedQueueDisc::LinkDelay", StringValue (bottleNeckLinkDelay));

    SetDefault ("ns3::RedQueueDisc::MeanPktSize", UintegerValue (pktSize));
    SetDefault ("ns3::RedQueueDisc::Wait", BooleanValue (true));
    SetDefault ("ns3::RedQueueDisc::Gentle", BooleanValue (true));
    SetDefault ("ns3::RedQueueDisc::QW", DoubleValue (0.002));


    //creating my topology
    PointToPointHelper bottleNeckLink;
    bottleNeckLink.SetDeviceAttribute  ("DataRate", StringValue (bottleNeckLinkBw));
    bottleNeckLink.SetChannelAttribute ("Delay", StringValue (bottleNeckLinkDelay));

    PointToPointHelper pointToPointLeaf;
    pointToPointLeaf.SetDeviceAttribute    ("DataRate", StringValue (appDataRate));
    pointToPointLeaf.SetChannelAttribute   ("Delay", StringValue (appDelay));

    PointToPointDumbbellHelper dumbbel (nLeaf, pointToPointLeaf,
                                  1, pointToPointLeaf,
                                  bottleNeckLink);

    // Install Stack
    InternetStackHelper stack;
    for (uint32_t i = 0; i < dumbbel.LeftCount(); ++i)
      {
        stack.Install (dumbbel.GetLeft(i));
      }
    for (uint32_t i = 0; i < dumbbel.RightCount(); ++i)
      {
        stack.Install (dumbbel.GetRight(i));
      }
    stack.Install (dumbbel.GetLeft());
    stack.Install (dumbbel.GetRight());

    //installing in trafficControlHelper
    TrafficControlHelper tchBottleneck;
    QueueDiscContainer queueDiscs;
    tchBottleneck.SetRootQueueDisc ("ns3::RedQueueDisc");
    tchBottleneck.Install (dumbbel.GetRight()->GetDevice(0));
    queueDiscs = tchBottleneck.Install (dumbbel.GetLeft()->GetDevice(0));

    //Assigning IP addresses
    dumbbel.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),   //left
                                Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),    //right
                                Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));   //bottleneck

                                
    Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny(), port));
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApps;
    for (uint32_t i = 0; i < dumbbel.RightCount(); ++i)
      {
        sinkApps.Add (packetSinkHelper.Install (dumbbel.GetRight(i)));
      }
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (30.0));
    

    OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
    clientHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable[Min=0.|Max=1.]"));
    clientHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable[Min=0.|Max=1.]"));

    ApplicationContainer clientApps;
    for (uint32_t i = 0; i < dumbbel.LeftCount (); ++i)
      {
        // Create an on/off app sending packets to the left side
        AddressValue remoteAddress (InetSocketAddress (dumbbel.GetRightIpv4Address (0), port));
        clientHelper.SetAttribute ("Remote", remoteAddress);
        clientApps.Add (clientHelper.Install (dumbbel.GetLeft (i)));
      }
    clientApps.Start (Seconds (1.0)); // Start 1 second after sink
    clientApps.Stop (Seconds (15.0)); // Stop before the sink

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint32_t SentPackets = 0;
    uint32_t ReceivedPackets = 0;
    uint32_t LostPackets = 0;

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
    // Simulator::Schedule (Seconds (0 + 0.000001), &TraceThroughput, monitor);

    Simulator::Stop (stopTime + TimeStep (1));

    cout << "Running the simulation" << std::endl;
    Simulator::Run ();

    int j=0;
    float AvgThroughput = 0;
    Time Jitter;
    Time Delay;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
      std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
        {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

    NS_LOG_UNCOND("----Flow ID:" <<iter->first);
    NS_LOG_UNCOND("Src Addr" <<t.sourceAddress << "Dst Addr "<< t.destinationAddress);
    NS_LOG_UNCOND("Sent Packets=" <<iter->second.txPackets);
    NS_LOG_UNCOND("Received Packets =" <<iter->second.rxPackets);
    NS_LOG_UNCOND("Lost Packets =" <<iter->second.txPackets-iter->second.rxPackets);
    NS_LOG_UNCOND("Packet delivery ratio =" <<iter->second.rxPackets*100/iter->second.txPackets << "%");
    NS_LOG_UNCOND("Packet loss ratio =" << (iter->second.txPackets-iter->second.rxPackets)*100/iter->second.txPackets << "%");
    NS_LOG_UNCOND("Delay =" <<iter->second.delaySum);
    NS_LOG_UNCOND("Jitter =" <<iter->second.jitterSum);
    NS_LOG_UNCOND("Throughput =" <<iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024<<"Kbps");

    SentPackets = SentPackets +(iter->second.txPackets);
    ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
    LostPackets = LostPackets + (iter->second.txPackets-iter->second.rxPackets);
    AvgThroughput = AvgThroughput + (iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024);
    Delay = Delay + (iter->second.delaySum);
    Jitter = Jitter + (iter->second.jitterSum);

    j = j + 1;

    }

    AvgThroughput = AvgThroughput/j;
    NS_LOG_UNCOND("--------Total Results of the simulation----------"<<std::endl);
    NS_LOG_UNCOND("Total sent packets  =" << SentPackets);
    NS_LOG_UNCOND("Total Received Packets =" << ReceivedPackets);
    NS_LOG_UNCOND("Total Lost Packets =" << LostPackets);
    NS_LOG_UNCOND("Packet Loss ratio =" << ((LostPackets*100)/SentPackets)<< "%");
    NS_LOG_UNCOND("Packet delivery ratio =" << ((ReceivedPackets*100)/SentPackets)<< "%");
    NS_LOG_UNCOND("Average Throughput =" << AvgThroughput<< "Kbps");
    NS_LOG_UNCOND("End to End Delay =" << Delay);
    NS_LOG_UNCOND("End to End Jitter delay =" << Jitter);
    NS_LOG_UNCOND("Total Flod id " << j);
    monitor->SerializeToXmlFile("manet-routing.xml", true, true);

    // QueueDisc::Stats st = queueDiscs.Get (0)->GetStats ();

    // cout << "*** Stats from the bottleneck queue disc to left side routers" << std::endl;
    // cout << st << std::endl;

    ofstream output;
    output.open("Outputs/CLREDDataRate.txt",ios::app);
    for(int i = 0; i<4; i++){
      appDataRate.pop_back();
    }

    output << appDataRate << " " << AvgThroughput << " " << Delay.GetNanoSeconds() << " " << ((ReceivedPackets*100)/SentPackets) << " "<< ((LostPackets*100)/SentPackets) << '\n';

    cout << "Destroying the simulation" << std::endl;

    Simulator::Destroy ();
    output.close();

    cout << "Done" << endl;
    return 0;

}
