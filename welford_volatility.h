#ifndef WELFORD_H
#define WELFORD_H

#include <optional>
#include "trade_packet.h"

/*
Implementation of Welford online algorithm to calculate
the sample (Bessel's correction) standard deviation
of logarithmic returns i.e. volatility
*/
class WelfordVolatility {
    private:
        std::optional<float> m_prev_value;
        long long m_count;
        double m_mean;
        double m_m2;

    public:
        WelfordVolatility()
            : m_prev_value {}
            , m_count { 0LL }
            , m_mean { 0.0 }
            , m_m2 { 0.0 }
        {
        }

        void update(const TradePacket& pack);
        long long get_count();
        double get_volatility();
};

#endif