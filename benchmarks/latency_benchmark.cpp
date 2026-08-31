#include "MatchingEngine.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace std::chrono;

struct BenchmarkResult {
    double average;
    long long minimum;
    long long maximum;
    long long p50;
    long long p90;
    long long p95;
    long long p99;
};

BenchmarkResult runBenchmark(int numberOfOrders) {

    constexpr int BATCH_SIZE = 1000;
    constexpr int WARMUP_ORDERS = 5000;
    constexpr int REPEATS = 10;

    vector<Order> orders;
    orders.reserve(numberOfOrders);

    mt19937 rng(42);

    uniform_int_distribution<int> sideDist(0, 1);
    uniform_int_distribution<int> priceDist(90, 110);
    uniform_int_distribution<int> quantityDist(1, 100);

    // --------------------------------------------------
    // Generate orders before timing
    // --------------------------------------------------

    for (OrderId id = 1; id <= numberOfOrders; ++id) {

        Side side =
            sideDist(rng) == 0
                ? Side::BUY
                : Side::SELL;

        int price = priceDist(rng);
        int quantity = quantityDist(rng);

        orders.emplace_back(
            id,
            side,
            price,
            quantity
        );
    }

    // --------------------------------------------------
    // Warm-up
    // --------------------------------------------------

    {
        MatchingEngine warmupEngine;

        int warmupCount =
            min(WARMUP_ORDERS, numberOfOrders);

        for (int i = 0; i < warmupCount; ++i) {
            warmupEngine.submitOrder(orders[i]);
        }
    }

    vector<long long> samples;

    int usableOrders =
        numberOfOrders - min(WARMUP_ORDERS, numberOfOrders);

    int batchCount =
        usableOrders / BATCH_SIZE;

    samples.reserve(batchCount * REPEATS);

    // --------------------------------------------------
    // Repeated batch measurements
    // --------------------------------------------------

    for (int repeat = 0; repeat < REPEATS; ++repeat) {

        MatchingEngine engine;

        int index =
            min(WARMUP_ORDERS, numberOfOrders);

        for (int batch = 0;
             batch < batchCount;
             ++batch) {

            auto start =
                steady_clock::now();

            for (int j = 0;
                 j < BATCH_SIZE;
                 ++j) {

                engine.submitOrder(
                    orders[index++]
                );
            }

            auto end =
                steady_clock::now();

            long long totalTime =
                duration_cast<nanoseconds>(
                    end - start
                ).count();

            long long averageBatchLatency =
                totalTime / BATCH_SIZE;

            samples.push_back(
                averageBatchLatency
            );
        }
    }

    // --------------------------------------------------
    // Sort samples
    // --------------------------------------------------

    sort(
        samples.begin(),
        samples.end()
    );

    long long minimum =
        samples.front();

    long long maximum =
        samples.back();

    long long total = 0;

    for (long long sample : samples) {
        total += sample;
    }

    double average =
        static_cast<double>(total)
        / samples.size();

    // --------------------------------------------------
    // Percentile calculation
    // --------------------------------------------------

    auto percentile =
        [&](double p) -> long long {

            size_t index =
                static_cast<size_t>(
                    p * (samples.size() - 1)
                );

            return samples[index];
        };

    return {
        average,
        minimum,
        maximum,
        percentile(0.50),
        percentile(0.90),
        percentile(0.95),
        percentile(0.99)
    };
}

void printResult(
    int numberOfOrders,
    const BenchmarkResult& result
) {

    cout << "\n=====================================\n";
    cout << "Orders: "
         << numberOfOrders
         << '\n';
    cout << "=====================================\n";

    cout << fixed << setprecision(2);

    cout << "Average latency : "
         << result.average
         << " ns/order\n";

    cout << "Minimum latency : "
         << result.minimum
         << " ns/order\n";

    cout << "p50 latency     : "
         << result.p50
         << " ns/order\n";

    cout << "p90 latency     : "
         << result.p90
         << " ns/order\n";

    cout << "p95 latency     : "
         << result.p95
         << " ns/order\n";

    cout << "p99 latency     : "
         << result.p99
         << " ns/order\n";

    cout << "Maximum latency : "
         << result.maximum
         << " ns/order\n";
}

int main() {

    cout << "\n";
    cout << "=====================================\n";
    cout << " Quant Matching Engine\n";
    cout << " Reliable Latency Benchmark\n";
    cout << "=====================================\n";

    vector<int> workloads = {
        10000
    };

    for (int orders : workloads) {

        BenchmarkResult result =
            runBenchmark(orders);

        printResult(
            orders,
            result
        );
    }

    cout << "\nBenchmark complete.\n";

    return 0;
}