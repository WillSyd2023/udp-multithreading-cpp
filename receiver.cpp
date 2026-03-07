#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "trade_packet.h" // The struct we defined above

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    TradePacket packet;
    std::cout << "Listening on port 8888..." << '\n';

    while (true) {
        recvfrom(sockfd, &packet, sizeof(TradePacket), 0, nullptr, nullptr);
        std::cout << "Received ID: " << packet.symbol_id 
                  << " Price: " << packet.price << '\n';
    }
    return 0;
}