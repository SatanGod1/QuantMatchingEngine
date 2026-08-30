#include "MatchingEngine.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace std;


// ============================================================
// Helper functions
// ============================================================

int countEvent(
    const vector<Event>& events,
    EventType type
) {
    int count = 0;

    for (const auto& event : events) {
        if (event.type == type) {
            count++;
        }
    }

    return count;
}


// ============================================================
// Test 1: Basic limit order
// ============================================================

void testLimitOrderAdded() {

    MatchingEngine engine;

    Order buy(
        1,
        Side::BUY,
        100,
        50
    );

    auto trades = engine.submitOrder(buy);

    assert(trades.empty());

    const auto& events = engine.getEvents();

    assert(
        countEvent(events, EventType::ORDER_ACCEPTED) == 1
    );

    assert(
        countEvent(events, EventType::ORDER_ADDED) == 1
    );

    assert(
        engine.getOrderBook().hasBids()
    );

    assert(
        engine.getOrderBook().bestBid() == 100
    );

    cout << "testLimitOrderAdded PASSED\n";
}


// ============================================================
// Test 2: Price-time priority
// ============================================================

void testPriceTimePriority() {

    MatchingEngine engine;

    // Earlier order at same price
    Order buy1(
        1,
        Side::BUY,
        100,
        100
    );

    Order buy2(
        2,
        Side::BUY,
        100,
        100
    );

    engine.submitOrder(buy1);
    engine.submitOrder(buy2);

    // Incoming sell
    Order sell(
        3,
        Side::SELL,
        100,
        150
    );

    auto trades = engine.submitOrder(sell);

    assert(trades.size() == 2);

    // First order must execute first
    assert(trades[0].buyOrderId == 1);
    assert(trades[0].sellOrderId == 3);
    assert(trades[0].quantity == 100);

    // Second order gets remaining 50
    assert(trades[1].buyOrderId == 2);
    assert(trades[1].sellOrderId == 3);
    assert(trades[1].quantity == 50);

    cout << "testPriceTimePriority PASSED\n";
}


// ============================================================
// Test 3: Price priority
// ============================================================

void testPricePriority() {

    MatchingEngine engine;

    Order buy1(
        1,
        Side::BUY,
        100,
        100
    );

    Order buy2(
        2,
        Side::BUY,
        101,
        100
    );

    engine.submitOrder(buy1);
    engine.submitOrder(buy2);

    Order sell(
        3,
        Side::SELL,
        100,
        150
    );

    auto trades = engine.submitOrder(sell);

    assert(trades.size() == 2);

    // Higher bid gets priority
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].quantity == 100);

    assert(trades[1].buyOrderId == 1);
    assert(trades[1].quantity == 50);

    cout << "testPricePriority PASSED\n";
}


// ============================================================
// Test 4: Partial fill
// ============================================================

void testPartialFill() {

    MatchingEngine engine;

    Order sell(
        1,
        Side::SELL,
        100,
        100
    );

    engine.submitOrder(sell);

    Order buy(
        2,
        Side::BUY,
        100,
        40
    );

    auto trades = engine.submitOrder(buy);

    assert(trades.size() == 1);

    assert(trades[0].quantity == 40);

    // 60 should remain on ask side
    assert(engine.getOrderBook().hasAsks());

    assert(engine.getOrderBook().bestAsk() == 100);

    cout << "testPartialFill PASSED\n";
}


// ============================================================
// Test 5: Multiple price levels
// ============================================================

void testMultiplePriceLevels() {

    MatchingEngine engine;

    Order sell1(
        1,
        Side::SELL,
        100,
        50
    );

    Order sell2(
        2,
        Side::SELL,
        101,
        50
    );

    Order sell3(
        3,
        Side::SELL,
        102,
        50
    );

    engine.submitOrder(sell1);
    engine.submitOrder(sell2);
    engine.submitOrder(sell3);

    Order buy(
        4,
        Side::BUY,
        102,
        120
    );

    auto trades = engine.submitOrder(buy);

    assert(trades.size() == 3);

    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    assert(trades[1].price == 101);
    assert(trades[1].quantity == 50);

    assert(trades[2].price == 102);
    assert(trades[2].quantity == 20);

    cout << "testMultiplePriceLevels PASSED\n";
}


// ============================================================
// Test 6: Market order
// ============================================================

void testMarketOrder() {

    MatchingEngine engine;

    Order sell1(
        1,
        Side::SELL,
        100,
        50
    );

    Order sell2(
        2,
        Side::SELL,
        101,
        100
    );

    engine.submitOrder(sell1);
    engine.submitOrder(sell2);

    Order marketBuy(
        3,
        Side::BUY,
        OrderType::MARKET,
        0,
        120
    );

    auto trades = engine.submitOrder(marketBuy);

    assert(trades.size() == 2);

    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    assert(trades[1].price == 101);
    assert(trades[1].quantity == 70);

    // 30 should remain at 101
    assert(engine.getOrderBook().hasAsks());

    assert(engine.getOrderBook().bestAsk() == 101);

    cout << "testMarketOrder PASSED\n";
}


// ============================================================
// Test 7: Market order does not rest
// ============================================================

void testMarketOrderDoesNotRest() {

    MatchingEngine engine;

    Order marketBuy(
        1,
        Side::BUY,
        OrderType::MARKET,
        0,
        100
    );

    auto trades = engine.submitOrder(marketBuy);

    assert(trades.empty());

    // Market order must not enter the book
    assert(!engine.getOrderBook().hasBids());

    assert(!engine.getOrderBook().hasAsks());

    cout << "testMarketOrderDoesNotRest PASSED\n";
}


// ============================================================
// Test 8: Cancellation
// ============================================================

void testCancellation() {

    MatchingEngine engine;

    Order buy(
        1,
        Side::BUY,
        100,
        100
    );

    engine.submitOrder(buy);

    assert(engine.getOrderBook().hasBids());

    bool result = engine.cancelOrder(1);

    assert(result);

    assert(!engine.getOrderBook().hasBids());

    // Cancelling again should fail
    assert(!engine.cancelOrder(1));

    cout << "testCancellation PASSED\n";
}


// ============================================================
// Test 9: Modification
// ============================================================

void testModification() {

    MatchingEngine engine;

    Order buy(
        1,
        Side::BUY,
        100,
        100
    );

    engine.submitOrder(buy);

    bool result =
        engine.modifyOrder(
            1,
            101,
            150
        );

    assert(result);

    assert(engine.getOrderBook().hasBids());

    assert(engine.getOrderBook().bestBid() == 101);

    cout << "testModification PASSED\n";
}


// ============================================================
// Test 10: Invalid order ID
// ============================================================

void testInvalidOrderId() {

    MatchingEngine engine;

    Order invalid(
        0,
        Side::BUY,
        100,
        100
    );

    auto trades = engine.submitOrder(invalid);

    assert(trades.empty());

    const auto& events = engine.getEvents();

    assert(
        countEvent(events, EventType::ORDER_REJECTED) == 1
    );

    assert(
        !engine.getOrderBook().hasBids()
    );

    cout << "testInvalidOrderId PASSED\n";
}


// ============================================================
// Test 11: Invalid quantity
// ============================================================

void testInvalidQuantity() {

    MatchingEngine engine;

    Order invalid(
        1,
        Side::BUY,
        100,
        0
    );

    auto trades = engine.submitOrder(invalid);

    assert(trades.empty());

    const auto& events = engine.getEvents();

    assert(
        countEvent(events, EventType::ORDER_REJECTED) == 1
    );

    cout << "testInvalidQuantity PASSED\n";
}


// ============================================================
// Test 12: Invalid limit price
// ============================================================

void testInvalidLimitPrice() {

    MatchingEngine engine;

    Order invalid(
        1,
        Side::BUY,
        0,
        100
    );

    auto trades = engine.submitOrder(invalid);

    assert(trades.empty());

    const auto& events = engine.getEvents();

    assert(
        countEvent(events, EventType::ORDER_REJECTED) == 1
    );

    cout << "testInvalidLimitPrice PASSED\n";
}


// ============================================================
// Test 13: Invalid market order price
// ============================================================

void testInvalidMarketPrice() {

    MatchingEngine engine;

    Order invalid(
        1,
        Side::BUY,
        OrderType::MARKET,
        100,
        100
    );

    auto trades = engine.submitOrder(invalid);

    assert(trades.empty());

    const auto& events = engine.getEvents();

    assert(
        countEvent(events, EventType::ORDER_REJECTED) == 1
    );

    cout << "testInvalidMarketPrice PASSED\n";
}

void testInitialInvariants() {

    MatchingEngine engine;

    assert(
        engine.getOrderBook().validateInvariants()
    );

    cout << "testInitialInvariants PASSED\n";
}

void testInvariantsAfterOrders() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::BUY,
            100,
            100
        )
    );

    assert(
        engine.getOrderBook().validateInvariants()
    );


    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            101,
            100
        )
    );

    assert(
        engine.getOrderBook().validateInvariants()
    );


    engine.submitOrder(
        Order(
            3,
            Side::SELL,
            102,
            100
        )
    );

    assert(
        engine.getOrderBook().validateInvariants()
    );


    cout << "testInvariantsAfterOrders PASSED\n";
}

void testInvariantsAfterMatching() {

    MatchingEngine engine;

    engine.submitOrder(
        Order(
            1,
            Side::SELL,
            100,
            100
        )
    );

    engine.submitOrder(
        Order(
            2,
            Side::BUY,
            100,
            50
        )
    );

    assert(
        engine.getOrderBook().validateInvariants()
    );


    engine.submitOrder(
        Order(
            3,
            Side::BUY,
            100,
            100
        )
    );

    assert(
        engine.getOrderBook().validateInvariants()
    );


    cout << "testInvariantsAfterMatching PASSED\n";
}


// ============================================================
// Main
// ============================================================

int main() {

    testLimitOrderAdded();
    testPriceTimePriority();
    testPricePriority();
    testPartialFill();
    testMultiplePriceLevels();
    testMarketOrder();
    testMarketOrderDoesNotRest();
    testCancellation();
    testModification();
    testInvalidOrderId();
    testInvalidQuantity();
    testInvalidLimitPrice();
    testInvalidMarketPrice();
    testInitialInvariants();
    testInvariantsAfterOrders();
    testInvariantsAfterMatching();

    cout << "\n====================================\n";
    cout << "ALL EDGE CASE TESTS PASSED\n";
    cout << "====================================\n";

    return 0;
}