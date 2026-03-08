#ifndef TRADE_PACKET_H
#define TRADE_PACKET_H

#include <string>

struct TradePacket {
    char symbol_id[12];
    double price;         
    double volume;       
    uint32_t sequence_no; 

    TradePacket() 
    : symbol_id {}
    , price {}
    , volume {}
    , sequence_no{}
    {
    }

    TradePacket(std::string_view sym, double p, double v, uint32_t seq) 
        : symbol_id {}
        , price {p}
        , volume {v}
        , sequence_no{seq}
    {
        std::fill(std::begin(symbol_id), std::end(symbol_id), 0);
        std::copy_n(sym.data(), std::min(sym.size(), sizeof(symbol_id) - 1), symbol_id);
    }
};

#endif