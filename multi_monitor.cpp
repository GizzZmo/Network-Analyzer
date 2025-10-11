/**
 * @file multi_monitor.cpp
 * @brief Implementation of the MultiMonitor class
 * 
 * This file contains the implementation of multi-interface packet capture
 * functionality using threads and the libpcap library.
 */

#include "multi_monitor.h"
#include <iostream>

/**
 * @brief Constructor - Initializes multi-interface monitoring
 * @param ifaces Vector of interface names to monitor
 * @param use_dash Whether to use dashboard mode
 */
MultiMonitor::MultiMonitor(const std::vector<std::string>& ifaces, bool use_dash)
    : interfaces(ifaces), running(false), use_dashboard(use_dash), dashboard(nullptr) {
    
    if (interfaces.empty()) {
        std::cerr << "No interfaces specified for multi-interface monitoring" << std::endl;
        return;
    }
    
    std::cout << "Initializing multi-interface monitoring for:" << std::endl;
    for (const auto& iface : interfaces) {
        std::cout << "  - " << iface << std::endl;
    }
}

/**
 * @brief Destructor - Cleans up all monitoring threads
 */
MultiMonitor::~MultiMonitor() {
    stopCapture();
}

/**
 * @brief Sets the dashboard for visualization
 * @param dash Shared pointer to dashboard instance
 */
void MultiMonitor::setDashboard(std::shared_ptr<Dashboard> dash) {
    dashboard = dash;
}

/**
 * @brief Thread function for capturing packets on a single interface
 * @param interface_name Name of the interface to capture on
 */
void MultiMonitor::captureThread(const std::string& interface_name) {
    try {
        // Create a monitor for this interface
        auto monitor = std::make_unique<NetworkMonitor>(interface_name, use_dashboard);
        
        if (dashboard) {
            monitor->setDashboard(dashboard);
        }
        
        // Start capturing (this will block until stopped)
        monitor->startCapture(-1);
        
    } catch (const std::exception& e) {
        std::cerr << "Error in capture thread for " << interface_name << ": " << e.what() << std::endl;
    }
}

/**
 * @brief Start capturing packets on all interfaces
 */
void MultiMonitor::startCapture() {
    if (running) {
        std::cerr << "Capture already running" << std::endl;
        return;
    }
    
    running = true;
    
    std::cout << "Starting capture on " << interfaces.size() << " interface(s)..." << std::endl;
    
    // Create a capture thread for each interface
    for (const auto& iface : interfaces) {
        capture_threads.emplace_back(&MultiMonitor::captureThread, this, iface);
    }
    
    // Wait for all threads to complete
    for (auto& thread : capture_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

/**
 * @brief Stop capturing packets on all interfaces
 */
void MultiMonitor::stopCapture() {
    if (!running) {
        return;
    }
    
    running = false;
    
    std::cout << "Stopping capture on all interfaces..." << std::endl;
    
    // Note: pcap_loop doesn't have a clean way to interrupt from another thread
    // In a production system, you'd want to use pcap_breakloop() or similar
    // For now, the threads will stop when the main process exits
}
