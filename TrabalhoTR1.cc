/************************************************************************************************
  Teleinformática e Redes 1 - Trabalho 2
  
  Alunos: André Luis Souto Ferreira 140016261
          Gustavo Henrique -- Abraço pra tu Gustavao !
          Victor Fabre
  
************************************************************************************************/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

/****TODO : DESENHAR TOPOLOGIA CORRETA ABAIXO *****/
// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TrabalhoFinalScript");

int main (int argc, char *argv[])
{

  bool verbose = true;
  
  /* 10 clientes em cada rede Ethernet */
  uint32_t nCsma = 10;

  /* 10 clientes em cada rede WiFi */
  uint32_t nWifi = 10;

  // CommandLine cmd;
  // cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  // cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  // cmd.Parse (argc,argv);

  // if (verbose)
  //   {
  //     LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //     LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  //   }

  // nCsma = nCsma == 0 ? 1 : nCsma;


  /* Ethernet 1 */
  NodeContainer csmaNodes1;
  csmaNodes1.Create (nCsma);

  CsmaHelper csma1;
  csma1.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma1.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices1;
  csmaDevices1 = csma1.Install (csmaNodes1);
  /**************/


  /* Ethernet 2*/
  NodeContainer csmaNodes2;
  csmaNodes2.Create (nCsma);

  CsmaHelper csma2;
  csma2.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices2;
  csmaDevices2 = csma2.Install (csmaNodes2);
  /**************/


  /* WiFi 1 */
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi);
  NodeContainer wifiApNode1 = csmaNodes1.Get (0);

  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  WifiHelper wifi1;
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac1;
  Ssid ssid1 = Ssid ("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1, wifiStaNodes1);

  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifiApNode1);

  MobilityHelper mobility1;

  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility1.Install (wifiStaNodes1);

  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifiApNode1);
  /**********/


  /* WiFi 2 */
    NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi);
  NodeContainer wifiApNode2 = csmaNodes2.Get (0);

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2;
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac2;
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices2;
  staDevices2 = wifi1.Install (phy2, mac2, wifiStaNodes2);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);

  MobilityHelper mobility2;

  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility2.Install (wifiStaNodes2);

  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiApNode2);
  /**********/


  /* WiFi 3 */
    NodeContainer wifiStaNodes3;
  wifiStaNodes3.Create (nWifi);
  NodeContainer wifiApNode3 = csmaNodes1.Get (1);

  YansWifiChannelHelper channel3 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy3 = YansWifiPhyHelper::Default ();
  phy3.SetChannel (channel3.Create ());

  WifiHelper wifi3;
  wifi3.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac3;
  Ssid ssid3 = Ssid ("ns-3-ssid");
  mac3.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid3),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices3;
  staDevices3 = wifi3.Install (phy3, mac3, wifiStaNodes3);

  mac3.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid3));

  NetDeviceContainer apDevices3;
  apDevices3 = wifi3.Install (phy3, mac3, wifiApNode3);

  MobilityHelper mobility3;

  mobility3.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility3.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility3.Install (wifiStaNodes3);

  mobility3.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility3.Install (wifiApNode3);
  /**********/


  /* WiFi 4 */
    NodeContainer wifiStaNodes4;
  wifiStaNodes4.Create (nWifi);
  NodeContainer wifiApNode4 = csmaNodes2.Get (1);

  YansWifiChannelHelper channel4 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy4 = YansWifiPhyHelper::Default ();
  phy4.SetChannel (channel4.Create ());

  WifiHelper wifi4;
  wifi4.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac4;
  Ssid ssid4 = Ssid ("ns-3-ssid");
  mac4.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid4),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices4;
  staDevices4 = wifi4.Install (phy4, mac4, wifiStaNodes4);

  mac4.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid4));

  NetDeviceContainer apDevices4;
  apDevices4 = wifi4.Install (phy4, mac4, wifiApNode4);

  MobilityHelper mobility4;

  mobility4.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility4.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility4.Install (wifiStaNodes4);

  mobility4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility4.Install (wifiApNode4);
  /**********/


  InternetStackHelper stack;
  stack.Install (csmaNodes1);
  stack.Install (csmaNodes2);
  stack.Install (wifiApNode1);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes2);
  stack.Install (wifiApNode3);
  stack.Install (wifiStaNodes3);
  stack.Install (wifiApNode4);
  stack.Install (wifiStaNodes4);


  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces1;
  csmaInterfaces1 = address.Assign (csmaDevices1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces2;
  csmaInterfaces2 = address.Assign (csmaDevices2);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices1);
  address.Assign (apDevices1);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  address.Assign (staDevices3);
  address.Assign (apDevices3);

  address.SetBase ("10.1.6.0", "255.255.255.0");
  address.Assign (staDevices4);
  address.Assign (apDevices4); 


  // UdpEchoServerHelper echoServer (9);

  // ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  // serverApps.Start (Seconds (1.0));
  // serverApps.Stop (Seconds (10.0));

  // UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  // echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  // echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  // echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  // ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  // clientApps.Start (Seconds (2.0));
  // clientApps.Stop (Seconds (10.0));

  // Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // pointToPoint.EnablePcapAll ("second");
  // csma.EnablePcap ("second", csmaDevices.Get (1), true);

  /* Gerando animacao NetAnim */
  AnimationInterface anim ("TrabalhoFinal.xml");
  anim.SetConstantPosition(nodes.Get(0), 1.0, 2.0);  
  anim.SetConstantPosition(nodes.Get(1), 2.0, 3.0);


  Simulator::Run ();
  Simulator::Destroy ();
  
  return 0;
}
