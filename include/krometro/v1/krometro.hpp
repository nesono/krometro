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

#pragma once

#include <vector>
#include <cmath>
#include <map>
#include <string>

#define krom kromv1

namespace krom
{
    inline std::ostream& print(std::string label="")
    {
        fprintf(stdout, "[ %-8s ] ", label.c_str());
        return std::cout;
    }

    template<class T>
    double stddev(const std::vector<T> &observations) {
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

    namespace internal {


        struct KromMeterBase;
        using testmap_t = std::multimap<std::string, KromMeterBase *>;

        struct KromMeterRegistry {
            testmap_t test_registry;

            static KromMeterRegistry &instance() {
                static KromMeterRegistry instance;
                return instance;
            }
        };

        inline void registerKromMeter(std::string suite, KromMeterBase *meter) {
            KromMeterRegistry::instance().test_registry.emplace(suite, meter);
        }

        struct KromMeterBase {
            KromMeterBase(
                    std::string suite,
                    std::string name,
                    uint32_t samples,
                    uint32_t runs
            ) : _suite(suite),
                _name(name),
                _samples(samples),
                _runs(runs) {
                registerKromMeter(suite, this);
            }

            virtual ~KromMeterBase() {}

            virtual void Body() = 0;

            const std::string _suite;
            const std::string _name;
            const uint32_t _samples;
            const uint32_t _runs;
            std::vector<double> _runtimes;
        };

        struct TestrunDecorator
        {
            TestrunDecorator(const testmap_t::value_type& test): _test(test)
            {
                krom::print("METER") << test.second->_suite << "." << test.second->_name << std::endl;
            }
            ~TestrunDecorator()
            {
                krom::print("DONE") << _test.second->_suite << "." << _test.second->_name << std::endl;
                std::cout << std::endl;
            }
            const testmap_t::value_type& _test;
        };

        inline void printStatistics(const std::vector<double> &observations, uint64_t samples)
        {
            krom::print() << "  Observed " << observations.size() << " runs with "
                      << samples << " iterations each." << std::endl;
            auto minmax = std::minmax_element(observations.begin(), observations.end());
            krom::print() << "    min: " << *minmax.first << " ns" << std::endl;
            krom::print() << "    max: " << *minmax.second << " ns" << std::endl;
            krom::print() << "    mean: " << mean(observations) << " ns " << std::endl;
            krom::print() << "    std dev: " << stddev(observations) << " ns" << std::endl;
        }
    }

    struct KromFixture
    {
        virtual void Setup() {}
        virtual void Teardown() {}
    };

    struct KromFixtureBaseline : public KromFixture
    {
        virtual void Baseline() {}
        virtual void Validate(const std::vector<double>& dut_runtimes, const std::vector<double>& baseline_runtimes)
        {
            if(mean(baseline_runtimes) < mean(dut_runtimes) )
            {
                 krom::print() << " Error: Baseline was faster in average" << std::endl;
            }
            else
            {
                krom::print() << " Success: Code under test was faster in average" << std::endl;
            }
        }

        std::vector<double> _runtimes_baseline;
    };

    struct TestRunner
    {
        int runAllTests() const
        {
            auto testmap = internal::KromMeterRegistry::instance().test_registry;
            for (auto test : testmap)
            {
                internal::TestrunDecorator deco(test);

                std::function<void()> testMethod = std::bind(&internal::KromMeterBase::Body, test.second);
                test.second->_runtimes = runOneTest(test, testMethod);

                KromFixtureBaseline* fixtureBaseline;
                if((fixtureBaseline = dynamic_cast<KromFixtureBaseline*>(test.second)) != nullptr)
                {
                    krom::print("BASELINE") << std::endl;
                    std::function<void()> baseline = std::bind(&KromFixtureBaseline::Baseline, fixtureBaseline);
                    fixtureBaseline->_runtimes_baseline = runOneTest(test, baseline);
                    auto speedup = mean(fixtureBaseline->_runtimes_baseline) /
                                   mean(test.second->_runtimes);
                    krom::print("TIME") << " Average: " << speedup << " times "
                              << (speedup > 1.0 ? "faster" : "slower") << std::endl;

                    krom::print("VALIDATE") << std::endl;
                    fixtureBaseline->Validate(test.second->_runtimes, fixtureBaseline->_runtimes_baseline);
                }
            }
            return 0;
        }

        template <class KromMeterType>
        std::vector<double> runOneTest(const KromMeterType &test, std::function<void()>& testMethod) const
        {
            std::vector<double> runtimes;

            for(auto r = 0; r < test.second->_runs; ++r)
            {
                performTestRun(test, runtimes, testMethod);

            }
            internal::printStatistics(runtimes, test.second->_samples);
            return runtimes;
        }

        template <class KromMeterType, class ClockType=std::chrono::steady_clock>
        void performTestRun(
                const KromMeterType &test,
                std::vector<double> &runtimes,
                std::function<void()>& testMethod) const
        {
            std::vector<uint64_t> samplestimes;

            for(auto s = 0; s < test.second->_samples; ++s)
            {
                auto start = ClockType::now();
                testMethod();
                auto end = ClockType::steady_clock::now();
                samplestimes.push_back(
                        static_cast<uint64_t>(
                                std::chrono::duration_cast<std::chrono::nanoseconds>(
                                        end - start
                                ).count()
                        )
                );
            }
            runtimes.push_back(mean(samplestimes));
        }

        static TestRunner& instance()
        {
            static TestRunner instance;
            return instance;
        }
    };
}

#define RUN_ALL_TESTS() krom::TestRunner::instance().runAllTests();

#define CONCATEXT( a, b, c, d, e ) a##b##c##d##e
#define CONCAT( a, b, c, d, e ) CONCATEXT( a, b, c, d, e )

#define KROM_METER(suite, name, samples, runs) \
    namespace meter { \
      struct CONCAT(Krom_, suite, name, samples, runs) \
        : public krom::internal::KromMeterBase \
      { \
        CONCAT(Krom_, suite, name, samples, runs)() \
          : krom::internal::KromMeterBase(#suite, #name, samples, runs) \
        {} \
        inline void Body(); \
      }; \
      static CONCAT(Krom_, suite, name, samples, runs) CONCAT(kromInstance, suite, name, samples, runs); \
    } \
    inline void meter::CONCAT(Krom_, suite, name, samples, runs)::Body()

#define KROM_METER_F(suite, name, samples, runs) \
    namespace meter { \
      struct CONCAT(Krom_, suite, name, samples, runs) \
        : public suite, public krom::internal::KromMeterBase \
      { \
        CONCAT(Krom_, suite, name, samples, runs)() \
          : krom::internal::KromMeterBase(#suite, #name, samples, runs), \
            suite() \
        { \
            Setup(); \
        } \
        ~CONCAT(Krom_, suite, name, samples, runs)() \
        { \
            Teardown(); \
        } \
        inline void Body(); \
      }; \
      static CONCAT(Krom_, suite, name, samples, runs) CONCAT(kromInstance, suite, name, samples, runs); \
    } \
    inline void meter::CONCAT(Krom_, suite, name, samples, runs)::Body()
