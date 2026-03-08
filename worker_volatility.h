#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <unordered_map>
#include "trade_packet.h"
#include "welford_volatility.h"

class WorkerVolatility {
    private:
        const std::atomic_bool& m_running;
        std::queue<TradePacket> m_channel;
        std::unordered_map<std::string, WelfordVolatility> welfords;

    public:
        std::queue<TradePacket> channel;
        std::mutex mux;
        std::condition_variable cv;

        WorkerVolatility(const std::atomic_bool& running)
            : m_running {running}
            , m_channel {}
            , welfords {}
            , channel {}
            , mux {}
            , cv {}
        {
        }
};

#endif