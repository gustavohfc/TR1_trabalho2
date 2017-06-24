/************************************************************************************************
  Teleinformática e Redes 1 - Trabalho 2

  Alunos: André Luis Souto Ferreira 140016261
          Gustavo Henrique Fernandes Carvalho 140021671
          Victor Fabre Figueiredo 150022948

************************************************************************************************/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

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

struct redeCsmaInfo
{
  NodeContainer csmaNodes;
  CsmaHelper csma;
  NetDeviceContainer csmaDevices;
  Ipv4InterfaceContainer csmaInterfaces;
  ApplicationContainer serverApps;
};


struct redeWifiInfo
{
  NodeContainer wifiStaNodes;
  NodeContainer wifiApNode;
  YansWifiChannelHelper channel;
  YansWifiPhyHelper phy;
  WifiHelper wifi;
  WifiMacHelper mac;
  Ssid ssid;
  NetDeviceContainer staDevices;
  NetDeviceContainer apDevices;
  MobilityHelper mobility;
};

// Prototipos
void inicializa_csma(redeCsmaInfo& rede);
void inicializa_wifi(redeWifiInfo& rede);
void inicializa_csmaGlobalConnection(redeCsmaInfo& redeCsmaGlobalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi);


int main (int argc, char *argv[])
{

  //bool verbose = true;

  // Mostra o log dos clientes e servidores
  // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  redeCsmaInfo redesCsma[2]; // Redes ethernet
  // redeWifiInfo redesWifi[4]; // Redes Wifi

  // Rede ethernet conectada a pelo menos um no de todas as outras redes,
  // permitindo que maquinas nas diferentes redes locais possam se comunicar.
  redeCsmaInfo redeCsmaGlobalConnection;

  inicializa_csma(redesCsma[0]);
  inicializa_csma(redesCsma[1]);

  inicializa_csmaGlobalConnection(redeCsmaGlobalConnection, redesCsma, /*redesWifi*/ NULL);



  InternetStackHelper stack;
  stack.Install (redesCsma[0].csmaNodes);
  stack.Install (redesCsma[1].csmaNodes);

  // for(int i = 0; i < N_REDES_WIFI; i++)
  // {
  //   //stack.Install (redesWifi[i].wifiApNode);
  //   stack.Install (redesWifi[i].wifiStaNodes);
  // }

  // stack.Install (redeCsmaGlobalConnection.csmaNodes);



  Ipv4AddressHelper address;

  address.SetBase ("10.1.0.0", "255.255.255.0");
  redeCsmaGlobalConnection.csmaInterfaces = address.Assign (redeCsmaGlobalConnection.csmaDevices);

  address.SetBase ("10.1.1.0", "255.255.255.0");
  redesCsma[0].csmaInterfaces = address.Assign (redesCsma[0].csmaDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  redesCsma[1].csmaInterfaces = address.Assign (redesCsma[1].csmaDevices);

  // address.SetBase ("10.1.3.0", "255.255.255.0");
  // address.Assign (redesWifi[0].staDevices);
  // address.Assign (redesWifi[0].apDevices);

  // address.SetBase ("10.1.4.0", "255.255.255.0");
  // address.Assign (redesWifi[1].staDevices);
  // address.Assign (redesWifi[1].apDevices);

  // address.SetBase ("10.1.5.0", "255.255.255.0");
  // address.Assign (redesWifi[2].staDevices);
  // address.Assign (redesWifi[2].apDevices);

  // address.SetBase ("10.1.6.0", "255.255.255.0");
  // address.Assign (redesWifi[3].staDevices);
  // address.Assign (redesWifi[3].apDevices);


  // Inicia os servidores, um servidor em cada rede csma
  
  UdpEchoServerHelper echoServer0 (9);
  redesCsma[0].serverApps = echoServer0.Install (redesCsma[0].csmaNodes.Get (10));
  redesCsma[0].serverApps.Start (Seconds (0.0));
  redesCsma[0].serverApps.Stop (Seconds (60.0));

  UdpEchoServerHelper echoServer1 (9);
  redesCsma[1].serverApps = echoServer1.Install (redesCsma[1].csmaNodes.Get (10));
  redesCsma[1].serverApps.Start (Seconds (0.0));
  redesCsma[1].serverApps.Stop (Seconds (60.0));


  // Inicia os clientes
  // for(int i = 0; i < 2; i++)
  // {
  //   UdpEchoClientHelper echoClient (redesCsma[i].csmaInterfaces.GetAddress (10), 9);
  //   echoClient.SetAttribute ("MaxPackets", UintegerValue (35));
  //   echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  //   echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  //   for(int j = 0; j < 2; j++)
  //   {
  //     ApplicationContainer clientApps = echoClient.Install (redesCsma[i].csmaNodes.Get (j));
  //     // Faz com que a cada 0.1 segundos um cliente de cada rede envie um pacote, intercalando entre os servidores.
  //     clientApps.Start (Seconds ( (((j+1) % (i+1)) ? (1.0) : (0.0)) + j / 10.0 ));
  //     clientApps.Stop (Seconds ((60.0)));
  //   }

  //   // for(int j = 0; j < N_NODES_WIFI; j++)
  //   // {
  //   //   ApplicationContainer clientApps = echoClient.Install (redesWifi[i].wifiStaNodes.Get (j));
  //   //   // Faz com que a cada 0.1 segundos um cliente de cada rede envie um pacote, intercalando entre os servidores.
  //   //   clientApps.Start (Seconds ( (((j+1) % (i+1)) ? (1.0) : (0.0)) + j / 10.0 ));
  //   //   clientApps.Stop (Seconds ((60.0)));
  //   // }
  // }


  UdpEchoClientHelper echoClient (redesCsma[0].csmaInterfaces.GetAddress (10), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps = echoClient.Install (redesCsma[1].csmaNodes.Get (0));
  clientApps.Start (Seconds (1));
  clientApps.Stop (Seconds ((60.0)));


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // pointToPoint.EnablePcapAll ("second");
  // csma.EnablePcap ("second", csmaDevices.Get (1), true);

  /* Gerando animacao NetAnim */
  // for(int i = 0; i < 2; i++)
  //   for(int j = 0; j < 11; j++)
  //     AnimationInterface::SetConstantPosition(redesCsma[i].csmaNodes.Get(j), j * 10, i * 10);

  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(0),  30, 30); // No conectado as outras redes por csma
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(1),  30, 20);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(2),  30, 10);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(3),  30,  0);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(4),  20,  0);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(5),  10,  0);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(6),   0,  0);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(7),   0, 10);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(8),   0, 20);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(9),   0, 30);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(10), 10, 30); // Servidor

  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(0),   150, 30); // No conectado as outras redes por csma
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(1),   150, 20);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(2),   150, 10);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(3),   150,  0);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(4),   160,  0);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(5),   170,  0);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(6),   180,  0);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(7),   180, 10);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(8),   170, 20);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(9),   180, 30);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(10),  150, 10); // Servidor



  AnimationInterface anim ("TrabalhoFinal.xml");
  
  // Altera a cor dos servidores para preto
  anim.UpdateNodeColor(redesCsma[0].csmaNodes.Get(10), 0, 0, 0);
  anim.UpdateNodeColor(redesCsma[1].csmaNodes.Get(10), 0, 0, 0);

  // AnimationInterface::SetConstantPosition (wifiApNode.Get (0), 10, 10);
  

  // for(int i = 0; i < N_REDES_CSMA; i++)
  // {
  //   for(int j = 0; j < N_NODES_CSMA; j++)
  //   {
  //     anim.SetConstantPosition(redesCsma[i].csmaNodes.Get(j), 2 * i, 2 * j);
  //     std::cout << 2 * i << " , " << 2 * j << std::endl;
  //   }
  // }
  // for(int i = 0; i < N_REDES_WIFI; i++)
  // {
  //   for(int j = 0; j < N_NODES_WIFI; j++)
  //   {
  //     anim.SetConstantPosition(redesWifi[i].wifiStaNodes.Get(j), 4 + 2 * i, 2 * j);
  //   }
  // }


  Simulator::Stop (Seconds (60.0));
  Simulator::Run ();
  Simulator::Destroy ();
  
  return 0;
}




void inicializa_csma(redeCsmaInfo& rede)
{
  rede.csmaNodes.Create (11);

  rede.csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  rede.csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  rede.csmaDevices = rede.csma.Install (rede.csmaNodes);
}


void inicializa_wifi(redeWifiInfo& rede)
{
  rede.wifiStaNodes.Create (10);
  rede.wifiApNode = rede.wifiStaNodes.Get (0);

  rede.channel = YansWifiChannelHelper::Default ();
  rede.phy = YansWifiPhyHelper::Default ();
  rede.phy.SetChannel (rede.channel.Create ());

  rede.wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  rede.ssid = Ssid ("ns-3-ssid");
  rede.mac.SetType ("ns3::StaWifiMac",
                    "Ssid", SsidValue (rede.ssid),
                    "ActiveProbing", BooleanValue (false));

  rede.staDevices = rede.wifi.Install (rede.phy, rede.mac, rede.wifiStaNodes);

  rede.mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (rede.ssid));

  rede.apDevices = rede.wifi.Install (rede.phy, rede.mac, rede.wifiApNode);

  rede.mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                      "MinX", DoubleValue (0.0),
                                      "MinY", DoubleValue (0.0),
                                      "DeltaX", DoubleValue (5.0),
                                      "DeltaY", DoubleValue (10.0),
                                      "GridWidth", UintegerValue (3),
                                      "LayoutType", StringValue ("RowFirst"));

  rede.mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                  "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  rede.mobility.Install (rede.wifiStaNodes);

  rede.mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  rede.mobility.Install (rede.wifiApNode);
}


void inicializa_csmaGlobalConnection(redeCsmaInfo& redeCsmaGlobalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi)
{
  // Conecta o no 0 de cada rede na rede
  redeCsmaGlobalConnection.csmaNodes.Add (redesCsma[0].csmaNodes.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesCsma[1].csmaNodes.Get (0));

  // for(int i = 0; i < N_REDES_WIFI; i++)
  //   redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[i].wifiStaNodes.Get (0));

  redeCsmaGlobalConnection.csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  redeCsmaGlobalConnection.csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  redeCsmaGlobalConnection.csmaDevices = redeCsmaGlobalConnection.csma.Install (redeCsmaGlobalConnection.csmaNodes);
}