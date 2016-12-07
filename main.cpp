#include <iostream>
#include <cstdlib>
#include "krometro/v1/krometro.h"

KROM_METER(statistics, stddev, 100, 10)
{
    std::vector<int32_t> testvec(1000);
    for(auto i : testvec)
    {
        i = rand();
    }
    auto result = krom::internal::standarddeviation(testvec);
}

template<class T>
double slow_stddev(const std::vector<T> &observations)
{
    double e_x = 0;
    for (auto i : observations) {
        e_x += i;
    }
    auto N = observations.size();
    e_x /= N;
    double res = 0;
    for (auto i : observations) {
        auto tmp = i - e_x;
        res += tmp * tmp;
    }

    auto variance = res / (N - 1);
    return std::sqrt(variance);
}

KROM_METER(statistics, stddev_slow, 100, 10)
{
    std::vector<int32_t> testvec(1000);
    for(auto i : testvec)
    {
        i = rand();
    }
    auto result = slow_stddev(testvec);
}

int main() {
    return RUN_ALL_TESTS();
}