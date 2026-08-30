#include "MatchingEngine.h"

#include <iostream>

int main() {

    MatchingEngine engine;

    // Add initial liquidity
    engine.submitOrder(
        Order(1, Side::SELL, 102, 40)
    );

    engine.submitOrder(
        Order(2, Side::SELL, 103, 100)
    );

    engine.submitOrder(
        Order(3, Side::BUY, 101, 50)
    );

    std::cout << "Initial Order Book:\n";

    engine.getOrderBook().printBook();

    // Incoming order
    auto trades = engine.submitOrder(
        Order(4, Side::BUY, 103, 70)
    );

    std::cout << "\nTrades:\n";

    for (const auto& trade : trades) {

        std::cout
            << "BUY Order "
            << trade.buyOrderId
            << " matched SELL Order "
            << trade.sellOrderId
            << " -> "
            << trade.quantity
            << " @ "
            << trade.price
            << '\n';
    }

    std::cout << "\nFinal Order Book:\n";

    engine.getOrderBook().printBook();

    return 0;
}