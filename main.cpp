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
 * @brief Displays usage information
 */
void showHelp() {
    std::cout << "Network Analyzer - Real-time network traffic monitor" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./network_monitor [OPTIONS] [INTERFACE]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d, --dashboard        Enable dashboard mode with visualizations" << std::endl;
    std::cout << "  -l, --list             List all available network interfaces" << std::endl;
    std::cout << "  -i, --interactive      Interactive interface selection" << std::endl;
    std::cout << "  -h, --help             Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./network_monitor                    # Use default interface" << std::endl;
    std::cout << "  ./network_monitor eth0               # Monitor specific interface" << std::endl;
    std::cout << "  ./network_monitor --dashboard        # Dashboard mode with default interface" << std::endl;
    std::cout << "  ./network_monitor -i                 # Interactive interface selection" << std::endl;
    std::cout << "  ./network_monitor --list             # List available interfaces" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Lists all available network interfaces
 */
void listInterfaces() {
    std::cout << "Available network interfaces:" << std::endl;
    std::cout << std::endl;
    
    std::vector<std::string> interfaces = NetworkMonitor::listInterfaces();
    
    if (interfaces.empty()) {
        std::cout << "No network interfaces found." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < interfaces.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << interfaces[i] << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief Interactive interface selection
 * @return Selected interface name, or empty string if cancelled
 */
std::string selectInterface() {
    std::vector<std::string> interfaces = NetworkMonitor::listInterfaces();
    
    if (interfaces.empty()) {
        std::cerr << "No network interfaces found." << std::endl;
        return "";
    }
    
    std::cout << "Available network interfaces:" << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < interfaces.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << interfaces[i] << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Select interface (1-" << interfaces.size() << "): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice < 1 || choice > static_cast<int>(interfaces.size())) {
        std::cerr << "Invalid selection." << std::endl;
        return "";
    }
    
    return interfaces[choice - 1];
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
    bool interactive_mode = false;
    bool list_mode = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "--dashboard" || arg == "-d") {
            use_dashboard = true;
        } else if (arg == "--interactive" || arg == "-i") {
            interactive_mode = true;
        } else if (arg == "--list" || arg == "-l") {
            list_mode = true;
        } else if (arg == "--help" || arg == "-h") {
            showHelp();
            return 0;
        } else if (dev_char == nullptr) {
            dev_char = argv[i];
        }
    }
    
    // Handle list mode
    if (list_mode) {
        listInterfaces();
        return 0;
    }
    
    // Handle interactive mode
    if (interactive_mode) {
        std::string selected = selectInterface();
        if (selected.empty()) {
            return 1;
        }
        dev_char = const_cast<char*>(selected.c_str());
    }

    // Find device if not specified
    if (dev_char == nullptr) {
        // Use pcap_findalldevs instead of deprecated pcap_lookupdev
        pcap_if_t* alldevs;
        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "Couldn't find devices: " << errbuf << std::endl;
            return 2;
        }
        
        if (alldevs == nullptr) {
            std::cerr << "No network interfaces found" << std::endl;
            return 2;
        }
        
        dev_char = alldevs->name;
        std::cout << "Using default device: " << dev_char << std::endl;
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
        std::cout << "     Use --help for more options" << std::endl;
        // Capture indefinitely until interrupted. Pass -1 for infinite loop.
        monitor->startCapture(-1);
    }

    return 0;
}
