#include <cmath>
#include "welford.h"

void WelfordVolatility::update(const TradePacket& pack) {
    if (not m_prev_value.has_value()) {
        m_prev_value = pack.price * pack.volume;
        return;
    }

    ++m_count;

    double log_return { m_prev_value.value() / (pack.price * pack.volume) };
    log_return = std::log(log_return);
    m_prev_value = pack.price * pack.volume;

    double old_mean { m_mean };
    m_mean += (log_return - m_mean) / static_cast<double>(m_count);
    m_m2 += (log_return - old_mean) * (log_return - m_mean);
}

long long WelfordVolatility::get_count() {
    return m_count;
}

std::optional<double> WelfordVolatility::get_volatility() {
    if (m_count > 1) {
        return std::sqrt(m_m2 / static_cast<double>(m_count - 1));
    }

    return {};
}