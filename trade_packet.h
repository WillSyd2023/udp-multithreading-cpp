#ifndef TRADE_PACKET_H
#define TRADE_PACKET_H

#include <cstdint>

struct TradePacket {
    double price;         
    double volume;       
    uint32_t symbol_id;   
    uint32_t sequence_no; 
};

#endif