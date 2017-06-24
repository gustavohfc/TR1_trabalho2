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
  ApplicationContainer clientApps;
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
  ApplicationContainer clientApps;
};

// Prototipos
void inicializa_csma(redeCsmaInfo& rede);
void inicializa_wifi(redeWifiInfo& rede, int n);
void inicializa_csmaGlobalConnection(redeCsmaInfo& redeCsmaGlobalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi);


int main (int argc, char *argv[])
{

  //bool verbose = true;

  // Mostra o log dos clientes e servidores
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  redeCsmaInfo redesCsma[2]; // Redes ethernet
  redeWifiInfo redesWifi[4]; // Redes Wifi

  // Rede ethernet conectada a pelo menos um no de todas as outras redes,
  // permitindo que maquinas nas diferentes redes locais possam se comunicar.
  redeCsmaInfo redeCsmaGlobalConnection;

  inicializa_csma(redesCsma[0]);
  inicializa_csma(redesCsma[1]);
  inicializa_wifi(redesWifi[0], 0);
  inicializa_wifi(redesWifi[1], 1);
  inicializa_wifi(redesWifi[2], 2);
  inicializa_wifi(redesWifi[3], 3);
  inicializa_csmaGlobalConnection(redeCsmaGlobalConnection, redesCsma, redesWifi);



  InternetStackHelper stack;
  stack.Install (redesCsma[0].csmaNodes);
  stack.Install (redesCsma[1].csmaNodes);
  stack.Install (redesWifi[0].wifiStaNodes);
  stack.Install (redesWifi[1].wifiStaNodes);
  stack.Install (redesWifi[2].wifiStaNodes);
  stack.Install (redesWifi[3].wifiStaNodes);
  stack.Install (redesWifi[0].wifiApNode);
  stack.Install (redesWifi[1].wifiApNode);
  stack.Install (redesWifi[2].wifiApNode);
  stack.Install (redesWifi[3].wifiApNode);


  Ipv4AddressHelper address;

  address.SetBase ("10.1.0.0", "255.255.255.0");
  redeCsmaGlobalConnection.csmaInterfaces = address.Assign (redeCsmaGlobalConnection.csmaDevices);

  address.SetBase ("10.1.1.0", "255.255.255.0");
  redesCsma[0].csmaInterfaces = address.Assign (redesCsma[0].csmaDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  redesCsma[1].csmaInterfaces = address.Assign (redesCsma[1].csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (redesWifi[0].staDevices);
  address.Assign (redesWifi[0].apDevices);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  address.Assign (redesWifi[1].staDevices);
  address.Assign (redesWifi[1].apDevices);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  address.Assign (redesWifi[2].staDevices);
  address.Assign (redesWifi[2].apDevices);

  address.SetBase ("10.1.6.0", "255.255.255.0");
  address.Assign (redesWifi[3].staDevices);
  address.Assign (redesWifi[3].apDevices);


  // Inicia os servidores, um servidor em cada rede csma
  
  UdpEchoServerHelper echoServer0 (9);
  redesCsma[0].serverApps = echoServer0.Install (redesCsma[0].csmaNodes.Get (10));
  redesCsma[0].serverApps.Start (Seconds (0.0));
  redesCsma[0].serverApps.Stop (Seconds (60.0));

  UdpEchoServerHelper echoServer1 (9);
  redesCsma[1].serverApps = echoServer1.Install (redesCsma[1].csmaNodes.Get (10));
  redesCsma[1].serverApps.Start (Seconds (0.0));
  redesCsma[1].serverApps.Stop (Seconds (60.0));



  
  UdpEchoClientHelper echoClient_to_server0 (redesCsma[0].csmaInterfaces.GetAddress (10), 9);
  echoClient_to_server0.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient_to_server0.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient_to_server1 (redesCsma[1].csmaInterfaces.GetAddress (10), 9);
  echoClient_to_server1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient_to_server1.SetAttribute ("PacketSize", UintegerValue (1024));

  // Inicia os clientes, a cada 0.1 segubdos um cliente de cada rede ira enviar um pacote para cada um dos servidores
  for(int i = 0; i < 2; i++)
  {
    for(int j = 0; j < 10; j++)
    {
      redesCsma[i].clientApps = echoClient_to_server0.Install (redesCsma[i].csmaNodes.Get (j));
      redesCsma[i].clientApps.Start (Seconds (0.1 * j));
      redesCsma[i].clientApps.Stop (Seconds ((60.0)));

      redesCsma[i].clientApps = echoClient_to_server1.Install (redesCsma[i].csmaNodes.Get (j));
      redesCsma[i].clientApps.Start (Seconds (0.1 * j));
      redesCsma[i].clientApps.Stop (Seconds ((60.0)));
    }
  }

  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 10; j++)
    {
      redesWifi[i].clientApps = echoClient_to_server0.Install (redesWifi[i].wifiStaNodes.Get (j));
      redesWifi[i].clientApps.Start (Seconds (0.1 * j));
      redesWifi[i].clientApps.Stop (Seconds ((60.0)));

      redesWifi[i].clientApps = echoClient_to_server1.Install (redesWifi[i].wifiStaNodes.Get (j));
      redesWifi[i].clientApps.Start (Seconds (0.1 * j));
      redesWifi[i].clientApps.Stop (Seconds ((60.0)));
    }
  }


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  /* Gerando animacao NetAnim */
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(0),  30, 130); // No conectado as outras redes por csma
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(1),  30, 120);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(2),  30, 110);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(3),  30, 100);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(4),  20, 100);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(5),  10, 100);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(6),   0, 100);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(7),   0, 110);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(8),   0, 120);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(9),   0, 130);
  AnimationInterface::SetConstantPosition(redesCsma[0].csmaNodes.Get(10), 10, 130); // Servidor

  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(0),   150, 130); // No conectado as outras redes por csma
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(1),   150, 120);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(2),   150, 110);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(3),   150, 100);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(4),   160, 100);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(5),   170, 100);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(6),   180, 100);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(7),   180, 110);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(8),   180, 120);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(9),   180, 130);
  AnimationInterface::SetConstantPosition(redesCsma[1].csmaNodes.Get(10),  170, 130); // Servidor



  AnimationInterface anim ("TrabalhoFinal.xml");
  
  // Altera a cor dos servidores para preto
  anim.UpdateNodeColor(redesCsma[0].csmaNodes.Get(10), 0, 0, 0);
  anim.UpdateNodeDescription(redesCsma[0].csmaNodes.Get(10), "SRV");
  anim.UpdateNodeColor(redesCsma[1].csmaNodes.Get(10), 0, 0, 0);
  anim.UpdateNodeDescription(redesCsma[1].csmaNodes.Get(10), "SRV");

  // Altera a cor dos APs para azul
  anim.UpdateNodeColor(redesWifi[0].wifiApNode.Get (0), 0, 0, 255);
  anim.UpdateNodeDescription(redesWifi[0].wifiApNode.Get (0), "AP");
  anim.UpdateNodeColor(redesWifi[1].wifiApNode.Get (0), 0, 0, 255);
  anim.UpdateNodeDescription(redesWifi[1].wifiApNode.Get (0), "AP");
  anim.UpdateNodeColor(redesWifi[2].wifiApNode.Get (0), 0, 0, 255);
  anim.UpdateNodeDescription(redesWifi[2].wifiApNode.Get (0), "AP");
  anim.UpdateNodeColor(redesWifi[3].wifiApNode.Get (0), 0, 0, 255);
  anim.UpdateNodeDescription(redesWifi[3].wifiApNode.Get (0), "AP");


  // Altera a cor dos clientes das redes wifi para verde
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 10; j++)
      anim.UpdateNodeColor(redesWifi[i].wifiStaNodes.Get (j), 0, 255, 0);


  anim.EnablePacketMetadata ();



  redeCsmaGlobalConnection.csma.EnablePcap ("TrabalhoFinal", redeCsmaGlobalConnection.csmaDevices.Get (0), true);
  // redeCsmaGlobalConnection.csma.EnablePcapAll ("TrabalhoFinal");

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


void inicializa_wifi(redeWifiInfo& rede, int n)
{
  rede.wifiStaNodes.Create (10);
  rede.wifiApNode.Create (1);

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
                                      "MinX", DoubleValue (60.0 * n),
                                      "MinY", DoubleValue (0.0),
                                      "DeltaX", DoubleValue (10.0),
                                      "DeltaY", DoubleValue (10.0),
                                      "GridWidth", UintegerValue (3),
                                      "LayoutType", StringValue ("RowFirst"));

  rede.mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                  "Bounds", RectangleValue (Rectangle (-10, 250, -10, 90)));
  rede.mobility.Install (rede.wifiStaNodes);

  rede.mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  rede.mobility.Install (rede.wifiApNode);
}


void inicializa_csmaGlobalConnection(redeCsmaInfo& redeCsmaGlobalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi)
{
  // Conecta o no 0 de cada rede na rede
  redeCsmaGlobalConnection.csmaNodes.Add (redesCsma[0].csmaNodes.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesCsma[1].csmaNodes.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[0].wifiApNode.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[1].wifiApNode.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[2].wifiApNode.Get (0));
  redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[3].wifiApNode.Get (0));

  // for(int i = 0; i < N_REDES_WIFI; i++)
  //   redeCsmaGlobalConnection.csmaNodes.Add (redesWifi[i].wifiStaNodes.Get (0));

  redeCsmaGlobalConnection.csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  redeCsmaGlobalConnection.csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  redeCsmaGlobalConnection.csmaDevices = redeCsmaGlobalConnection.csma.Install (redeCsmaGlobalConnection.csmaNodes);
}