#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// Structure to hold packet information
struct PacketInfo {
    std::string source_ip;
    std::string dest_ip;
    int source_port;
    int dest_port;
    std::string protocol;
    int length;
};

// The main class for monitoring network traffic
class NetworkMonitor {
public:
    // Constructor
    NetworkMonitor(const std::string& device);
    // Destructor
    ~NetworkMonitor();

    // Start capturing packets
    void startCapture(int packet_count);

private:
    pcap_t* handle; // Session handle
    char errbuf[PCAP_ERRBUF_SIZE]; // Error string
    std::string device; // Device to sniff on

    // Callback function to process packets
    static void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);
    // Prints details of a captured packet
    static void printPacketInfo(const PacketInfo& info);
};

#endif // NETWORK_MONITOR_H
