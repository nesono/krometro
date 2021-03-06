/* BSD LICENSE

   Copyright (c) 2016, Jochen Issing <iss@nesono.com>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
       * Neither the name of the <organization> nor the
         names of its contributors may be used to endorse or promote products
         derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include <cstdlib>
#include <iostream>

#include "krometro/v1/krometro.hpp"
#include "krometro/v1/krometro_main.hpp"

const int vecsize = 1000;

KROM_METER(statistics, stddev, 100, 10)
{
    std::vector<int32_t> testvec(vecsize);
    for(auto i : testvec)
    {
        i = rand();
    }
    auto result = krom::stddev(testvec);
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
    std::vector<int32_t> testvec(vecsize);
    for(auto i : testvec)
    {
        i = rand();
    }
    auto result = slow_stddev(testvec);
}

struct stat_fixture : public krom::KromFixture
{
    void Setup()
    {
        testvec.resize(vecsize);
        for(auto i : testvec)
        {
            i = rand();
        }
    }

    void Teardown() {}

    std::vector<int32_t> testvec;
};

KROM_METER_F(stat_fixture, stddev, 100, 10)
{
    auto result = krom::stddev(testvec);
}

struct stat_fixture_baseline : public krom::KromFixtureBaseline
{
    void Setup()
    {
        testvec.resize(vecsize);
        for(auto i : testvec)
        {
            i = rand();
        }
    }

    void Teardown() {}

    void Baseline()
    {
        auto result = slow_stddev(testvec);
    }

    std::vector<int32_t> testvec;
};

KROM_METER_F(stat_fixture_baseline, stddev, 100, 10)
{
    auto result = krom::stddev(testvec);
}
