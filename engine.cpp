#include <iostream>
#include <vector>
#include <thread>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "worker.h"

// Global flag for Signal Handler
std::atomic_bool keep_running{true};

void signal_handler(int signal) {
    keep_running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);

    const int NUM_WORKERS = 4;
    
    std::vector<std::unique_ptr<WorkerVolatility>> workers;
    workers.reserve(NUM_WORKERS);
    std::vector<std::thread> threads;
    threads.reserve(NUM_WORKERS);

    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers.push_back(std::make_unique<WorkerVolatility>());
    }

    // Start Worker Threads
    for (int i = 0; i < NUM_WORKERS; ++i) {
        // When running a member function, you need to give the worker's pointer
        // as argument i.e. via get() given the use of std::unique_ptr
        threads.emplace_back(&WorkerVolatility::update_loop, workers[i].get());
    }

    // Setup listening for UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in servaddr {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    std::cout << "Engine online. Listening on 8888..." << std::endl;

    // Main ingestor loop
    TradePacket packet;
    while (keep_running) {
        ssize_t n = recvfrom(sockfd, &packet, sizeof(TradePacket), 0, nullptr, nullptr);
        if (n <= 0) continue;

        // Shard by ID
        int shard_id {packet.asset_id % NUM_WORKERS};
        auto& worker = workers[shard_id];

        // Securely give value to worker and notify
        {
            std::lock_guard<std::mutex> lock(worker->mux);
            worker->channel.push_back(packet);
        }
        worker->cv.notify_one();
    }

    // Graceful shutdown
    std::cout << "\nShutdown initiated. Draining queues...\n";
    
    // Stop all workers from endlessly looping
    for (auto& w : workers) {
        std::lock_guard<std::mutex> lock(w->mux);
        w->running = false;
        w->cv.notify_all();
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Engine offline.\n";
    return 0;
}