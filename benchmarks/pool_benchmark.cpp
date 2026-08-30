#include "OrderPool.h"

#include <chrono>
#include <iostream>


using Clock = std::chrono::steady_clock;


int main()
{
    constexpr std::size_t NUM_ORDERS = 100000;


    OrderPool pool(NUM_ORDERS);


    auto start =
        Clock::now();


    for (std::size_t i = 0;
         i < NUM_ORDERS;
         ++i)
    {
        Order order(
            i + 1,
            Side::BUY,
            OrderType::LIMIT,
            10000 + (i % 100),
            1,
            i + 1
        );


        pool.create(order);
    }


    auto end =
        Clock::now();


    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();


    double throughput =
        static_cast<double>(
            NUM_ORDERS
        ) / seconds;


    std::cout
        << "\n========================================\n";

    std::cout
        << "          ORDER POOL BENCHMARK\n";

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
        << "Pool size: "
        << pool.size()
        << '\n';


    return 0;
}