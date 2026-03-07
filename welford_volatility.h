#ifndef WELFORD_H
#define WELFORD_H

#include <optional>

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
};

#endif