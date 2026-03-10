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

std::vector<std::unique_ptr<WorkerVolatility>> workers {};

void reporter_loop(const std::vector<std::unique_ptr<WorkerVolatility>>& workers) {
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // ANSI escape codes to clear screen and move cursor to top-left
        std::cout << "\033[2J\033[H\n"; 
        std::cout << "=== VOLATILITY ENGINE MONITOR ===\n";
        std::cout << "SYMBOL\t\tUPDATES\t\tVOLATILITY\n";
        std::cout << "--------------------------------------------\n";

        for (auto const& w : workers) {
            auto stats = w->get_snapshot();
            for (auto const& s : stats) {
                if (s.vol < 0) {
                    printf("%-10s\t%-10lld\tN/A\n", s.name.c_str(), s.count);
                    continue;
                }
                printf("%-10s\t%-10lld\t%.6f\n", s.name.c_str(), s.count, s.vol);
            }
        }
    }
}

int main() {
    std::signal(SIGINT, signal_handler);

    const int NUM_WORKERS = 4;
    
    workers.reserve(NUM_WORKERS);
    std::vector<std::thread> threads;
    threads.reserve(NUM_WORKERS);

    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers.push_back(std::make_unique<WorkerVolatility>());
    }

    // Start Worker Threads
    for (int i = 0; i < NUM_WORKERS; ++i) {
        // When running a member function, you need to give both:
        // member function pointer
        // the worker's pointer as argument i.e. via get() given use of std::unique_ptr
        threads.emplace_back(&WorkerVolatility::update_loop, workers[i].get());
    }

    // Start reporter thread
    std::thread reporter(reporter_loop, std::ref(workers));

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
    auto start {std::chrono::high_resolution_clock::now()};
    long long count { 0 };
    while (keep_running) {
        ssize_t n = recvfrom(sockfd, &packet, sizeof(TradePacket), 0, nullptr, nullptr);
        if (n <= 0) continue;

        // Shard by ID
        int shard_id {static_cast<int>(packet.asset_id % NUM_WORKERS)};
        auto& worker = workers[shard_id];

        // Securely give value to worker and notify
        {
            std::lock_guard<std::mutex> lock(worker->mux);
            worker->channel.push_back(packet);
        }
        worker->cv.notify_one();
        ++count;
    }
    auto end {std::chrono::high_resolution_clock::now()};
    const auto diff_ns {
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
    };
    const auto diff_s {
        std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
    };
    
    // Stop all workers from endlessly looping
    for (auto& w : workers) {
        std::lock_guard<std::mutex> lock(w->mux);
        w->running = false;
        w->cv.notify_all();
    }

    for (auto& t : threads) {
        t.join();
    }

    // Optional, mostly for testing purposes
    /*
    for (auto& w : workers) {
        w->print_final_stats();
    }
    */
    reporter.join();
    std::cout << "Total time: " << diff_s << "s\n";
    std::cout << "Average latency per update: " << \
        static_cast<double>(diff_ns) / static_cast<double>(count) << "ns\n";
    std::cout << "Throughtput - updates per second: " << \
        static_cast<double>(count) / static_cast<double>(diff_s) << "/s\n";
    // Graceful shutdown
    std::cout << "\nShutdown initiated. Draining queues...\n";

    std::cout << "Engine offline.\n";
    return 0;
}