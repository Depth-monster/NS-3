#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/animation-interface.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NetAnimExample");

int main (int argc, char *argv[])
{
  // Set up some default values for the simulation.
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (137));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("14kb/s"));

  // Create two nodes.
  NodeContainer nodes;
  nodes.Create (2);

  // Create a point-to-point link between the nodes.
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  // Install the Internet stack on the nodes.
  InternetStackHelper stack;
  stack.Install (nodes);

  // Assign IP addresses to the device interfaces.
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Create a packet sink on the receiver node to receive these packets.
  PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (interfaces.GetAddress (1), 10));
  ApplicationContainer apps_sink = sink.Install (nodes.Get (1));
  apps_sink.Start (Seconds (0.0));
  apps_sink.Stop (Seconds (10.0));

  // Create an OnOffApplication to send UDP packets from the first node to the second.
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     InetSocketAddress (interfaces.GetAddress (1), 10));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer apps = onoff.Install (nodes.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  // Enable NetAnim
  AnimationInterface anim ("animation.xml");
  anim.SetConstantPosition (nodes.Get (0), 10, 10);
  anim.SetConstantPosition (nodes.Get (1), 20, 20);

  // Run the simulation
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
