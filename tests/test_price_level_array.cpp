#include "PriceLevelArray.h"

#include <cassert>
#include <iostream>

int main()
{
    // -----------------------------
    // BUY price levels
    // -----------------------------

    PriceLevelArray bids(true, 90, 110);

    bids.activate(100, 1);
    assert(bids.bestPrice() == 100);

    bids.activate(105, 2);
    assert(bids.bestPrice() == 105);

    bids.activate(102, 3);
    assert(bids.bestPrice() == 105);

    assert(
        bids.level(105).head == 2
    );

    assert(
        bids.level(105).tail == 2
    );


    // -----------------------------
    // SELL price levels
    // -----------------------------

    PriceLevelArray asks(false, 90, 110);

    asks.activate(102, 4);
    assert(asks.bestPrice() == 102);

    asks.activate(100, 5);
    assert(asks.bestPrice() == 100);

    asks.activate(104, 6);
    assert(asks.bestPrice() == 100);


    // -----------------------------
    // Deactivation
    // -----------------------------

    bids.deactivate(105);

    assert(
        bids.bestPrice() == 102
    );

    bids.deactivate(102);

    assert(
        bids.bestPrice() == 100
    );

    bids.deactivate(100);

    assert(
        bids.bestPrice() == 0
    );


    asks.deactivate(100);

    assert(
        asks.bestPrice() == 102
    );

    asks.deactivate(102);

    assert(
        asks.bestPrice() == 104
    );

    asks.deactivate(104);

    assert(
        asks.bestPrice() == 0
    );


    // -----------------------------
    // Price bounds
    // -----------------------------

    assert(
        bids.level(90).active == false
    );

    assert(
        bids.level(110).active == false
    );


    std::cout
        << "PriceLevelArray tests passed.\n";

    return 0;
}
