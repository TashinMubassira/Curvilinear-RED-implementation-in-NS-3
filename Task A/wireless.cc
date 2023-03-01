/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Universita' di Firenze, Italy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Tommaso Pecorella <tommaso.pecorella@unifi.it>
 */


#include "ns3/core-module.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"

#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("RED Wireless");

int main (int argc, char** argv)
{
  bool verbose = false;
  double mxrange = 10;
  uint32_t nWifi = 10;
  uint32_t speed = 5;
  int maxPackets = 30;
  int pktSize = 64;
  double interval = 1.0;
  
  Packet::EnablePrinting ();

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("mxrange", "max range", mxrange);
  cmd.AddValue ("mxpackets", "max packet in each flo", maxPackets);
  cmd.AddValue ("packetsz", "sz of each packet", pktSize);
  cmd.AddValue ("interval", "interval in second of packets", interval);
  cmd.AddValue ("speed", "Speed of each node ", speed);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
      LogComponentEnable ("SixLowPanNetDevice", LOG_LEVEL_ALL);
    }

  uint32_t nWsnNodes = nWifi;
  NodeContainer wsnNodes;
  wsnNodes.Create (nWsnNodes);

  uint32_t wiredcnt = 1;
  NodeContainer wiredNodes;
  wiredNodes.Create (wiredcnt);
  wiredNodes.Add (wsnNodes.Get (0));
  
  uint32_t gwidth = (int)sqrt(nWsnNodes);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (2),
                                 "DeltaY", DoubleValue (2),
                                 "GridWidth", UintegerValue (gwidth),
                                 "LayoutType", StringValue ("RowFirst"));

  double x_max = gwidth*1.0;
  double y_max = ((nWsnNodes*1.0)/gwidth)*1.0;
  double z_max = 25.0;
  std::stringstream ssSpeed;
  ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << speed << "]";
  mobility.SetMobilityModel("ns3::GaussMarkovMobilityModel",
                                 "MeanVelocity", StringValue(ssSpeed.str()),
                                 "Bounds", BoxValue(Box(-10, x_max, -10, y_max, 0, z_max)),
                                 "TimeStep", TimeValue(Seconds(5)));
  mobility.Install (wsnNodes);

  LrWpanHelper lrWpanHelper;

  // creating a channel with range propagation loss model  
  Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (mxrange));
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<RangePropagationLossModel> propModel = CreateObject<RangePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);
  lrWpanHelper.SetChannel(channel);

  // Add and install the LrWpanNetDevice for each node
  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install (wsnNodes);

  // Fake PAN association and short address assignment.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.
  lrWpanHelper.AssociateToPan (lrwpanDevices, 0);
  
  //std::cout<< " internet stack er age " << std::endl;
  
  InternetStackHelper internetv6;
  internetv6.Install (wsnNodes);
  internetv6.Install (wiredNodes.Get(0));
  /*
  for(uint32_t i=0; i<wiredcnt; i++){
    internetv6.Install (wiredNodes.Get(i));  
  }
  */


  //std::cout<< " internet stack er age " << std::endl;
  
  SixLowPanHelper sixLowPanHelper;
  NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install (lrwpanDevices);

  CsmaHelper csmaHelper;
  NetDeviceContainer csmaDevices = csmaHelper.Install (wiredNodes);

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:cafe::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wiredDeviceInterfaces;
  wiredDeviceInterfaces = ipv6.Assign (csmaDevices);
  wiredDeviceInterfaces.SetForwarding (1, true);
  wiredDeviceInterfaces.SetDefaultRouteInAllNodes (1);

  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnDeviceInterfaces;
  wsnDeviceInterfaces = ipv6.Assign (sixLowPanDevices);
  wsnDeviceInterfaces.SetForwarding (0, true);
  wsnDeviceInterfaces.SetDefaultRouteInAllNodes (0);


  for (uint32_t i = 0; i < sixLowPanDevices.GetN (); i++)
    {
      Ptr<NetDevice> dev = sixLowPanDevices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }

  uint32_t packetSize = pktSize;
  uint32_t maxPacketCount = maxPackets;
  Time interPacketInterval = Seconds (interval);

  for(uint32_t i=0; i<nWsnNodes-2; i++){
    UdpEchoServerHelper echoServer (10);
    ApplicationContainer serverApps = echoServer.Install (wsnNodes.Get (i+1));   // dynamically choose a server node
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));
    
    Ipv6Address addr = wsnDeviceInterfaces.GetAddress (i+1, 1);
    std::cout << addr << std::endl;
    
    UdpEchoClientHelper echoClient (addr, 10);     // telling client about server app's (ip, port)
    echoClient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    echoClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
    echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

    ApplicationContainer clientApps = echoClient.Install (wiredNodes.Get(0) );  // dynamically choose a client node
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));
  }
  
  uint32_t SentPackets = 0;
  uint32_t ReceivedPackets = 0;
  uint32_t LostPackets = 0;
    int j=0;
    float AvgThroughput = 0;
    Time Jitter;
    Time Delay;

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  Simulator::Stop (Seconds (10));
  
  cout << "Running the simulation" << std::endl;
  Simulator::Run ();
  
  Ptr<Ipv6FlowClassifier> classifier = DynamicCast<Ipv6FlowClassifier> (flowmon.GetClassifier6() );
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

  for(auto iter = stats.begin(); iter != stats.end(); ++iter){
     Ipv6FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

	 
	  NS_LOG_UNCOND("__Flow ID: " << iter->first);
	  NS_LOG_UNCOND("src addr: " << t.sourceAddress << "-- dest addr: " << t.destinationAddress);
	  NS_LOG_UNCOND("Sent Packets=" <<iter->second. txPackets);
	  NS_LOG_UNCOND ("Received Patkets =" <<iter->second. rxPackets);
	  NS_LOG_UNCOND ("Lost Packets=" <<iter->second. txPackets-iter->second. rxPackets);
	  NS_LOG_UNCOND("Packet delivery ratio = " <<iter->second.rxPackets*100.0/iter->second.txPackets << "%");
	  NS_LOG_UNCOND ("Packet loss ratio =" << (iter->second.txPackets-iter->second.rxPackets)*100.0/iter->second. txPackets << "%");
	  NS_LOG_UNCOND ("Delay =" <<iter->second.delaySum);
	  NS_LOG_UNCOND ("Jitter =" <<iter->second. jitterSum);
	  NS_LOG_UNCOND ("Throughput =" <<iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()));
	  
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

    ofstream outnodes;
    outnodes.open("Outputs/nWifiData.txt",ios::app);
    ofstream outspeed;
    outspeed.open("Outputs/speedData.txt", ios::app);
    ofstream outrate;
    outrate.open("Outputs/WirelessDataRate.txt", ios::app);

    outnodes << nWifi << " " << AvgThroughput << " " << Delay.GetNanoSeconds() << " " << ((ReceivedPackets*100)/SentPackets) << " "<< ((LostPackets*100)/SentPackets) << '\n';
    outrate << interval << " " << AvgThroughput << " " << Delay.GetNanoSeconds() << " " << ((ReceivedPackets*100)/SentPackets) << " "<< ((LostPackets*100)/SentPackets) << '\n';
    outspeed << speed << " " << AvgThroughput << " " << Delay.GetNanoSeconds() << " " << ((ReceivedPackets*100)/SentPackets) << " "<< ((LostPackets*100)/SentPackets) << '\n';
   
  
  cout << "Destroying the simulation" << std::endl;

    Simulator::Destroy ();
    outnodes.close();
    outrate.close();
    outspeed.close();

    cout << "Done" << endl;
    return 0;

}

