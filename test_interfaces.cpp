#include <iostream>
#include <pcap.h>

int main() {
    pcap_if_t* alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error: " << errbuf << std::endl;
        return 1;
    }
    
    int i = 1;
    for (pcap_if_t* dev = alldevs; dev != nullptr; dev = dev->next) {
        std::cout << i << ". Name: [" << dev->name << "]";
        if (dev->description) {
            std::cout << " Desc: [" << dev->description << "]";
        } else {
            std::cout << " Desc: [NULL]";
        }
        std::cout << std::endl;
        i++;
    }
    
    pcap_freealldevs(alldevs);
    return 0;
}
