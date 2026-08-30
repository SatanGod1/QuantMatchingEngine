#include "MatchingEngine.h"
#include "MarketData.h"

#include <iostream>

int main()
{
    MatchingEngine engine;

    // ============================================================
    // ADD SELL ORDERS
    // ============================================================

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10200,
            40,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::SELL,
            OrderType::LIMIT,
            10300,
            100,
            2
        )
    );

    // ============================================================
    // BUY ORDER
    // ============================================================

    engine.submitOrder(
        Order(
            3,
            Side::BUY,
            OrderType::LIMIT,
            10300,
            70,
            3
        )
    );

    // ============================================================
    // PRINT TRADES
    // ============================================================

    std::cout << "\nTRADES\n";
    std::cout << "======\n";

    const auto& trades =
        engine.getTrades();

    for (const auto& trade : trades)
    {
        std::cout
            << "Buy Order: " << trade.buyOrderId
            << " | Sell Order: " << trade.sellOrderId
            << " | Price: " << trade.price
            << " | Quantity: " << trade.quantity
            << '\n';
    }

    // ============================================================
    // PRINT EVENTS
    // ============================================================

    std::cout << "\nEVENTS\n";
    std::cout << "======\n";

    const auto& events =
        engine.getEvents();

    for (const auto& event : events)
    {
        std::cout
            << "Event type: "
            << static_cast<int>(event.type)
            << " | Order ID: "
            << event.orderId
            << " | Price: "
            << event.price
            << " | Quantity: "
            << event.quantity
            << '\n';
    }

    // ============================================================
    // PRINT BOOK
    // ============================================================

    std::cout << "\nFINAL BOOK\n";

    engine.getOrderBook().printBook();

    return 0;
}