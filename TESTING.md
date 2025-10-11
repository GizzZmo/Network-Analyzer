# Testing Guide for Network Analyzer

This document provides comprehensive testing guidelines for the Network Analyzer application, including single-interface and multi-interface monitoring capabilities.

## Prerequisites

- Root/Administrator privileges (required for packet capture)
- At least one active network interface
- Multiple network interfaces (for multi-interface testing)

## Building the Application

Before testing, ensure you have built the application:

**Linux/macOS:**
```bash
g++ -o network_monitor main.cpp network_monitor.cpp dashboard.cpp multi_monitor.cpp -lpcap -lpthread
```

**Windows:**
```powershell
g++ -o network_monitor.exe main.cpp network_monitor.cpp dashboard.cpp multi_monitor.cpp -lwpcap -lpacket -lws2_32 -static-libgcc -static-libstdc++
```

## Test Cases

### 1. List Available Interfaces

**Purpose:** Verify that the application can enumerate all network interfaces.

**Command:**
```bash
sudo ./network_monitor --list
```

**Expected Output:**
```
Available network interfaces:

  1. eth0
  2. any
  3. lo
  4. docker0
  ...
```

**Validation:** Ensure all expected interfaces are listed.

---

### 2. Single Interface Monitoring (Default)

**Purpose:** Test basic packet capture on the default interface.

**Command:**
```bash
sudo ./network_monitor
```

**Expected Output:**
```
Using default device: eth0
Sniffing on device: eth0
Starting network monitor... (Press Ctrl+C to stop)
Tip: Use --dashboard flag for visual dashboard mode
     Use --help for more options
[eth0] Packet captured. Length: X | Protocol: TCP | From: X.X.X.X:YYYY -> To: X.X.X.X:ZZZZ
...
```

**Validation:** 
- Application starts without errors
- Packets are captured and displayed with interface name
- Ctrl+C gracefully stops the application

---

### 3. Single Interface Monitoring (Specified)

**Purpose:** Test packet capture on a specific interface.

**Command:**
```bash
sudo ./network_monitor eth0
```

**Expected Output:**
```
Sniffing on device: eth0
Starting network monitor... (Press Ctrl+C to stop)
[eth0] Packet captured. Length: X | Protocol: TCP | From: X.X.X.X:YYYY -> To: X.X.X.X:ZZZZ
...
```

**Validation:**
- Application captures packets only from eth0
- Each packet line shows [eth0] prefix

---

### 4. Interactive Interface Selection

**Purpose:** Test interactive interface selection menu.

**Command:**
```bash
sudo ./network_monitor --interactive
```

**Interactive Steps:**
1. Application displays numbered list of interfaces
2. User enters a number (e.g., 1 for eth0)
3. Application starts monitoring the selected interface

**Expected Output:**
```
Available network interfaces:

  1. eth0
  2. any
  3. lo
  4. docker0

Select interface (1-4): 1
Sniffing on device: eth0
Starting network monitor... (Press Ctrl+C to stop)
[eth0] Packet captured...
```

**Validation:**
- Menu displays correctly
- Selection works
- Invalid selections are handled gracefully

---

### 5. Dashboard Mode (Single Interface)

**Purpose:** Test dashboard visualization with single interface.

**Command:**
```bash
sudo ./network_monitor --dashboard
```

**Expected Output:**
- Screen clears and shows dashboard
- Real-time updates every second
- Displays:
  - Traffic statistics
  - Protocol distribution
  - Top connections
  - Color legend

**Validation:**
- Dashboard displays without errors
- Statistics update in real-time
- Colors are visible (if terminal supports ANSI)
- Ctrl+C exits gracefully

---

### 6. Multi-Interface Monitoring (Basic)

**Purpose:** Test concurrent capture from multiple interfaces.

**Command:**
```bash
sudo ./network_monitor --multi --interfaces eth0,lo
```

**Expected Output:**
```
Initializing multi-interface monitoring for:
  - eth0
  - lo
Starting capture on 2 interface(s)...
Sniffing on device: eth0
Sniffing on device: lo
Starting network monitor... (Press Ctrl+C to stop)
[eth0] Packet captured. Length: X | Protocol: TCP | From: X.X.X.X:YYYY -> To: X.X.X.X:ZZZZ
[lo] Packet captured. Length: Y | Protocol: TCP | From: 127.0.0.1:AAAA -> To: 127.0.0.1:BBBB
...
```

**Validation:**
- Both interfaces are initialized
- Packets from both interfaces are captured
- Interface names are correctly displayed in output
- Packets appear interleaved based on actual traffic

---

### 7. Multi-Interface with Dashboard

**Purpose:** Test dashboard with multi-interface statistics.

**Command:**
```bash
sudo ./network_monitor -m -d --interfaces eth0,lo,docker0
```

**Expected Output:**
- Dashboard displays with additional "Interface Statistics" section
- Shows packet/byte counts per interface
- Updates in real-time

**Validation:**
- Dashboard includes interface statistics section
- Each monitored interface appears in the statistics
- Counts are accurate and update properly

---

### 8. Interactive Multi-Interface Selection

**Purpose:** Test interactive selection of multiple interfaces.

**Command:**
```bash
sudo ./network_monitor --multi --interactive
```

**Interactive Steps:**
1. Application displays numbered list of interfaces
2. User enters comma-separated numbers (e.g., 1,3,4)
3. Application starts monitoring selected interfaces

**Expected Output:**
```
Available network interfaces:

  1. eth0
  2. any
  3. lo
  4. docker0

Select interfaces (comma-separated, e.g., 1,3,4): 1,3
Initializing multi-interface monitoring for:
  - eth0
  - lo
Starting capture on 2 interface(s)...
...
```

**Validation:**
- Menu displays correctly
- Multiple selections work with comma separation
- Invalid selections are handled gracefully
- Selected interfaces start monitoring

---

### 9. Help Documentation

**Purpose:** Verify help text is complete and accurate.

**Command:**
```bash
./network_monitor --help
```

**Expected Output:**
- Clear usage instructions
- All options documented
- Examples provided

**Validation:**
- All implemented features are documented
- Examples are correct and runnable

---

## Performance Testing

### Multi-Interface Performance

**Purpose:** Verify that multi-interface monitoring doesn't drop packets or cause performance issues.

**Test Procedure:**
1. Start multi-interface monitoring with 3-4 interfaces
2. Generate traffic on multiple interfaces simultaneously
3. Monitor CPU and memory usage
4. Run for at least 5 minutes

**Success Criteria:**
- No packet drops or errors
- CPU usage remains reasonable (<50% on modern systems)
- Memory usage is stable (no leaks)
- Application remains responsive

---

## Edge Cases and Error Handling

### Invalid Interface Name

**Command:**
```bash
sudo ./network_monitor nonexistent_interface
```

**Expected:** Clear error message about interface not being available.

---

### No Interfaces Available

**Test:** Run on system with no accessible network interfaces.

**Expected:** Clear error message, graceful exit.

---

### Permission Denied

**Command (without sudo):**
```bash
./network_monitor
```

**Expected:** Clear error about permissions, instruction to use sudo.

---

### Interrupted Capture

**Test:** Start capture, press Ctrl+C after a few seconds.

**Expected:** Clean shutdown, no resource leaks, proper cleanup message.

---

## Automated Testing Script

For convenience, here's a simple test script:

```bash
#!/bin/bash

echo "=== Network Analyzer Test Suite ==="
echo

echo "Test 1: Help Display"
./network_monitor --help
echo "✓ Help test complete"
echo

echo "Test 2: List Interfaces"
sudo ./network_monitor --list
echo "✓ List test complete"
echo

echo "Test 3: Single Interface (3 seconds)"
timeout 3 sudo ./network_monitor eth0 || true
echo "✓ Single interface test complete"
echo

echo "Test 4: Multi-Interface (3 seconds)"
timeout 3 sudo ./network_monitor -m --interfaces eth0,lo || true
echo "✓ Multi-interface test complete"
echo

echo "=== All Tests Complete ==="
```

Save as `test.sh`, make executable with `chmod +x test.sh`, and run with `./test.sh`.

---

## Troubleshooting

### Issue: "Permission denied" errors

**Solution:** Run with sudo/administrator privileges.

### Issue: Interface not showing packets

**Solution:** 
- Verify interface is active: `ip link show`
- Generate traffic on that interface
- Check interface is not in a down state

### Issue: Dashboard not displaying colors

**Solution:** 
- Ensure terminal supports ANSI color codes
- Try a different terminal emulator
- Check TERM environment variable

### Issue: Multi-interface showing only one interface

**Solution:**
- Verify all specified interfaces exist
- Check comma separation in interface list
- Ensure no spaces in interface names

---

## Reporting Issues

When reporting issues, please include:
- Operating system and version
- Network interfaces being monitored
- Command used
- Expected vs actual behavior
- Complete error messages
- Output of `./network_monitor --list`
