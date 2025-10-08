/**
 * @file dashboard.h
 * @brief Real-time network traffic dashboard with color-coded visualizations
 * 
 * This header defines the Dashboard class for visualizing network traffic
 * with color-coded protocols based on OSI model layers.
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "network_monitor.h"

/**
 * @namespace Colors
 * @brief ANSI color codes for terminal output
 */
namespace Colors {
    // Reset
    const std::string RESET = "\033[0m";
    
    // OSI Layer 3 (Network Layer) - Blue tones
    const std::string ICMP = "\033[38;5;33m";      // Bright blue for ICMP
    const std::string IP = "\033[38;5;27m";        // Blue for IP
    
    // OSI Layer 4 (Transport Layer) - Green and Yellow tones
    const std::string TCP = "\033[38;5;46m";       // Bright green for TCP
    const std::string UDP = "\033[38;5;226m";      // Yellow for UDP
    
    // Other protocols - Magenta
    const std::string OTHER = "\033[38;5;201m";    // Magenta for others
    
    // UI Elements
    const std::string HEADER = "\033[38;5;51m";    // Cyan for headers
    const std::string LABEL = "\033[38;5;250m";    // Gray for labels
    const std::string BAR = "\033[38;5;208m";      // Orange for bars
}

/**
 * @struct ConnectionInfo
 * @brief Stores information about a network connection
 */
struct ConnectionInfo {
    std::string source_ip;
    std::string dest_ip;
    int source_port;
    int dest_port;
    std::string protocol;
    size_t packet_count;
    size_t total_bytes;
    
    bool operator<(const ConnectionInfo& other) const {
        if (source_ip != other.source_ip) return source_ip < other.source_ip;
        if (dest_ip != other.dest_ip) return dest_ip < other.dest_ip;
        if (source_port != other.source_port) return source_port < other.source_port;
        if (dest_port != other.dest_port) return dest_port < other.dest_port;
        return protocol < other.protocol;
    }
};

/**
 * @class Dashboard
 * @brief Real-time dashboard for network traffic visualization
 * 
 * Collects packet statistics and displays color-coded visualizations
 * organized by OSI model layers and protocol types.
 */
class Dashboard {
public:
    /**
     * @brief Constructor - initializes the dashboard
     */
    Dashboard();
    
    /**
     * @brief Updates dashboard with new packet information
     * @param info PacketInfo structure containing packet details
     */
    void updatePacket(const PacketInfo& info);
    
    /**
     * @brief Displays the dashboard to console
     */
    void display();
    
    /**
     * @brief Gets the color code for a given protocol
     * @param protocol Protocol name (TCP, UDP, ICMP, etc.)
     * @return ANSI color code string
     */
    static std::string getProtocolColor(const std::string& protocol);
    
    /**
     * @brief Gets the OSI layer description for a protocol
     * @param protocol Protocol name
     * @return OSI layer description
     */
    static std::string getOSILayer(const std::string& protocol);

private:
    // Statistics
    std::map<std::string, size_t> protocol_counts;
    std::map<std::string, size_t> protocol_bytes;
    std::map<ConnectionInfo, size_t> connections;
    size_t total_packets;
    size_t total_bytes;
    
    // Timing
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_update;
    
    /**
     * @brief Displays protocol distribution chart
     */
    void displayProtocolDistribution();
    
    /**
     * @brief Displays traffic statistics
     */
    void displayTrafficStats();
    
    /**
     * @brief Displays top connections
     */
    void displayTopConnections();
    
    /**
     * @brief Draws a horizontal bar chart
     * @param label Label for the bar
     * @param value Current value
     * @param max_value Maximum value for scaling
     * @param color Color for the bar
     * @param width Width of the bar in characters
     */
    void drawBar(const std::string& label, size_t value, size_t max_value, 
                 const std::string& color, int width = 40);
    
    /**
     * @brief Formats bytes for human-readable display
     * @param bytes Number of bytes
     * @return Formatted string (e.g., "1.5 KB", "2.3 MB")
     */
    std::string formatBytes(size_t bytes);
    
    /**
     * @brief Clears the terminal screen
     */
    void clearScreen();
};

#endif // DASHBOARD_H
