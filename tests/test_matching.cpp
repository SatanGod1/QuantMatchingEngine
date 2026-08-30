#include "MatchingEngine.h"
#include "Event.h"

#include <cassert>
#include <iostream>

void testBasicMatch() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::SELL, 102, 100)
    );

    auto trades = engine.submitOrder(
        Order(2, Side::BUY, 102, 100)
    );

    assert(trades.size() == 1);

    assert(trades[0].quantity == 100);
    assert(trades[0].price == 102);

    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);

    std::cout << "testBasicMatch PASSED\n";
}

void testPartialFill() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::SELL, 102, 100)
    );

    auto trades = engine.submitOrder(
        Order(2, Side::BUY, 102, 40)
    );

    assert(trades.size() == 1);

    assert(trades[0].quantity == 40);
    assert(trades[0].price == 102);

    assert(
        engine.getOrderBook().bestAsk() == 102
    );

    std::cout << "testPartialFill PASSED\n";
}

void testMultiplePriceLevels() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::SELL, 102, 40)
    );

    engine.submitOrder(
        Order(2, Side::SELL, 103, 100)
    );

    auto trades = engine.submitOrder(
        Order(3, Side::BUY, 103, 70)
    );

    assert(trades.size() == 2);

    assert(trades[0].price == 102);
    assert(trades[0].quantity == 40);

    assert(trades[1].price == 103);
    assert(trades[1].quantity == 30);

    assert(
        engine.getOrderBook().bestAsk() == 103
    );

    std::cout << "testMultiplePriceLevels PASSED\n";
}

void testFIFO() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::SELL, 102, 100)
    );

    engine.submitOrder(
        Order(2, Side::SELL, 102, 200)
    );

    engine.submitOrder(
        Order(3, Side::SELL, 102, 300)
    );

    auto trades = engine.submitOrder(
        Order(4, Side::BUY, 102, 150)
    );

    assert(trades.size() == 2);

    assert(trades[0].sellOrderId == 1);
    assert(trades[0].quantity == 100);

    assert(trades[1].sellOrderId == 2);
    assert(trades[1].quantity == 50);

    std::cout << "testFIFO PASSED\n";
}

void testNoMatch() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::SELL, 105, 100)
    );

    auto trades = engine.submitOrder(
        Order(2, Side::BUY, 104, 100)
    );

    assert(trades.empty());

    assert(
        engine.getOrderBook().bestAsk() == 105
    );

    assert(
        engine.getOrderBook().bestBid() == 104
    );

    std::cout << "testNoMatch PASSED\n";
}

void testCancellation() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::BUY, 101, 100)
    );

    engine.submitOrder(
        Order(2, Side::BUY, 101, 200)
    );

    engine.submitOrder(
        Order(3, Side::BUY, 100, 300)
    );

    bool cancelled =
        engine.cancelOrder(2);

    assert(cancelled);

    assert(
        engine.getOrderBook().bestBid() == 101
    );

    std::cout << "testCancellation PASSED\n";
}

void testModifyQuantityDecrease() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::BUY, 101, 100)
    );

    engine.submitOrder(
        Order(2, Side::BUY, 101, 200)
    );

    bool modified =
        engine.modifyOrder(2, 101, 150);

    assert(modified);

    std::cout
        << "testModifyQuantityDecrease PASSED\n";
}

void testModifyPrice() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::BUY, 101, 100)
    );

    bool modified =
        engine.modifyOrder(1, 102, 100);

    assert(modified);

    assert(
        engine.getOrderBook().bestBid() == 102
    );

    std::cout
        << "testModifyPrice PASSED\n";
}

void testModifyQuantityIncrease() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::BUY, 101, 100)
    );

    bool modified =
        engine.modifyOrder(1, 101, 200);

    assert(modified);

    assert(
        engine.getOrderBook().bestBid() == 101
    );

    std::cout
        << "testModifyQuantityIncrease PASSED\n";
}

void testInvalidModify() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(1, Side::BUY, 101, 100)
    );

    bool modified =
        engine.modifyOrder(999, 102, 100);

    assert(!modified);

    std::cout
        << "testInvalidModify PASSED\n";
}

void testMarketBuy() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            102,
            40
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::SELL,
            103,
            100
        )
    );

    auto trades =
        engine.submitOrder(
            Order(
                3,
                Side::BUY,
                OrderType::MARKET,
                0,
                70
            )
        );

    assert(trades.size() == 2);

    assert(trades[0].price == 102);
    assert(trades[0].quantity == 40);

    assert(trades[1].price == 103);
    assert(trades[1].quantity == 30);

    assert(
        engine.getOrderBook().bestAsk() == 103
    );

    std::cout
        << "testMarketBuy PASSED\n";
}

void testMarketSell() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::BUY,
            103,
            40
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            102,
            100
        )
    );

    auto trades =
        engine.submitOrder(
            Order(
                3,
                Side::SELL,
                OrderType::MARKET,
                0,
                70
            )
        );

    assert(trades.size() == 2);

    assert(trades[0].price == 103);
    assert(trades[0].quantity == 40);

    assert(trades[1].price == 102);
    assert(trades[1].quantity == 30);

    assert(
        engine.getOrderBook().bestBid() == 102
    );

    std::cout
        << "testMarketSell PASSED\n";
}

void testMarketOrderInsufficientLiquidity() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            102,
            40
        )
    );

    auto trades =
        engine.submitOrder(
            Order(
                2,
                Side::BUY,
                OrderType::MARKET,
                0,
                100
            )
        );

    assert(trades.size() == 1);

    assert(trades[0].quantity == 40);

    // No asks remain.
    assert(
        !engine.getOrderBook().hasAsks()
    );

    // The unfilled market quantity
    // must NOT become a bid.
    assert(
        !engine.getOrderBook().hasBids()
    );

    std::cout
        << "testMarketOrderInsufficientLiquidity PASSED\n";
}



int main() {

    testBasicMatch();
    testPartialFill();
    testMultiplePriceLevels();
    testFIFO();
    testNoMatch();
    testCancellation();

    testModifyQuantityDecrease();
    testModifyPrice();
    testModifyQuantityIncrease();
    testInvalidModify();

    testMarketBuy();
    testMarketSell();
    testMarketOrderInsufficientLiquidity();

    std::cout
        << "\nAll tests PASSED!\n";

    return 0;
}