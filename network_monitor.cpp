#include "network_monitor.h"

// Constructor implementation
NetworkMonitor::NetworkMonitor(const std::string& dev) : device(dev), handle(nullptr) {
    // Open the session in promiscuous mode
    handle = pcap_open_live(device.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "Couldn't open device " << device << ": " << errbuf << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Sniffing on device: " << device << std::endl;
}

// Destructor implementation
NetworkMonitor::~NetworkMonitor() {
    if (handle) {
        pcap_close(handle);
    }
}

// Start capturing packets
void NetworkMonitor::startCapture(int packet_count) {
    // Start the capture
    pcap_loop(handle, packet_count, packetHandler, nullptr);
}

// Callback function to process each packet
void NetworkMonitor::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    const struct ip* ip_header = (struct ip*)(packet + 14); // Skip Ethernet header
    const struct tcphdr* tcp_header;
    int ip_header_length = ip_header->ip_hl * 4;

    PacketInfo info;
    info.source_ip = inet_ntoa(ip_header->ip_src);
    info.dest_ip = inet_ntoa(ip_header->ip_dst);
    info.length = pkthdr->len;

    switch (ip_header->ip_p) {
        case IPPROTO_TCP:
            info.protocol = "TCP";
            tcp_header = (struct tcphdr*)(packet + 14 + ip_header_length);
            info.source_port = ntohs(tcp_header->th_sport);
            info.dest_port = ntohs(tcp_header->th_dport);
            break;
        case IPPROTO_UDP:
            info.protocol = "UDP";
            // For UDP, port info is at the same offset as TCP
            tcp_header = (struct tcphdr*)(packet + 14 + ip_header_length);
            info.source_port = ntohs(tcp_header->th_sport);
            info.dest_port = ntohs(tcp_header->th_dport);
            break;
        case IPPROTO_ICMP:
            info.protocol = "ICMP";
            info.source_port = 0;
            info.dest_port = 0;
            break;
        default:
            info.protocol = "Other";
            info.source_port = 0;
            info.dest_port = 0;
            break;
    }

    printPacketInfo(info);
}

// Print packet information to the console
void NetworkMonitor::printPacketInfo(const PacketInfo& info) {
    std::cout << "Packet captured. Length: " << info.length << " | ";
    std::cout << "Protocol: " << info.protocol << " | ";
    std::cout << "From: " << info.source_ip << ":" << info.source_port << " -> ";
    std::cout << "To: " << info.dest_ip << ":" << info.dest_port << std::endl;
}
