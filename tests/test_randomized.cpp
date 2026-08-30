#include "MatchingEngine.h"

#include <cassert>
#include <iostream>
#include <random>

using namespace std;

void testRandomLimitOrders() {

    MatchingEngine engine;

    // Fixed seed = reproducible tests
    mt19937 rng(42);

    uniform_int_distribution<int> sideDist(0, 1);
    uniform_int_distribution<int> priceDist(90, 110);
    uniform_int_distribution<int> quantityDist(1, 100);

    const int NUM_ORDERS = 10000;

    for (OrderId id = 1; id <= NUM_ORDERS; ++id) {

        Side side =
            sideDist(rng) == 0
                ? Side::BUY
                : Side::SELL;

        int price = priceDist(rng);
        int quantity = quantityDist(rng);

        Order order(
            id,
            side,
            price,
            quantity
        );

        engine.submitOrder(order);

        // The order book must remain valid
        assert(
            engine.getOrderBook().validateInvariants()
        );
    }

    cout << "testRandomLimitOrders PASSED\n";
}


void testRandomMarketOrders() {

    MatchingEngine engine;

    mt19937 rng(12345);

    uniform_int_distribution<int> sideDist(0, 1);
    uniform_int_distribution<int> priceDist(90, 110);
    uniform_int_distribution<int> quantityDist(1, 100);

    const int NUM_ORDERS = 10000;

    OrderId id = 1;

    for (int i = 0; i < NUM_ORDERS; ++i) {

        Side side =
            sideDist(rng) == 0
                ? Side::BUY
                : Side::SELL;

        int quantity = quantityDist(rng);

        // First create some liquidity using limit orders.
        if (i < 5000) {

            int price = priceDist(rng);

            engine.submitOrder(
                Order(
                    id++,
                    side,
                    price,
                    quantity
                )
            );
        }

        // Then send market orders.
        else {

            engine.submitOrder(
                Order(
                    id++,
                    side,
                    OrderType::MARKET,
                    0,
                    quantity
                )
            );
        }

        assert(
            engine.getOrderBook().validateInvariants()
        );
    }

    cout << "testRandomMarketOrders PASSED\n";
}


void testRandomMatching() {

    MatchingEngine engine;

    mt19937 rng(999);

    uniform_int_distribution<int> quantityDist(1, 50);

    OrderId id = 1;

    // Build a deterministic market first.
    for (int i = 0; i < 1000; ++i) {

        int quantity = quantityDist(rng);

        engine.submitOrder(
            Order(
                id++,
                Side::SELL,
                105,
                quantity
            )
        );

        assert(
            engine.getOrderBook().validateInvariants()
        );
    }

    // Send aggressive BUY orders.
    for (int i = 0; i < 1000; ++i) {

        int quantity = quantityDist(rng);

        engine.submitOrder(
            Order(
                id++,
                Side::BUY,
                105,
                quantity
            )
        );

        assert(
            engine.getOrderBook().validateInvariants()
        );
    }

    cout << "testRandomMatching PASSED\n";
}


int main() {

    testRandomLimitOrders();

    testRandomMarketOrders();

    testRandomMatching();

    cout << "\nAll randomized tests passed!\n";

    return 0;
}