# Security Policy

## Antivirus False Positives

### Windows Defender Detection (Trojan:Script/Wacatac.B!ml)

**This is a false positive.** The Network Analyzer executable is flagged by some antivirus software, particularly Windows Defender, due to the nature of network monitoring tools.

#### Why This Happens

Network monitoring tools like Network Analyzer are frequently flagged as false positives because they:

1. **Use packet capture capabilities**: The tool uses libpcap/Npcap to capture network packets, which requires low-level network access
2. **Access raw network interfaces**: This behavior is similar to some malware that monitors network traffic
3. **Are statically linked**: The Windows build uses `-static-libgcc -static-libstdc++` to avoid runtime dependencies, which can trigger heuristic-based detection
4. **Are unsigned executables**: The binaries are not code-signed with a commercial certificate

The `Trojan:Script/Wacatac.B!ml` detection is a **heuristic-based false positive** commonly triggered by legitimate network tools, packet sniffers, and system utilities.

#### Verification Steps

You can verify the safety of this software by:

1. **Check the source code**: All source code is available in this repository. You can review it to confirm there is no malicious behavior.

2. **Build from source**: Compile the tool yourself using the instructions in the README:
   ```powershell
   g++ -o network_monitor.exe main.cpp network_monitor.cpp dashboard.cpp multi_monitor.cpp -lwpcap -lpacket -lws2_32 -static-libgcc -static-libstdc++ -I"C:/Program Files/Npcap/sdk/Include" -L"C:/Program Files/Npcap/sdk/Lib/x64"
   ```

3. **Check VirusTotal scan**: Each release includes a VirusTotal scan link showing results from multiple antivirus engines. While some may flag it, reputable engines will show it as clean.

4. **Review the GitHub Actions build logs**: The builds are automated and reproducible via GitHub Actions. You can inspect the exact build process.

#### How to Use Despite False Positives

If you want to use the pre-built Windows executable:

**Option 1: Add an Exclusion in Windows Defender**
1. Open Windows Security
2. Go to "Virus & threat protection"
3. Click "Manage settings" under "Virus & threat protection settings"
4. Scroll down to "Exclusions" and click "Add or remove exclusions"
5. Click "Add an exclusion" → "File"
6. Select the `network_monitor.exe` file

**Option 2: Add a Folder Exclusion**
1. Follow steps 1-4 above
2. Click "Add an exclusion" → "Folder"
3. Select the folder containing Network Analyzer

**Option 3: Build from Source**
Building from source eliminates concerns about pre-built binaries. See the [README.md](README.md) for build instructions.

**Option 4: Use Windows Sandbox or Virtual Machine**
If you're still unsure, run the tool in an isolated environment first.

#### False Positive Reports

We understand that antivirus false positives can be concerning. This is an ongoing challenge for all network monitoring and security tools. Similar false positives affect well-known tools like:
- Wireshark (network protocol analyzer)
- Nmap (network scanner)
- tcpdump (packet analyzer)
- Other packet capture utilities

#### Reporting False Positives to Microsoft

You can help improve Windows Defender's detection by reporting this as a false positive:
1. Go to [Microsoft Security Intelligence](https://www.microsoft.com/en-us/wdsi/filesubmission)
2. Submit `network_monitor.exe` as a false positive
3. Provide details about the legitimate nature of the tool

## Vulnerability Reporting

If you discover a legitimate security vulnerability in this project, please report it by:

1. **DO NOT** open a public issue
2. Email the repository maintainer with details
3. Allow reasonable time for a fix before public disclosure

We take security seriously and will respond to legitimate vulnerability reports promptly.

## Supported Versions

We provide security updates for the latest released version. Please ensure you're using the most recent version from the [Releases](https://github.com/GizzZmo/Network-Analyzer/releases) page.

## License and Warranty

This project is provided under the MIT License "as is", without warranty of any kind. See the [LICENSE](LICENSE) file for details.
