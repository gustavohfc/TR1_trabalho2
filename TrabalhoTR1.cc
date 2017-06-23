/************************************************************************************************
  Teleinformática e Redes 1 - Trabalho 2

  Alunos: André Luis Souto Ferreira 140016261
          Gustavo Henrique 140021671
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


// Numero total de redes
#define N_REDES_CSMA 2
#define N_REDES_WIFI 4

// Cada rede csma tem 10 clientes e 1 host
#define N_NODES_CSMA 11

// Cada rede wifi tem 10 clientes
#define N_NODES_WIFI 10

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
void inicializa_csmaLocalConnection(redeCsmaInfo& redeCsmaLocalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi);


int main (int argc, char *argv[])
{

  //bool verbose = true;

  // Mostra o log dos clientes e servidores
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  redeCsmaInfo redesCsma[N_REDES_CSMA]; // Redes ethernet
  redeWifiInfo redesWifi[N_REDES_WIFI]; // Redes Wifi

  // Rede ethernet conectada a pelo menos um no de todas as outras redes,
  // permitindo que maquinas nas diferentes redes locais possam se comunicar.
  redeCsmaInfo redeCsmaLocalConnection;

  // Inicializa as redes
  for(int i = 0; i < N_REDES_CSMA; i++)
    inicializa_csma(redesCsma[i]);

  for(int i = 0; i < N_REDES_WIFI; i++)
    inicializa_wifi(redesWifi[i]);

  inicializa_csmaLocalConnection(redeCsmaLocalConnection, redesCsma, redesWifi);



  InternetStackHelper stack;
  for(int i = 0; i < N_REDES_CSMA; i++)
    stack.Install (redesCsma[i].csmaNodes);

  for(int i = 0; i < N_REDES_WIFI; i++)
  {
    //stack.Install (redesWifi[i].wifiApNode);
    stack.Install (redesWifi[i].wifiStaNodes);
  }

  //stack.Install (redeCsmaLocalConnection.csmaNodes);


  Ipv4AddressHelper address;

  address.SetBase ("10.1.0.0", "255.255.255.0");
  redeCsmaLocalConnection.csmaInterfaces = address.Assign (redeCsmaLocalConnection.csmaDevices);

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
  
  for(int i = 0; i < N_REDES_CSMA; i++)
  {
    UdpEchoServerHelper echoServer(9);
    redesCsma[i].serverApps = echoServer.Install (redesCsma[i].csmaNodes.Get (N_NODES_CSMA - 1));
    redesCsma[i].serverApps.Start (Seconds (0.0));
    redesCsma[i].serverApps.Stop (Seconds (60.0));
  }


  // Inicia os clientes
  for(int i = 0; i < N_REDES_CSMA; i++)
  {
    UdpEchoClientHelper echoClient (redesCsma[i].csmaInterfaces.GetAddress (N_NODES_CSMA - 1), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (35));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    for(int j = 0; j < N_NODES_CSMA; j++)
    {
      ApplicationContainer clientApps = echoClient.Install (redesCsma[i].csmaNodes.Get (j));
      // Faz com que a cada 0.1 segundos um cliente de cada rede envie um pacote, intercalando entre os servidores.
      clientApps.Start (Seconds ( (((j+1) % (i+1)) ? (1.0) : (0.0)) + j / 10.0 ));
      clientApps.Stop (Seconds ((60.0)));
    }

    for(int j = 0; j < N_NODES_WIFI; j++)
    {
      ApplicationContainer clientApps = echoClient.Install (redesWifi[i].wifiStaNodes.Get (j));
      // Faz com que a cada 0.1 segundos um cliente de cada rede envie um pacote, intercalando entre os servidores.
      clientApps.Start (Seconds ( (((j+1) % (i+1)) ? (1.0) : (0.0)) + j / 10.0 ));
      clientApps.Stop (Seconds ((60.0)));
    }
  }


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // pointToPoint.EnablePcapAll ("second");
  // csma.EnablePcap ("second", csmaDevices.Get (1), true);

  /* Gerando animacao NetAnim */
  AnimationInterface anim ("TrabalhoFinal.xml");
  for(int i = 0; i < N_REDES_CSMA; i++)
  {
    for(int j = 0; j < N_NODES_CSMA; j++)
    {
      anim.SetConstantPosition(redesCsma[i].csmaNodes.Get(j), 2 * i, 2 * j);
    }
  }
  for(int i = 0; i < N_REDES_WIFI; i++)
  {
    for(int j = 0; j < N_NODES_WIFI; j++)
    {
      anim.SetConstantPosition(redesWifi[i].wifiStaNodes.Get(j), 4 + 2 * i, 2 * j);
    }
  }


  Simulator::Stop (Seconds (60.0));
  Simulator::Run ();
  Simulator::Destroy ();
  
  return 0;
}




void inicializa_csma(redeCsmaInfo& rede)
{
  rede.csmaNodes.Create (N_NODES_CSMA);

  rede.csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  rede.csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  rede.csmaDevices = rede.csma.Install (rede.csmaNodes);
}


void inicializa_wifi(redeWifiInfo& rede)
{
  rede.wifiStaNodes.Create (N_NODES_WIFI);
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


void inicializa_csmaLocalConnection(redeCsmaInfo& redeCsmaLocalConnection, redeCsmaInfo *redesCsma, redeWifiInfo *redesWifi)
{
  // Conecta o no 0 de cada rede na rede
  for(int i = 0; i < N_REDES_CSMA; i++)
    redeCsmaLocalConnection.csmaNodes.Add (redesCsma[i].csmaNodes.Get (0));

  for(int i = 0; i < N_REDES_WIFI; i++)
    redeCsmaLocalConnection.csmaNodes.Add (redesWifi[i].wifiStaNodes.Get (0));

  redeCsmaLocalConnection.csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  redeCsmaLocalConnection.csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  redeCsmaLocalConnection.csmaDevices = redeCsmaLocalConnection.csma.Install (redeCsmaLocalConnection.csmaNodes);
}