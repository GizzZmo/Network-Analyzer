/**
 * @file network_monitor.cpp
 * @brief Implementation of the NetworkMonitor class
 * 
 * This file contains the implementation of network packet capture and analysis
 * functionality using the libpcap library.
 */

#include "network_monitor.h"
#include "dashboard.h"

// Static dashboard instance
std::shared_ptr<Dashboard> NetworkMonitor::dashboard = nullptr;

/**
 * @brief Constructor - Opens network device for packet capture
 * 
 * Initializes the pcap session in promiscuous mode, which allows capturing
 * all packets on the network interface, not just those destined for this host.
 * 
 * @param dev Network device name (e.g., "eth0", "wlan0", "en0")
 * @param use_dash Whether to use dashboard mode
 */
NetworkMonitor::NetworkMonitor(const std::string& dev, bool use_dash) 
    : device(dev), handle(nullptr), use_dashboard(use_dash) {
    // Open the session in promiscuous mode
    // Parameters: device, snapshot length, promiscuous mode, timeout (ms), error buffer
    handle = pcap_open_live(device.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "Couldn't open device " << device << ": " << errbuf << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Sniffing on device: " << device << std::endl;
}

/**
 * @brief Destructor - Closes the pcap session and releases resources
 */
NetworkMonitor::~NetworkMonitor() {
    if (handle) {
        pcap_close(handle);
    }
}

/**
 * @brief Sets the dashboard for visualization
 * @param dash Shared pointer to dashboard instance
 */
void NetworkMonitor::setDashboard(std::shared_ptr<Dashboard> dash) {
    dashboard = dash;
}

/**
 * @brief Starts the packet capture loop
 * 
 * Begins capturing packets and calling the packet handler for each one.
 * The loop continues until the specified number of packets is captured
 * or the process is interrupted (e.g., with Ctrl+C).
 * 
 * @param packet_count Number of packets to capture (-1 for infinite loop)
 */
void NetworkMonitor::startCapture(int packet_count) {
    // Start the capture loop
    pcap_loop(handle, packet_count, packetHandler, nullptr);
}

/**
 * @brief Packet handler callback - Processes each captured packet
 * 
 * This static callback function is invoked by pcap_loop for each captured packet.
 * It extracts IP header information, determines the protocol type, and extracts
 * relevant port information for TCP/UDP packets.
 * 
 * @param userData User-defined data (not used in this implementation)
 * @param pkthdr Pointer to packet header with capture metadata
 * @param packet Pointer to raw packet data
 */
void NetworkMonitor::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    // Parse IP header (skip 14-byte Ethernet header)
    const struct ip* ip_header = (struct ip*)(packet + 14);
    const struct tcphdr* tcp_header;
    int ip_header_length = ip_header->ip_hl * 4; // IP header length in bytes

    PacketInfo info;
    info.source_ip = inet_ntoa(ip_header->ip_src);
    info.dest_ip = inet_ntoa(ip_header->ip_dst);
    info.length = pkthdr->len;

    // Determine protocol and extract port information
    switch (ip_header->ip_p) {
        case IPPROTO_TCP:
            info.protocol = "TCP";
            tcp_header = (struct tcphdr*)(packet + 14 + ip_header_length);
            info.source_port = ntohs(tcp_header->th_sport); // Convert from network to host byte order
            info.dest_port = ntohs(tcp_header->th_dport);
            break;
        case IPPROTO_UDP:
            info.protocol = "UDP";
            // UDP header has same layout as TCP for port fields
            tcp_header = (struct tcphdr*)(packet + 14 + ip_header_length);
            info.source_port = ntohs(tcp_header->th_sport);
            info.dest_port = ntohs(tcp_header->th_dport);
            break;
        case IPPROTO_ICMP:
            info.protocol = "ICMP";
            info.source_port = 0; // ICMP doesn't use ports
            info.dest_port = 0;
            break;
        default:
            info.protocol = "Other";
            info.source_port = 0;
            info.dest_port = 0;
            break;
    }

    // Update dashboard if enabled, otherwise print packet info
    if (dashboard) {
        dashboard->updatePacket(info);
    } else {
        printPacketInfo(info);
    }
}

/**
 * @brief Prints formatted packet information to the console
 * 
 * Displays captured packet details in a human-readable format including
 * length, protocol type, source and destination IP addresses and ports.
 * 
 * @param info PacketInfo structure containing the packet metadata
 */
void NetworkMonitor::printPacketInfo(const PacketInfo& info) {
    std::cout << "Packet captured. Length: " << info.length << " | ";
    std::cout << "Protocol: " << info.protocol << " | ";
    std::cout << "From: " << info.source_ip << ":" << info.source_port << " -> ";
    std::cout << "To: " << info.dest_ip << ":" << info.dest_port << std::endl;
}
