# Real-Time Network Monitor

[![Build](https://github.com/GizzZmo/Network-Analyzer/actions/workflows/build.yml/badge.svg)](https://github.com/GizzZmo/Network-Analyzer/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A lightweight, real-time network traffic monitor written in C++. It uses the pcap (packet capture) library to sniff packets on a network interface and displays detailed information about them.

## Features

- 📡 Captures live network packets in real-time
- 🌐 Displays source and destination IP addresses and ports
- 🔍 Identifies TCP, UDP, and ICMP protocols
- 🎯 Automatically selects a default network device or uses one specified by the user
- 💻 Cross-platform support (Linux, macOS)
- 🚀 Lightweight with minimal dependencies

## Dependencies

You need to have the `libpcap` library installed on your system.

### On Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install libpcap-dev
```

### On Fedora/CentOS/RHEL:
```bash
sudo yum install libpcap-devel
```

### On macOS:
libpcap is usually included with Xcode Command Line Tools. If not available:
```bash
xcode-select --install
# Or with Homebrew:
brew install libpcap
```

## How to Build

You can build the project using a C++ compiler like g++.

### Clone the repository:
```bash
git clone https://github.com/GizzZmo/Network-Analyzer.git
cd Network-Analyzer
```

### Compile the source files:
```bash
g++ -o network_monitor main.cpp network_monitor.cpp -lpcap
```

## How to Run

You need to run the executable with sudo permissions to access network interfaces.

### Monitor a Specific Interface
You can specify the network interface you want to monitor as a command-line argument:
```bash
sudo ./network_monitor eth0
```
Replace `eth0` with the name of your network interface (e.g., `en0` on macOS, `wlan0` for wireless).

### Monitor the Default Interface
If you don't provide an interface, the program will try to find a default one:
```bash
sudo ./network_monitor
```

Press **Ctrl+C** to stop the monitor.

## Example Output

```
Sniffing on device: eth0
Starting network monitor... (Press Ctrl+C to stop)
Packet captured. Length: 66 | Protocol: TCP | From: 192.168.1.10:12345 -> To: 172.217.16.14:443
Packet captured. Length: 74 | Protocol: UDP | From: 192.168.1.10:54321 -> To: 8.8.8.8:53
Packet captured. Length: 98 | Protocol: ICMP | From: 192.168.1.1 -> To: 192.168.1.10
...
```

## CI/CD and Releases

This project uses GitHub Actions for:
- **Continuous Integration**: Automatically builds the project on Ubuntu and macOS for every push and pull request
- **Release Automation**: Creates distributable binaries for tagged releases

### Downloading Pre-built Binaries

Pre-built binaries are available from the [Releases](https://github.com/GizzZmo/Network-Analyzer/releases) page for:
- Linux (amd64)
- macOS (amd64)

### Creating a Release

To create a new release with binaries:
```bash
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

The release workflow will automatically build and upload binaries for all supported platforms.

For detailed instructions on creating releases, see [RELEASE.md](RELEASE.md).

## Project Structure

```
Network-Analyzer/
├── main.cpp              # Entry point and signal handling
├── network_monitor.h     # Header file with class definitions
├── network_monitor.cpp   # Implementation of NetworkMonitor class
├── README.md            # This file
├── LICENSE              # MIT License
├── CONTRIBUTING.md      # Contribution guidelines
├── RELEASE.md           # Release process documentation
└── .github/
    └── workflows/
        ├── build.yml    # CI workflow for building artifacts
        └── release.yml  # Release workflow for distributable assets
```

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to contribute to this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with [libpcap](https://www.tcpdump.org/) - the packet capture library
- Inspired by classic network monitoring tools
