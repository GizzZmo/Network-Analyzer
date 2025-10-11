# Contributing to Network Analyzer

Thank you for considering contributing to the Network Analyzer project!

## Development Setup

### Prerequisites

- C++ compiler (g++ or clang++)
- libpcap development libraries

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y libpcap-dev
```

**Fedora/CentOS/RHEL:**
```bash
sudo yum install libpcap-devel
```

**macOS:**
```bash
# libpcap is usually included with Xcode Command Line Tools
xcode-select --install
```

**Windows:**
```powershell
# Install Npcap and MinGW using Chocolatey
choco install npcap -y
choco install mingw -y
```

### Building the Project

**Linux/macOS:**
```bash
g++ -o network_monitor main.cpp network_monitor.cpp dashboard.cpp multi_monitor.cpp -lpcap -lpthread
```

**Windows (MinGW):**
```powershell
g++ -o network_monitor.exe main.cpp network_monitor.cpp dashboard.cpp multi_monitor.cpp -lwpcap -lpacket -lws2_32 -static-libgcc -static-libstdc++ -I"C:/Program Files/Npcap/sdk/Include" -L"C:/Program Files/Npcap/sdk/Lib/x64"
```

### Testing Your Changes

After building, test the network monitor:

**Linux/macOS:**
```bash
sudo ./network_monitor
```

**Windows (run as Administrator):**
```powershell
.\network_monitor.exe
```

Or specify a network interface:

**Linux/macOS:**
```bash
sudo ./network_monitor eth0
```

**Windows:**
```powershell
.\network_monitor.exe "Ethernet"
```

## Code Style

- Use consistent indentation (4 spaces)
- Follow existing code conventions
- Add comments for complex logic
- Keep functions focused and concise

## Submitting Changes

1. Fork the repository
2. Create a new branch for your feature or bugfix
3. Make your changes
4. Test your changes thoroughly
5. Submit a pull request with a clear description

## Reporting Issues

When reporting issues, please include:
- Operating system and version
- Network interface being monitored
- Steps to reproduce the issue
- Expected vs actual behavior
- Any error messages

## Questions?

Feel free to open an issue for any questions or discussions!
