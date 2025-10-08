/**
 * @file dashboard.cpp
 * @brief Implementation of the Dashboard class
 * 
 * This file contains the implementation of the real-time network traffic
 * dashboard with color-coded visualizations.
 */

#include "dashboard.h"
#include <sstream>
#include <cmath>

/**
 * @brief Constructor - Initializes the dashboard
 */
Dashboard::Dashboard() : total_packets(0), total_bytes(0) {
    start_time = std::chrono::steady_clock::now();
    last_update = start_time;
}

/**
 * @brief Updates dashboard with new packet information
 * @param info PacketInfo structure containing packet details
 */
void Dashboard::updatePacket(const PacketInfo& info) {
    total_packets++;
    total_bytes += info.length;
    
    // Update protocol statistics
    protocol_counts[info.protocol]++;
    protocol_bytes[info.protocol] += info.length;
    
    // Update connection tracking
    ConnectionInfo conn;
    conn.source_ip = info.source_ip;
    conn.dest_ip = info.dest_ip;
    conn.source_port = info.source_port;
    conn.dest_port = info.dest_port;
    conn.protocol = info.protocol;
    
    auto it = connections.find(conn);
    if (it != connections.end()) {
        it->second++;
    } else {
        connections[conn] = 1;
    }
    
    last_update = std::chrono::steady_clock::now();
}

/**
 * @brief Gets the color code for a given protocol
 * @param protocol Protocol name (TCP, UDP, ICMP, etc.)
 * @return ANSI color code string
 */
std::string Dashboard::getProtocolColor(const std::string& protocol) {
    if (protocol == "TCP") return Colors::TCP;
    if (protocol == "UDP") return Colors::UDP;
    if (protocol == "ICMP") return Colors::ICMP;
    return Colors::OTHER;
}

/**
 * @brief Gets the OSI layer description for a protocol
 * @param protocol Protocol name
 * @return OSI layer description
 */
std::string Dashboard::getOSILayer(const std::string& protocol) {
    if (protocol == "TCP" || protocol == "UDP") {
        return "Layer 4 (Transport)";
    }
    if (protocol == "ICMP") {
        return "Layer 3 (Network)";
    }
    return "Layer 3/4 (Network/Transport)";
}

/**
 * @brief Clears the terminal screen
 */
void Dashboard::clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

/**
 * @brief Formats bytes for human-readable display
 * @param bytes Number of bytes
 * @return Formatted string (e.g., "1.5 KB", "2.3 MB")
 */
std::string Dashboard::formatBytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

/**
 * @brief Draws a horizontal bar chart
 * @param label Label for the bar
 * @param value Current value
 * @param max_value Maximum value for scaling
 * @param color Color for the bar
 * @param width Width of the bar in characters
 */
void Dashboard::drawBar(const std::string& label, size_t value, size_t max_value,
                       const std::string& color, int width) {
    int bar_length = 0;
    if (max_value > 0) {
        bar_length = static_cast<int>((static_cast<double>(value) / max_value) * width);
    }
    
    std::cout << Colors::LABEL << std::setw(10) << std::left << label << Colors::RESET << " │ ";
    std::cout << color;
    for (int i = 0; i < bar_length; i++) {
        std::cout << "█";
    }
    std::cout << Colors::RESET;
    for (int i = bar_length; i < width; i++) {
        std::cout << " ";
    }
    std::cout << " │ " << Colors::LABEL << std::setw(10) << std::right << value << Colors::RESET << std::endl;
}

/**
 * @brief Displays protocol distribution chart
 */
void Dashboard::displayProtocolDistribution() {
    std::cout << Colors::HEADER << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  PROTOCOL DISTRIBUTION (by OSI Layer)                         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    
    // Find max count for scaling
    size_t max_count = 0;
    for (const auto& pair : protocol_counts) {
        if (pair.second > max_count) {
            max_count = pair.second;
        }
    }
    
    // Display each protocol with its OSI layer info
    for (const auto& pair : protocol_counts) {
        std::string color = getProtocolColor(pair.first);
        std::string layer = getOSILayer(pair.first);
        
        std::cout << color << "  " << pair.first << Colors::RESET 
                  << " (" << Colors::LABEL << layer << Colors::RESET << ")" << std::endl;
        drawBar("Packets", pair.second, max_count, color, 40);
        
        size_t bytes = protocol_bytes[pair.first];
        std::cout << Colors::LABEL << "           └─ Traffic: " << formatBytes(bytes) 
                  << Colors::RESET << std::endl;
        std::cout << std::endl;
    }
}

/**
 * @brief Displays traffic statistics
 */
void Dashboard::displayTrafficStats() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
    
    if (duration == 0) duration = 1; // Avoid division by zero
    
    double packets_per_sec = static_cast<double>(total_packets) / duration;
    double bytes_per_sec = static_cast<double>(total_bytes) / duration;
    
    std::cout << Colors::HEADER << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  TRAFFIC STATISTICS                                            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    
    std::cout << Colors::LABEL << "  Total Packets:    " << Colors::RESET << total_packets << std::endl;
    std::cout << Colors::LABEL << "  Total Traffic:    " << Colors::RESET << formatBytes(total_bytes) << std::endl;
    std::cout << Colors::LABEL << "  Monitoring Time:  " << Colors::RESET << duration << " seconds" << std::endl;
    std::cout << Colors::LABEL << "  Packet Rate:      " << Colors::RESET 
              << std::fixed << std::setprecision(2) << packets_per_sec << " packets/sec" << std::endl;
    std::cout << Colors::LABEL << "  Traffic Rate:     " << Colors::RESET 
              << formatBytes(static_cast<size_t>(bytes_per_sec)) << "/sec" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Displays top connections
 */
void Dashboard::displayTopConnections() {
    std::cout << Colors::HEADER << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  TOP 10 CONNECTIONS                                            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    
    // Convert map to vector for sorting
    std::vector<std::pair<ConnectionInfo, size_t>> conn_vec(connections.begin(), connections.end());
    
    // Sort by packet count (descending)
    std::sort(conn_vec.begin(), conn_vec.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Display top 10
    int count = 0;
    for (const auto& pair : conn_vec) {
        if (count >= 10) break;
        
        const ConnectionInfo& conn = pair.first;
        std::string color = getProtocolColor(conn.protocol);
        
        std::cout << "  " << color << conn.protocol << Colors::RESET << " │ ";
        std::cout << conn.source_ip << ":" << conn.source_port << " → ";
        std::cout << conn.dest_ip << ":" << conn.dest_port;
        std::cout << Colors::LABEL << " (" << pair.second << " packets)" << Colors::RESET << std::endl;
        
        count++;
    }
    
    if (connections.empty()) {
        std::cout << Colors::LABEL << "  No connections yet..." << Colors::RESET << std::endl;
    }
    
    std::cout << std::endl;
}

/**
 * @brief Displays the complete dashboard to console
 */
void Dashboard::display() {
    clearScreen();
    
    // Dashboard title
    std::cout << Colors::HEADER;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "║          NETWORK TRAFFIC ANALYZER DASHBOARD                    ║" << std::endl;
    std::cout << "║          Real-time Monitoring with OSI Layer View              ║" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    // Display sections
    displayTrafficStats();
    displayProtocolDistribution();
    displayTopConnections();
    
    // Legend
    std::cout << Colors::HEADER << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  COLOR LEGEND (OSI Model)                                      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::TCP << "■ TCP" << Colors::RESET << " - Layer 4 (Transport Layer)" << std::endl;
    std::cout << "  " << Colors::UDP << "■ UDP" << Colors::RESET << " - Layer 4 (Transport Layer)" << std::endl;
    std::cout << "  " << Colors::ICMP << "■ ICMP" << Colors::RESET << " - Layer 3 (Network Layer)" << std::endl;
    std::cout << "  " << Colors::OTHER << "■ Other" << Colors::RESET << " - Various Layers" << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::LABEL << "Press Ctrl+C to stop monitoring..." << Colors::RESET << std::endl;
}
