#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string>
#include <unordered_map>
#include <vector>
#include "trade_packet.h"
#include "welford_volatility.h"
#include "sym_chars.h"

class WorkerVolatility {
    private:
        const std::atomic_bool& m_running;
        std::unordered_map<uint32_t, WelfordVolatility> m_welfords;
        std::unordered_map<uint32_t, SymChars> m_stock_symbols;

    public:
        std::vector<TradePacket> channel;
        std::mutex mux;
        std::condition_variable cv;

        WorkerVolatility(const std::atomic_bool& running)
            : m_running {running}
            , m_welfords {}
            , m_stock_symbols {}
            , channel {}
            , mux {}
            , cv {}
        {
        }

        void update_loop();
};

#endif