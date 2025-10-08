/**
 * @file main.cpp
 * @brief Entry point for the Network Analyzer application
 * 
 * This file contains the main function and signal handling for graceful shutdown.
 * It initializes the NetworkMonitor and starts packet capture on a specified
 * or default network interface.
 */

#include "network_monitor.h"
#include <csignal>
#include <memory>

/// Global pointer to the NetworkMonitor instance for signal handler access
std::unique_ptr<NetworkMonitor> monitor;

/**
 * @brief Signal handler for graceful shutdown
 * 
 * Handles SIGINT (Ctrl+C) to cleanly stop packet capture and exit.
 * The NetworkMonitor destructor will automatically close the pcap session.
 * 
 * @param signum Signal number received
 */
void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    std::cout << "Stopping packet capture..." << std::endl;
    // The pcap_loop will break, and the program will terminate gracefully.
    if(monitor) {
       // A more direct way to stop pcap_loop is needed for immediate exit.
       // For simplicity, we let the destructor handle cleanup.
    }
    exit(signum);
}

/**
 * @brief Main entry point for the network monitor application
 * 
 * Parses command-line arguments, initializes packet capture on the specified
 * or default network interface, and begins monitoring network traffic.
 * 
 * Usage:
 *   ./network_monitor [interface]
 * 
 * @param argc Argument count
 * @param argv Argument vector (optional interface name)
 * @return Exit status code (0 for success)
 */
int main(int argc, char* argv[]) {
    // Register signal handler for Ctrl+C
    signal(SIGINT, signalHandler);

    char* dev_char;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Check for a device argument
    if (argc == 2) {
        dev_char = argv[1];
    } else {
        // Find a default device
        dev_char = pcap_lookupdev(errbuf);
        if (dev_char == nullptr) {
            std::cerr << "Couldn't find default device: " << errbuf << std::endl;
            return 2;
        }
    }

    std::string device(dev_char);
    monitor = std::make_unique<NetworkMonitor>(device);

    std::cout << "Starting network monitor... (Press Ctrl+C to stop)" << std::endl;
    // Capture indefinitely until interrupted. Pass -1 for infinite loop.
    monitor->startCapture(-1);

    return 0;
}
