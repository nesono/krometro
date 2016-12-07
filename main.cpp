#include <iostream>
#include <cstdlib>
#include "krometro/v1/krometro.h"

KROM_METER(statistics, stddev, 10, 10)
{
    std::vector<int32_t> testvec(1000);
    for(auto i : testvec)
    {
        i = rand();
    }
    auto result = krom::standarddeviation(testvec);
}

int main() {
    return RUN_ALL_TESTS();
}