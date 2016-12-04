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
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#pragma once

#include <vector>
#include <cmath>
#include <map>
#include <string>

#define krom kromv1

namespace krom {
    template<class T>
    double standarddeviation(const std::vector<T> &observations) {
        /*  // This is a plain implementation
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
            //  below is an optimized implementation */
        double A = observations[0];
        double Q = 0;
        for (auto i = 1U; i < observations.size(); ++i) {
            auto A_1 = A;
            A += (observations[i] - A) / (i + 1);
            Q += (observations[i] - A_1) * (observations[i] - A);
        }
        return std::sqrt(Q / (observations.size() - 1));
    }

    template<class T>
    double mean(const std::vector<T> &observations) {
        double m = 0;
        for (auto v: observations)
            m += v;
        return m / observations.size();
    }

    struct KromMeterBase;
    struct KromMeterRegistry
    {
        std::multimap<std::string, KromMeterBase*> test_registry;
        static KromMeterRegistry& instance()
        {
            static KromMeterRegistry instance;
            return instance;
        }
    };

    void registerKromMeter(std::string suite, KromMeterBase* meter)
    {
        KromMeterRegistry::instance().test_registry.emplace(suite, meter);
    }

    struct KromMeterBase
    {
        KromMeterBase(std::string suite)
        {
            registerKromMeter(suite, this);
        }
        virtual ~KromMeterBase() {}

        virtual void Body() = 0;
    };

    struct TestRunner
    {
        int runAllTests() const
        {
            auto testmap = KromMeterRegistry::instance().test_registry;
            for (auto test : testmap) {
                test.second->Body();
            }
            return 0;
        }

        static TestRunner& instance()
        {
            static TestRunner instance;
            return instance;
        }
    };

#define RUN_ALL_TESTS() krom::TestRunner::instance().runAllTests();

#define CONCATEXT( a, b, c, d, e ) a##b##c##d##e
#define CONCAT( a, b, c, d, e ) CONCATEXT( a, b, c, d, e )

#define KROM_METER(suite, name, samples, runs) \
    namespace ___meter { \
      struct CONCAT(Krom_, suite, name, samples, runs) \
        : public krom::KromMeterBase \
      { \
        CONCAT(Krom_, suite, name, samples, runs)() \
          : krom::KromMeterBase(#suite) \
        {} \
        inline void Body(); \
      }; \
    } \
    inline void ___meter::CONCAT(Krom_, suite, name, samples, runs)::Body()
}