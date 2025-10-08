Real-Time Network MonitorThis is a simple real-time network traffic monitor written in C++. It uses the pcap (packet capture) library to sniff packets on a network interface and displays information about them.FeaturesCaptures live network packets.Displays source and destination IP addresses and ports.Identifies TCP, UDP, and ICMP protocols.Automatically selects a default network device or uses one specified by the user.DependenciesYou need to have the libpcap library installed on your system.On Debian/Ubuntu:sudo apt-get update
sudo apt-get install libpcap-dev
On Fedora/CentOS/RHEL:sudo yum install libpcap-devel
On macOS (with Homebrew):libpcap is usually included with Xcode Command Line Tools. If not, you can install it:brew install libpcap
How to BuildYou can build the project using a C++ compiler like g++.Clone the repository:git clone <your-repository-url>
cd <your-repository-name>
Compile the source files:g++ -o network_monitor main.cpp network_monitor.cpp -lpcap
How to RunYou need to run the executable with sudo permissions to access network interfaces.Monitor a Specific InterfaceYou can specify the network interface you want to monitor as a command-line argument.sudo ./network_monitor eth0
Replace eth0 with the name of your network interface (e.g., en0 on macOS, wlan0 for wireless).Monitor the Default InterfaceIf you don't provide an interface, the program will try to find a default one.sudo ./network_monitor
Press Ctrl+C to stop the monitor.Example OutputSniffing on device: eth0
Starting network monitor... (Press Ctrl+C to stop)
Packet captured. Length: 66 | Protocol: TCP | From: 192.168.1.10:12345 -> To: 172.217.16.14:443
Packet captured. Length: 74 | Protocol: UDP | From: 192.168.1.10:54321 -> To: 8.8.8.8:53
Packet captured. Length: 98 | Protocol: ICMP | From: 192.168.1.1 -> To: 192.168.1.10
...
LicenseThis project is licensed under the MIT License.
