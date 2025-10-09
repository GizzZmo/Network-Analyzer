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
#include <memory>
#include <pcap.h>

// Platform-specific includes
#ifdef _WIN32
    // Windows includes
    #include <winsock2.h>
    #include <ws2tcpip.h>
    
    // Windows compatibility definitions for BSD-style structures
    // Define IP header structure compatible with BSD sockets
    struct ip {
        u_char  ip_hl:4;        // header length
        u_char  ip_v:4;         // version
        u_char  ip_tos;         // type of service
        u_short ip_len;         // total length
        u_short ip_id;          // identification
        u_short ip_off;         // fragment offset field
        u_char  ip_ttl;         // time to live
        u_char  ip_p;           // protocol
        u_short ip_sum;         // checksum
        struct in_addr ip_src;  // source address
        struct in_addr ip_dst;  // dest address
    };
    
    // Define TCP header structure compatible with BSD sockets
    struct tcphdr {
        u_short th_sport;       // source port
        u_short th_dport;       // destination port
        u_int   th_seq;         // sequence number
        u_int   th_ack;         // acknowledgement number
        u_char  th_x2:4;        // (unused)
        u_char  th_off:4;       // data offset
        u_char  th_flags;       // flags
        u_short th_win;         // window
        u_short th_sum;         // checksum
        u_short th_urp;         // urgent pointer
    };
#else
    // Unix/Linux/macOS includes
    #include <netinet/ip.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
#endif

// Forward declaration
class Dashboard;

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
    std::string interface;    ///< Network interface name
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
     * @param use_dashboard Whether to use dashboard mode (default: false)
     */
    NetworkMonitor(const std::string& device, bool use_dashboard = false);
    
    /**
     * @brief Destructor - cleans up packet capture resources
     */
    ~NetworkMonitor();

    /**
     * @brief Start capturing packets
     * @param packet_count Number of packets to capture (-1 for infinite)
     */
    void startCapture(int packet_count);
    
    /**
     * @brief Sets the dashboard for visualization
     * @param dash Shared pointer to dashboard instance
     */
    void setDashboard(std::shared_ptr<Dashboard> dash);
    
    /**
     * @brief Lists all available network interfaces
     * @return Vector of interface names
     */
    static std::vector<std::string> listInterfaces();
    
    /**
     * @brief Gets the current network interface name
     * @return Interface name
     */
    std::string getDevice() const;

private:
    pcap_t* handle;                ///< pcap session handle
    char errbuf[PCAP_ERRBUF_SIZE]; ///< Error message buffer
    std::string device;            ///< Network device to sniff on
    bool use_dashboard;            ///< Whether to use dashboard mode
    static std::shared_ptr<Dashboard> dashboard; ///< Shared dashboard instance
    static std::string current_device; ///< Current device being monitored (for packet handler)

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
