// Rede com 5 nós
 // 0 -> cliente
 // 1 -> atacante
 // 2 -> switch 
 // 3 -> switch
 // 4 -> receptor

#include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#define TCP_PORT 9000
#define UDP_PORT 9010

//parameters to change
#define TCP_BYTES 2097152 //2GB
#define ATTACKER_DoS_SPEED "20480kb/s"
#define SIMULATION_TIME 30.0

#define MITIGATE_DoS true

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DoSAttack");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Cria os nós
  NodeContainer nodes;
  nodes.Create (5);

  // Define as conexões entre os nós
  PointToPointHelper canal1, canal2;
  canal1.SetDeviceAttribute ("DataRate", StringValue ("100Mbps")); //Largura de banda
  canal1.SetChannelAttribute ("Delay", StringValue ("1ms")); //RTT

  canal2.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  canal2.SetChannelAttribute ("Delay", StringValue ("20ms"));

  // Define quais conexões são de quais nós
  NetDeviceContainer device02, device12, device23, device34;
  device02 = canal1.Install (nodes.Get(0),nodes.Get(2));
  device12 = canal1.Install (nodes.Get(1),nodes.Get(2));
  device23 = canal2.Install (nodes.Get(2),nodes.Get(3));
  device34 = canal1.Install (nodes.Get(3),nodes.Get(4));

  InternetStackHelper stack;
  stack.Install (nodes);

  // Define IPs e máscaras
  Ipv4AddressHelper end02, end12, end23, end34;
  end02.SetBase ("10.1.1.0", "255.255.255.0");
  end12.SetBase ("10.1.2.0", "255.255.255.0");
  end23.SetBase ("10.1.3.0", "255.255.255.0");
  end34.SetBase ("10.1.4.0", "255.255.255.0");

  // Vincula IPs aos nós
  Ipv4InterfaceContainer i02, i12, i23, i34;
  i02 = end02.Assign(device02);
  i12 = end12.Assign(device12);
  i23 = end23.Assign(device23);
  i34 = end34.Assign(device34);

  // Definindo DoS
  OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (i34.GetAddress(1),UDP_PORT)));
  onoff.SetConstantRate (DataRate (ATTACKER_DoS_SPEED));
  onoff.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=30]"));
  onoff.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer onOffApp = onoff.Install(nodes.Get(1));
  onOffApp.Start(Seconds(0.0));
  onOffApp.Stop(Seconds(SIMULATION_TIME));

  // Cliente normal enviando dados
  BulkSendHelper bulkSend ("ns3::TcpSocketFactory",InetSocketAddress (i34.GetAddress(1), TCP_PORT));
  bulkSend.SetAttribute ("MaxBytes", UintegerValue (TCP_BYTES));
  ApplicationContainer bulkSendApp = bulkSend.Install (nodes.Get(0));
  bulkSendApp.Start (Seconds (0.0));
  bulkSendApp.Stop (Seconds (SIMULATION_TIME-10));

  // Receptor de pacotes TCP
  PacketSinkHelper TCPsink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), TCP_PORT));
  ApplicationContainer TCPSinkApp = TCPsink.Install (nodes.Get (4));
  TCPSinkApp.Start (Seconds (0.0));
  TCPSinkApp.Stop (Seconds (SIMULATION_TIME));

  //Mitiga o efeito do ataque DoS negando os pacotes do atacante com probabilidade x
  if(MITIGATE_DoS == true){

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (0.01));
    device12.Get(1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Hapilita gravação de arquivos PCAP
  canal1.EnablePcapAll("DoSAttack");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
