#include "MatchingEngine.h"

#include <cassert>
#include <iostream>

void testFullMatch()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            50,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            50,
            2
        )
    );

    const auto& trades =
        engine.getTrades();

    assert(trades.size() == 1);
    assert(trades[0].quantity == 50);
    assert(trades[0].price == 10000);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);

    assert(!engine.getOrderBook().hasBids());
    assert(!engine.getOrderBook().hasAsks());
}

void testPartialFill()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            50,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            100,
            2
        )
    );

    const auto& trades =
        engine.getTrades();

    assert(trades.size() == 1);
    assert(trades[0].quantity == 50);

    assert(engine.getOrderBook().hasBids());
    assert(!engine.getOrderBook().hasAsks());

    assert(
        engine.getOrderBook().getBestBidPrice()
        == 10000
    );

    const auto& orders =
        engine.getOrderBook().getBestBidOrders();

    assert(orders.size() == 1);
    assert(orders.front().quantity == 50);
}

void testPricePriority()
{
    MatchingEngine engine;

    // Older/lower ask.
    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10100,
            50,
            1
        )
    );

    // Better ask.
    engine.submitOrder(
        Order(
            2,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            50,
            2
        )
    );

    engine.submitOrder(
        Order(
            3,
            Side::BUY,
            OrderType::LIMIT,
            10100,
            70,
            3
        )
    );

    const auto& trades =
        engine.getTrades();

    assert(trades.size() == 2);

    // Best ask first.
    assert(trades[0].sellOrderId == 2);
    assert(trades[0].price == 10000);
    assert(trades[0].quantity == 50);

    assert(trades[1].sellOrderId == 1);
    assert(trades[1].price == 10100);
    assert(trades[1].quantity == 20);
}

void testTimePriority()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            50,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            100,
            2
        )
    );

    engine.submitOrder(
        Order(
            3,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            75,
            3
        )
    );

    const auto& trades =
        engine.getTrades();

    assert(trades.size() == 2);

    // Earlier order gets filled first.
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].quantity == 50);

    assert(trades[1].sellOrderId == 2);
    assert(trades[1].quantity == 25);
}

void testNoMatch()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10500,
            50,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            50,
            2
        )
    );

    assert(engine.getTrades().empty());

    assert(engine.getOrderBook().hasBids());
    assert(engine.getOrderBook().hasAsks());

    assert(
        engine.getOrderBook().getBestBidPrice()
        == 10000
    );

    assert(
        engine.getOrderBook().getBestAskPrice()
        == 10500
    );
}

void testCancel()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::BUY,
            OrderType::LIMIT,
            10000,
            50,
            1
        )
    );

    assert(engine.getOrderBook().hasBids());

    bool result =
        engine.cancelOrder(1);

    assert(result);
    assert(!engine.getOrderBook().hasBids());

    // Cancelling again should fail.
    assert(!engine.cancelOrder(1));
}

void testMarketBuy()
{
    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            OrderType::LIMIT,
            10000,
            50,
            1
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::SELL,
            OrderType::LIMIT,
            10100,
            50,
            2
        )
    );

    engine.submitOrder(
        Order(
            3,
            Side::BUY,
            OrderType::MARKET,
            0,
            75,
            3
        )
    );

    const auto& trades =
        engine.getTrades();

    assert(trades.size() == 2);

    assert(trades[0].quantity == 50);
    assert(trades[0].price == 10000);

    assert(trades[1].quantity == 25);
    assert(trades[1].price == 10100);
}

int main()
{
    testFullMatch();
    testPartialFill();
    testPricePriority();
    testTimePriority();
    testNoMatch();
    testCancel();
    testMarketBuy();

    std::cout
        << "All tests passed!\n";

    return 0;
}