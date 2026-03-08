#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string>
#include <unordered_map>
#include <vector>
#include "trade_packet.h"
#include "welford.h"
#include "sym_chars.h"

class WorkerVolatility {
    private:
        std::unordered_map<uint32_t, WelfordVolatility> m_welfords;
        std::unordered_map<uint32_t, SymChars> m_stock_symbols;

    public:
        std::atomic_bool running;
        std::vector<TradePacket> channel;
        std::mutex mux;
        std::condition_variable cv;

        WorkerVolatility()
            : running { true }
            , m_welfords {}
            , m_stock_symbols {}
            , channel {}
            , mux {}
            , cv {}
        {
        }

        void update_loop();
        void print_final_stats();
};

#endif