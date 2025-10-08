#include "network_monitor.h"
#include <csignal>
#include <memory>

std::unique_ptr<NetworkMonitor> monitor;

// Signal handler for graceful shutdown
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
