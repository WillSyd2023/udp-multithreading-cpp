#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#include <csignal>
#include "trade_packet.h"

bool keep_running {true};

void signal_handler(int signal) {
    keep_running = false;
}

std::string symbols[4] = {"TSLA", "NVDIA", "MSFT", "ANTH.PVT"};

int main() {
    // Register handler for Ctrl+C (SIGINT)
    std::signal(SIGINT, signal_handler);

    // Create the UDP Socket
    int sockfd {socket(AF_INET, SOCK_DGRAM, 0)};
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Define the Destination (Localhost:8888)
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

    // Setup Random Data Generation
    std::mt19937 gen(42);
    std::uniform_int_distribution<> symbol_dist(0, 3);
    std::uniform_real_distribution<double> price_dist(100.0, 150.0);
    std::uniform_real_distribution<double> vol_dist(1.0, 50.0);

    std::cout << "Starting C++ Firehose..." << '\n';

    uint32_t seq = 0;
    while (keep_running) {
        // Create the packet
        TradePacket packet {symbols[symbol_dist(gen)], price_dist(gen), vol_dist(gen), seq++};

        // Send the raw binary struct
        ssize_t sent = sendto(sockfd, &packet, sizeof(TradePacket), 0,
                              (struct sockaddr*) (&dest_addr), sizeof(dest_addr));

        if (sent < 0) perror("Send failed");

        // Slow down slightly for debugging
        usleep(500000); // 500ms delay
        
        if (seq % 10 == 0) std::cout << "Sent " << seq << " packets...\n";
    }
    std::cout << "FIN\n";
    close(sockfd);
    return 0;
}