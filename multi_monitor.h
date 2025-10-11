/**
 * @file multi_monitor.h
 * @brief Multi-interface network packet monitoring header
 * 
 * This header defines the MultiMonitor class for capturing and analyzing
 * network packets from multiple interfaces simultaneously.
 */

#ifndef MULTI_MONITOR_H
#define MULTI_MONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include "network_monitor.h"
#include "dashboard.h"

/**
 * @class MultiMonitor
 * @brief Manager class for monitoring multiple network interfaces simultaneously
 * 
 * MultiMonitor creates separate threads for each network interface and
 * coordinates packet capture from all of them concurrently.
 */
class MultiMonitor {
public:
    /**
     * @brief Constructor - initializes multi-interface monitoring
     * @param interfaces Vector of interface names to monitor
     * @param use_dashboard Whether to use dashboard mode (default: false)
     */
    MultiMonitor(const std::vector<std::string>& interfaces, bool use_dashboard = false);
    
    /**
     * @brief Destructor - cleans up all monitoring threads
     */
    ~MultiMonitor();

    /**
     * @brief Start capturing packets on all interfaces
     */
    void startCapture();
    
    /**
     * @brief Stop capturing packets on all interfaces
     */
    void stopCapture();
    
    /**
     * @brief Sets the dashboard for visualization
     * @param dash Shared pointer to dashboard instance
     */
    void setDashboard(std::shared_ptr<Dashboard> dash);

private:
    std::vector<std::string> interfaces;           ///< List of interfaces to monitor
    std::vector<std::unique_ptr<NetworkMonitor>> monitors; ///< Monitor for each interface
    std::vector<std::thread> capture_threads;      ///< Thread for each interface
    std::atomic<bool> running;                     ///< Flag to control capture threads
    bool use_dashboard;                            ///< Whether to use dashboard mode
    std::shared_ptr<Dashboard> dashboard;          ///< Shared dashboard instance
    std::mutex mutex;                              ///< Mutex for thread safety
    
    /**
     * @brief Thread function for capturing packets on a single interface
     * @param interface_name Name of the interface to capture on
     */
    void captureThread(const std::string& interface_name);
};

#endif // MULTI_MONITOR_H
