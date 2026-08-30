#include "OrderBook.h"

#include <chrono>
#include <iostream>

using Clock = std::chrono::steady_clock;


// ============================================================
// EXISTING PRICE LEVEL BENCHMARK
// ============================================================

void benchmarkExistingPrice()
{
    constexpr int NUM_ORDERS = 100000;

    OrderBook book;

    auto start = Clock::now();

    for (int i = 0; i < NUM_ORDERS; ++i) {

        // Every order uses the SAME price.
        Order order(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            1,
            i + 1
        );

        book.addOrder(order);
    }

    auto end = Clock::now();

    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();

    double throughput =
        static_cast<double>(NUM_ORDERS)
        / seconds;

    std::cout
        << "\n========================================\n";

    std::cout
        << "   EXISTING PRICE LEVEL BENCHMARK\n";

    std::cout
        << "========================================\n\n";

    std::cout
        << "Orders: "
        << NUM_ORDERS
        << '\n';

    std::cout
        << "Price levels: 1\n";

    std::cout
        << "Time: "
        << seconds
        << " seconds\n";

    std::cout
        << "Throughput: "
        << throughput
        << " orders/sec\n";
}


// ============================================================
// NEW PRICE LEVEL BENCHMARK
// ============================================================

void benchmarkNewPrice()
{
    constexpr int NUM_ORDERS = 100000;

    OrderBook book;

    auto start = Clock::now();

    for (int i = 0; i < NUM_ORDERS; ++i) {

        // Every order gets a DIFFERENT price.
        Order order(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000 + i,
            1,
            i + 1
        );

        book.addOrder(order);
    }

    auto end = Clock::now();

    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();

    double throughput =
        static_cast<double>(NUM_ORDERS)
        / seconds;

    std::cout
        << "\n========================================\n";

    std::cout
        << "      NEW PRICE LEVEL BENCHMARK\n";

    std::cout
        << "========================================\n\n";

    std::cout
        << "Orders: "
        << NUM_ORDERS
        << '\n';

    std::cout
        << "Price levels: "
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
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    benchmarkExistingPrice();

    benchmarkNewPrice();

    return 0;
}