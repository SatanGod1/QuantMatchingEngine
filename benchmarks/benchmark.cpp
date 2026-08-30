#include "MatchingEngine.h"
#include "LatencyStats.h"

#include <chrono>
#include <iostream>
#include <random>


using Clock = std::chrono::steady_clock;


// ============================================================
// INSERTION BENCHMARK
// ============================================================

void benchmarkInsertion()
{
    constexpr int NUM_ORDERS = 100000;


    MatchingEngine engine;

    LatencyStats latency;


    auto start =
        Clock::now();


    for (int i = 0; i < NUM_ORDERS; ++i) {

        Order order(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            1,
            i + 1
        );


        auto orderStart =
            Clock::now();


        engine.submitOrder(order);


        auto orderEnd =
            Clock::now();


        uint64_t ns =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                orderEnd - orderStart
            ).count();


        latency.record(ns);
    }


    auto end =
        Clock::now();


    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();


    double throughput =
        NUM_ORDERS / seconds;

    // double tradesPerSecond =
    //     engine.getTrades().size() / seconds;

    std::cout
        << "\n========================================\n";

    std::cout
        << "       INSERTION BENCHMARK\n";

    std::cout
        << "========================================\n\n";


    std::cout
        << "Orders: "
        << NUM_ORDERS
        << '\n';

    std::cout
        << "Time: "
        << seconds
        << " seconds\n";


    std::cout
        << "Throughput: "
        << throughput
        << " orders/sec\n";

    std::cout
        << "Average latency: "
        << latency.average()
        << " ns\n";


    std::cout
        << "P50 latency: "
        << latency.percentile(0.50)
        << " ns\n";


    std::cout
        << "P99 latency: "
        << latency.percentile(0.99)
        << " ns\n";
}


// ============================================================
// MATCHING BENCHMARK
// ============================================================

void benchmarkMatching()
{
    constexpr int NUM_ORDERS = 100000;


    MatchingEngine engine;

    LatencyStats latency;


    // ========================================================
    // CREATE RESTING SELL ORDERS
    // ========================================================

    for (int i = 0; i < NUM_ORDERS; ++i) {

        Order sellOrder(
            i + 1,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            1,
            i + 1
        );


        engine.submitOrder(
            sellOrder
        );
    }


    // ========================================================
    // MATCH BUY ORDERS
    // ========================================================

    auto start =
        Clock::now();


    for (int i = 0; i < NUM_ORDERS; ++i) {

        Order buyOrder(
            NUM_ORDERS + i + 1,
            Side::BUY,
            OrderType::MARKET,
            0,
            1,
            NUM_ORDERS + i + 1
        );


        auto orderStart =
            Clock::now();


        engine.submitOrder(
            buyOrder
        );


        auto orderEnd =
            Clock::now();


        uint64_t ns =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                orderEnd - orderStart
            ).count();


        latency.record(ns);
    }


    auto end =
        Clock::now();


    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();


    double throughput =
        NUM_ORDERS / seconds;

    double tradesPerSecond =
        static_cast<double>(
            engine.getTrades().size()) /
        seconds;

    std::cout
        << "\n========================================\n";

    std::cout
        << "         MATCHING BENCHMARK\n";

    std::cout
        << "========================================\n\n";


    std::cout
        << "Trades: "
        << NUM_ORDERS
        << '\n';

    std::cout
        << "Trades/sec: "
        << tradesPerSecond
        << '\n';

    std::cout
        << "Time: "
        << seconds
        << " seconds\n";


    std::cout
        << "Throughput: "
        << throughput
        << " orders/sec\n";


    std::cout
        << "Average latency: "
        << latency.average()
        << " ns\n";


    std::cout
        << "P50 latency: "
        << latency.percentile(0.50)
        << " ns\n";


    std::cout
        << "P99 latency: "
        << latency.percentile(0.99)
        << " ns\n";
}


// ============================================================
// MIXED WORKLOAD BENCHMARK
// ============================================================

void benchmarkMixedWorkload()
{
    constexpr int NUM_ORDERS = 100000;

    MatchingEngine engine;

    MatchingEngine warmupEngine;

    for (int i = 0; i < 1000; ++i)
    {

        Order warmupOrder(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            1,
            i + 1);

        warmupEngine.submitOrder(warmupOrder);
    }

    LatencyStats latency;


    // ========================================================
    // RANDOM GENERATOR
    // ========================================================

    std::mt19937 rng(42);


    std::uniform_int_distribution<int> sideDist(0, 1);

    std::uniform_int_distribution<int> typeDist(0, 99);

    std::uniform_int_distribution<int> priceDist(
        9900,
        10100
    );

    // ========================================================
    // WARM-UP
    // ========================================================

    for (int i = 0; i < 1000; ++i)
    {

        Order warmupOrder(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            1,
            i + 1);

        engine.submitOrder(warmupOrder);
    }

    // ========================================================
    // START TIMER
    // ========================================================

    auto start =
        Clock::now();


    for (int i = 0; i < NUM_ORDERS; ++i) {

        // ----------------------------------------------------
        // RANDOM SIDE
        // ----------------------------------------------------

        Side side;

        if (sideDist(rng) == 0) {

            side = Side::BUY;
        }
        else {

            side = Side::SELL;
        }


        // ----------------------------------------------------
        // RANDOM ORDER TYPE
        // ----------------------------------------------------

        int typeValue =
            typeDist(rng);


        OrderType type;


        if (typeValue < 80) {

            type = OrderType::LIMIT;
        }
        else if (typeValue < 90) {

            type = OrderType::MARKET;
        }
        else if (typeValue < 95) {

            type = OrderType::IOC;
        }
        else {

            type = OrderType::FOK;
        }


        // ----------------------------------------------------
        // PRICE
        // ----------------------------------------------------

        int price =
            priceDist(rng);


        // Market orders don't need a meaningful price.

        if (type == OrderType::MARKET) {

            price = 0;
        }


        // ----------------------------------------------------
        // CREATE ORDER
        // ----------------------------------------------------

        Order order(
            i + 1,
            side,
            type,
            price,
            1,
            i + 1
        );


        // ----------------------------------------------------
        // MEASURE LATENCY
        // ----------------------------------------------------

        auto orderStart =
            Clock::now();


        engine.submitOrder(order);


        auto orderEnd =
            Clock::now();


        uint64_t ns =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                orderEnd - orderStart
            ).count();


        latency.record(ns);
    }


    // ========================================================
    // END TIMER
    // ========================================================

    auto end =
        Clock::now();


    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();


    double ordersPerSecond =
        static_cast<double>(NUM_ORDERS)
        / seconds;


    double tradesPerSecond =
        static_cast<double>(
            engine.getTrades().size()
        )
        / seconds;

    double tradesPerOrder =
        static_cast<double>(
            engine.getTrades().size()) /
        NUM_ORDERS;

    // ========================================================
    // RESULTS
    // ========================================================

    std::cout
        << "\n========================================\n";

    std::cout
        << "       MIXED WORKLOAD BENCHMARK\n";

    std::cout
        << "========================================\n\n";


    std::cout
        << "Orders: "
        << NUM_ORDERS
        << '\n';


    std::cout
        << "Trades: "
        << engine.getTrades().size()
        << '\n';

    std::cout
        << "Trades/order: "
        << tradesPerOrder
        << '\n';

    std::cout
        << "Time: "
        << seconds
        << " seconds\n";


    std::cout
        << "Orders/sec: "
        << ordersPerSecond
        << '\n';


    std::cout
        << "Trades/sec: "
        << tradesPerSecond
        << '\n';


    std::cout
        << "Average latency: "
        << latency.average()
        << " ns\n";


    std::cout
        << "P50 latency: "
        << latency.percentile(0.50)
        << " ns\n";


    std::cout
        << "P95 latency: "
        << latency.percentile(0.95)
        << " ns\n";


    std::cout
        << "P99 latency: "
        << latency.percentile(0.99)
        << " ns\n";

    std::cout
        << "P99.9 latency: "
        << latency.percentile(0.999)
        << " ns\n";

    std::cout
        << "Max latency: "
        << latency.max()
        << " ns\n";
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    benchmarkInsertion();

    benchmarkMatching();

    benchmarkMixedWorkload();

    return 0;
}