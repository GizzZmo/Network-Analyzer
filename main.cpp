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
#include "multi_monitor.h"
#include <csignal>
#include <memory>
#include <thread>
#include <atomic>
#include <sstream>

/// Global pointer to the NetworkMonitor instance for signal handler access
std::unique_ptr<NetworkMonitor> monitor;
/// Global pointer to the MultiMonitor instance
std::unique_ptr<MultiMonitor> multi_monitor;
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
    std::cout << "  -m, --multi            Multi-interface mode (specify interfaces with --interfaces)" << std::endl;
    std::cout << "  --interfaces <list>    Comma-separated list of interfaces for multi-mode" << std::endl;
    std::cout << "  -h, --help             Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./network_monitor                                # Use default interface" << std::endl;
    std::cout << "  ./network_monitor eth0                           # Monitor specific interface" << std::endl;
    std::cout << "  ./network_monitor --dashboard                    # Dashboard mode with default interface" << std::endl;
    std::cout << "  ./network_monitor -i                             # Interactive interface selection" << std::endl;
    std::cout << "  ./network_monitor --list                         # List available interfaces" << std::endl;
    std::cout << "  ./network_monitor -m --interfaces eth0,lo        # Monitor multiple interfaces" << std::endl;
    std::cout << "  ./network_monitor -m -d --interfaces eth0,docker0  # Multi-interface with dashboard" << std::endl;
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
 * @brief Interactive multi-interface selection
 * @return Vector of selected interface names
 */
std::vector<std::string> selectMultipleInterfaces() {
    std::vector<std::string> interfaces = NetworkMonitor::listInterfaces();
    std::vector<std::string> selected;
    
    if (interfaces.empty()) {
        std::cerr << "No network interfaces found." << std::endl;
        return selected;
    }
    
    std::cout << "Available network interfaces:" << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < interfaces.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << interfaces[i] << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Select interfaces (comma-separated, e.g., 1,3,4): ";
    
    std::string input;
    std::cin >> input;
    
    // Parse comma-separated input
    std::stringstream ss(input);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        try {
            int choice = std::stoi(token);
            if (choice >= 1 && choice <= static_cast<int>(interfaces.size())) {
                selected.push_back(interfaces[choice - 1]);
            } else {
                std::cerr << "Warning: Invalid selection " << choice << " ignored." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid input '" << token << "' ignored." << std::endl;
        }
    }
    
    if (selected.empty()) {
        std::cerr << "No valid interfaces selected." << std::endl;
    }
    
    return selected;
}

/**
 * @brief Parse comma-separated interface list
 * @param list Comma-separated list of interface names
 * @return Vector of interface names
 */
std::vector<std::string> parseInterfaceList(const std::string& list) {
    std::vector<std::string> interfaces;
    std::stringstream ss(list);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        
        if (start != std::string::npos && end != std::string::npos) {
            interfaces.push_back(token.substr(start, end - start + 1));
        }
    }
    
    return interfaces;
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
    bool multi_mode = false;
    std::string interface_list;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "--dashboard" || arg == "-d") {
            use_dashboard = true;
        } else if (arg == "--interactive" || arg == "-i") {
            interactive_mode = true;
        } else if (arg == "--list" || arg == "-l") {
            list_mode = true;
        } else if (arg == "--multi" || arg == "-m") {
            multi_mode = true;
        } else if (arg == "--interfaces" && i + 1 < argc) {
            interface_list = argv[++i];
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
    
    // Handle multi-interface mode
    if (multi_mode) {
        std::vector<std::string> interfaces;
        
        if (!interface_list.empty()) {
            // Parse provided interface list
            interfaces = parseInterfaceList(interface_list);
        } else if (interactive_mode) {
            // Interactive selection
            interfaces = selectMultipleInterfaces();
        } else {
            std::cerr << "Multi-interface mode requires --interfaces or --interactive flag" << std::endl;
            std::cerr << "Use --help for more information" << std::endl;
            return 1;
        }
        
        if (interfaces.empty()) {
            std::cerr << "No interfaces specified for monitoring" << std::endl;
            return 1;
        }
        
        // Create multi-monitor instance
        multi_monitor = std::make_unique<MultiMonitor>(interfaces, use_dashboard);
        
        if (use_dashboard) {
            // Create dashboard instance
            dashboard_ptr = std::make_shared<Dashboard>();
            multi_monitor->setDashboard(dashboard_ptr);
            
            std::cout << "Starting multi-interface monitor with dashboard... (Press Ctrl+C to stop)" << std::endl;
            std::cout << "Initializing dashboard in 2 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            // Start dashboard update thread
            std::thread dashboard_thread([&]() {
                while (running) {
                    dashboard_ptr->display();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            });
            
            // Start capture (this will block)
            multi_monitor->startCapture();
            
            // Wait for dashboard thread to finish
            running = false;
            if (dashboard_thread.joinable()) {
                dashboard_thread.join();
            }
        } else {
            std::cout << "Starting multi-interface monitor... (Press Ctrl+C to stop)" << std::endl;
            std::cout << "Tip: Use --dashboard flag for visual dashboard mode" << std::endl;
            // Start capture (this will block)
            multi_monitor->startCapture();
        }
        
        return 0;
    }
    
    // Handle interactive mode (single interface)
    if (interactive_mode) {
        std::string selected = selectInterface();
        if (selected.empty()) {
            return 1;
        }
        dev_char = const_cast<char*>(selected.c_str());
    }

    // Find device if not specified (single interface mode)
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
