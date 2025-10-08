/**
 * @file main.cpp
 * @brief Entry point for the Network Analyzer application
 * 
 * This file contains the main function and signal handling for graceful shutdown.
 * It initializes the NetworkMonitor and starts packet capture on a specified
 * or default network interface with optional dashboard visualization.
 */

#include "network_monitor.h"
#include "dashboard.h"
#include <csignal>
#include <memory>
#include <thread>
#include <atomic>

/// Global pointer to the NetworkMonitor instance for signal handler access
std::unique_ptr<NetworkMonitor> monitor;
/// Global pointer to the Dashboard instance
std::shared_ptr<Dashboard> dashboard_ptr;
/// Flag to control dashboard updates
std::atomic<bool> running(true);

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
    running = false;
    // A more direct way to stop pcap_loop is needed for immediate exit.
    // For simplicity, we let the destructor handle cleanup.
    exit(signum);
}

/**
 * @brief Main entry point for the network monitor application
 * 
 * Parses command-line arguments, initializes packet capture on the specified
 * or default network interface, and begins monitoring network traffic.
 * Supports dashboard mode with --dashboard flag.
 * 
 * Usage:
 *   ./network_monitor [interface] [--dashboard]
 *   ./network_monitor --dashboard [interface]
 * 
 * @param argc Argument count
 * @param argv Argument vector (optional interface name and --dashboard flag)
 * @return Exit status code (0 for success)
 */
int main(int argc, char* argv[]) {
    // Register signal handler for Ctrl+C
    signal(SIGINT, signalHandler);

    char* dev_char = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE];
    bool use_dashboard = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "--dashboard" || arg == "-d") {
            use_dashboard = true;
        } else if (dev_char == nullptr) {
            dev_char = argv[i];
        }
    }

    // Find device if not specified
    if (dev_char == nullptr) {
        dev_char = pcap_lookupdev(errbuf);
        if (dev_char == nullptr) {
            std::cerr << "Couldn't find default device: " << errbuf << std::endl;
            return 2;
        }
    }

    std::string device(dev_char);
    monitor = std::make_unique<NetworkMonitor>(device, use_dashboard);

    if (use_dashboard) {
        // Create dashboard instance
        dashboard_ptr = std::make_shared<Dashboard>();
        monitor->setDashboard(dashboard_ptr);
        
        std::cout << "Starting network monitor with dashboard... (Press Ctrl+C to stop)" << std::endl;
        std::cout << "Initializing dashboard in 2 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Start dashboard update thread
        std::thread dashboard_thread([&]() {
            while (running) {
                dashboard_ptr->display();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        
        // Capture packets (this will block)
        monitor->startCapture(-1);
        
        // Wait for dashboard thread to finish
        running = false;
        if (dashboard_thread.joinable()) {
            dashboard_thread.join();
        }
    } else {
        std::cout << "Starting network monitor... (Press Ctrl+C to stop)" << std::endl;
        std::cout << "Tip: Use --dashboard flag for visual dashboard mode" << std::endl;
        // Capture indefinitely until interrupted. Pass -1 for infinite loop.
        monitor->startCapture(-1);
    }

    return 0;
}
