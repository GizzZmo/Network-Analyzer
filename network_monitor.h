/**
 * @file network_monitor.h
 * @brief Network packet monitoring and analysis header
 * 
 * This header defines the NetworkMonitor class for capturing and analyzing
 * network packets using the libpcap library.
 */

#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

/**
 * @struct PacketInfo
 * @brief Structure to hold captured packet information
 * 
 * Contains extracted metadata from network packets including IP addresses,
 * ports, protocol type, and packet length.
 */
struct PacketInfo {
    std::string source_ip;    ///< Source IP address
    std::string dest_ip;      ///< Destination IP address
    int source_port;          ///< Source port number
    int dest_port;            ///< Destination port number
    std::string protocol;     ///< Protocol type (TCP, UDP, ICMP, etc.)
    int length;               ///< Total packet length in bytes
};

/**
 * @class NetworkMonitor
 * @brief Main class for monitoring network traffic
 * 
 * NetworkMonitor captures live network packets on a specified interface
 * and displays detailed information about each packet including source/destination
 * addresses, ports, and protocol type.
 */
class NetworkMonitor {
public:
    /**
     * @brief Constructor - initializes packet capture on specified device
     * @param device Network interface name (e.g., "eth0", "en0")
     */
    NetworkMonitor(const std::string& device);
    
    /**
     * @brief Destructor - cleans up packet capture resources
     */
    ~NetworkMonitor();

    /**
     * @brief Start capturing packets
     * @param packet_count Number of packets to capture (-1 for infinite)
     */
    void startCapture(int packet_count);

private:
    pcap_t* handle;                ///< pcap session handle
    char errbuf[PCAP_ERRBUF_SIZE]; ///< Error message buffer
    std::string device;            ///< Network device to sniff on

    /**
     * @brief Callback function to process each captured packet
     * @param userData User-defined data passed to callback
     * @param pkthdr Packet header with metadata
     * @param packet Raw packet data
     */
    static void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);
    
    /**
     * @brief Prints formatted packet information to console
     * @param info PacketInfo structure containing packet details
     */
    static void printPacketInfo(const PacketInfo& info);
};

#endif // NETWORK_MONITOR_H
