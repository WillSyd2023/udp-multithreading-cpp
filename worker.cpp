#include <mutex>
#include "trade_packet.h"
#include "welford.h"
#include "worker.h"

void WorkerVolatility::update_loop() {
    std::vector<TradePacket> local_batch {};
    while (running || !channel.empty()) {
        std::unique_lock<std::mutex> lock(mux);

        cv.wait(lock, [&]{ return !channel.empty() || !running; });
        local_batch.swap(channel);

        lock.unlock();

        for (const TradePacket& elm : local_batch) {
            if (m_stock_symbols.find(elm.asset_id) == m_stock_symbols.end()) {
                std::copy_n(elm.symbol_id, 12, \
                    m_stock_symbols[elm.asset_id].data);
            }

            m_welfords[elm.asset_id].update(elm);
        }

        local_batch.clear();
    }
}