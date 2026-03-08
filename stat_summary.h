#ifndef STAT_SUMMARY_H
#define STAT_SUMMARY_H

#include <string>

struct StatSummary {
    std::string name;
    long long count;
    double vol;

    StatSummary(std::string data, long long count, double volatility)
    : name { data }
    , count { count }
    , vol { volatility }
    {

    }
};

#endif