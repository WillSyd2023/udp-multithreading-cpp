#ifndef WORKER_H
#define WORKER_H

#include <queue>
#include "trade_packet.h"

class WorkerVolatility {
    private:
        std::queue<TradePacket> local_queue;
};

#endif