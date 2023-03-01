set terminal png
set output "nodes-throughput.png"
set title "Nodes vs Throughput"
set xlabel "Number of Nodes"
set ylabel "Avg Throughput (Kbps)"
plot "Nodesdata.txt" using 1:2 with lines title "RED", "CLREDNodesdata.txt" using 1:2 with lines title "CLRED"

set output "nodes-End-to-End-Delay.png"
set title "Nodes vs End-to-End Delay"
set xlabel "Number of Nodes"
set ylabel "End-to-End Delay (ns)"
plot "Nodesdata.txt" using 1:3 with lines title "RED", "CLREDNodesdata.txt" using 1:3 with lines title "CLRED"

set output "nodes-Packet-Delivery-Ratio.png"
set title "Nodes vs Packet Delivery Ratio"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio (in %)"
plot "Nodesdata.txt" using 1:4 with lines title "RED", "CLREDNodesdata.txt" using 1:4 with lines title "CLRED"

set output "nodes-Packet-loss-Ratio.png"
set title "Nodes vs Packet Loss Ratio"
set xlabel "Number of Nodes"
set ylabel "Packet Loss Ratio (in %)"
plot "Nodesdata.txt" using 1:5 with lines title "RED", "CLREDNodesdata.txt" using 1:5 with lines title "CLRED"

set output "Flows-Throughput.png"
set title "Flows vs Throughput"
set xlabel "Number of Flows"
set ylabel "Avg Throughput (Kbps)"
plot "Flowsdata.txt" using 1:2 with lines title "RED", "CLREDFlowsdata.txt" using 1:2 with lines title "CLRED"

set output "Flows-End-to-End-Delay.png"
set title "Flows vs End-to-End Delay"
set xlabel "Number of Flows"
set ylabel "End-to-End Delay (ns)"
plot "Flowsdata.txt" using 1:3 with lines title "RED", "CLREDFlowsdata.txt" using 1:3 with lines title "CLRED"

set output "Flows-Packet-Delivery-Ratio.png"
set title "Flows vs Packet Delivery Ratio"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio (in %)"
plot "Flowsdata.txt" using 1:4 with lines title "RED", "CLREDFlowsdata.txt" using 1:4 with lines title "CLRED"

set output "Flows-Packet-loss-Ratio.png"
set title "Flows vs Packet Loss Ratio"
set xlabel "Number of Flows"
set ylabel "Packet Loss Ratio (in %)"
plot "Flowsdata.txt" using 1:5 with lines title "RED", "CLREDFlowsdata.txt" using 1:5 with lines title "CLRED"

set output "PacketsPerSecond-Throughput.png"
set title "Packets per second vs Throughput"
set xlabel "Packets per second(Mbps)"
set ylabel "Avg Throughput (Kbps)"
plot "DataRate.txt" using 1:2 with lines title "RED", "CLREDDataRate.txt" using 1:2 with lines title "CLRED"


set output "PacketsPerSecond-End-to-End-Delay.png"
set title "Packets per second vs End-to-End Delay"
set xlabel "Packets per second(Mbps)"
set ylabel "End-to-End Delay (ns)"
plot "DataRate.txt" using 1:3 with lines title "RED", "CLREDDataRate.txt" using 1:3 with lines title "CLRED"

set output "PacketsPerSecond-Packet-Delivery-Ratio.png"
set title "Packets per second vs Packet Delivery Ratio"
set xlabel "Packets per second(Mbps)"
set ylabel "Packet Delivery Ratio (in %)"
plot "DataRate.txt" using 1:4 with lines title "RED", "CLREDDataRate.txt" using 1:4 with lines title "CLRED"

set output "PacketsPerSecond-Packet-loss-Ratio.png"
set title "Packets per second vs Packet Loss Ratio"
set xlabel "Packets per second(Mbps)"
set ylabel "Packet Loss Ratio (in %)"
plot "DataRate.txt" using 1:5 with lines title "RED", "CLREDDataRate.txt" using 1:5 with lines title "CLRED"

set output "wireless-nodes-throughput.png"
set title "Nodes vs Throughput"
set xlabel "Number of Nodes"
set ylabel "Avg Throughput (Kbps)"
plot "nWifiData.txt" using 1:2 with lines title "RED"

set output "wireless-nodes-delay.png"
set title "Nodes vs End-to-End Delay"
set xlabel "Number of Nodes"
set ylabel "End-to-End Delay (ns)"
plot "nWifiData.txt" using 1:3 with lines title "RED"

set output "wireless-nodes-delivery-ratio.png"
set title "Nodes vs Packet Delivery Ratio"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio (in %)"
plot "nWifiData.txt" using 1:4 with lines title "RED"

set output "wireless-nodes-loss-ratio.png"
set title "Nodes vs Packet Loss Ratio"
set xlabel "Number of Nodes"
set ylabel "Packet Loss Ratio (in %)"
plot "nWifiData.txt" using 1:5 with lines title "RED"



set output "wireless-speed-throughput.png"
set title "Speed vs Throughput"
set xlabel "Speed of Nodes(m/s)"
set ylabel "Avg Throughput (Kbps)"
plot "speedData.txt" using 1:2 with lines title "RED"

set output "wireless-speed-delay.png"
set title "Speed vs End-to-End Delay"
set xlabel "Speed of Nodes(m/s)"
set ylabel "End-to-End Delay (ns)"
plot "speedData.txt" using 1:3 with lines title "RED"

set output "wireless-speed-delivery-ratio.png"
set title "Speed vs Packet Delivery Ratio"
set xlabel "Speed of Nodes(m/s)"
set ylabel "Packet Delivery Ratio (in %)"
plot "speedData.txt" using 1:4 with lines title "RED"

set output "wireless-speed-loss-ratio.png"
set title "Speed vs Packet Loss Ratio"
set xlabel "Speed of Nodes(m/s)"
set ylabel "Packet Loss Ratio (in %)"
plot "speedData.txt" using 1:5 with lines title "RED"




set output "wireless-interval-throughput.png"
set title "Interval vs Throughput"
set xlabel "Interval(s)"
set ylabel "Avg Throughput (Kbps)"
plot "WirelessDataRate.txt" using 1:2 with lines title "RED"

set output "wireless-interval-delay.png"
set title "Interval vs End-to-End Delay"
set xlabel "Interval(s)"
set ylabel "End-to-End Delay (ns)"
plot "WirelessDataRate.txt" using 1:3 with lines title "RED"

set output "wireless-interval-delivery-ratio.png"
set title "Interval vs Packet Delivery Ratio"
set xlabel "Interval(s)"
set ylabel "Packet Delivery Ratio (in %)"
plot "WirelessDataRate.txt" using 1:4 with lines title "RED"

set output "wireless-interval-loss-ratio.png"
set title "Interval vs Packet Loss Ratio"
set xlabel "Interval(s)"
set ylabel "Packet Loss Ratio (in %)"
plot "WirelessDataRate.txt" using 1:5 with lines title "RED"



















