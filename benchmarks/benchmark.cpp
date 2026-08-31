#include "MatchingEngine.h"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace std::chrono;


struct BenchmarkResult {
    long long totalTimeNs;
    double ordersPerSecond;
    double averageLatencyNs;
};


BenchmarkResult runBenchmark(int numberOfOrders) {

    MatchingEngine engine;

    mt19937 rng(42);

    uniform_int_distribution<int> sideDist(0, 1);
    uniform_int_distribution<int> priceDist(90, 110);
    uniform_int_distribution<int> quantityDist(1, 100);

    vector<Order> orders;
    orders.reserve(numberOfOrders);

    // Generate orders before timing.
    // This ensures random-number generation does not
    // affect the matching-engine measurement.
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


    auto start = high_resolution_clock::now();


    for (const auto& order : orders) {
        engine.submitOrder(order);
    }


    auto end = high_resolution_clock::now();


    long long totalTimeNs =
        duration_cast<nanoseconds>(
            end - start
        ).count();


    double averageLatency =
        static_cast<double>(totalTimeNs)
        / numberOfOrders;


    double ordersPerSecond =
        static_cast<double>(numberOfOrders)
        /
        (static_cast<double>(totalTimeNs) / 1e9);


    return {
        totalTimeNs,
        ordersPerSecond,
        averageLatency
    };
}


void printResult(
    int numberOfOrders,
    const BenchmarkResult& result
) {

    cout << "\nOrders: "
         << numberOfOrders
         << '\n';

    cout << "Total time: "
         << result.totalTimeNs
         << " ns\n";

    cout << "Average latency: "
         << result.averageLatencyNs
         << " ns/order\n";

    cout << "Throughput: "
         << result.ordersPerSecond
         << " orders/sec\n";
}


int main() {

    cout << "=====================================\n";
    cout << " Quant Matching Engine Benchmark\n";
    cout << "=====================================\n";


    vector<int> workloads = {
        10000,
        100000,
        1000000
    };


    for (int numberOfOrders : workloads) {

        BenchmarkResult result =
            runBenchmark(numberOfOrders);

        printResult(
            numberOfOrders,
            result
        );
    }


    cout << "\nBenchmark complete.\n";

    return 0;
}