#pragma once

#include "Order.h"
#include "OrderLocation.h"

#include <cstddef>
#include <functional>
#include <map>
#include <vector>

class OrderBook {

private:

    static constexpr std::size_t INVALID_INDEX =
        OrderLocation::INVALID_INDEX;

    struct OrderNode {

        Order order;

        std::size_t prev;

        std::size_t next;

        bool active;
    };

    /*
     * Price levels remain map-based in Phase 9.2.
     * We will optimize them separately later.
     */
    struct PriceLevel {

        std::size_t head;

        std::size_t tail;

        PriceLevel()
            : head(INVALID_INDEX),
              tail(INVALID_INDEX) {}
    };

    std::map<
        int,
        PriceLevel,
        std::greater<int>
    > bids;

    std::map<
        int,
        PriceLevel
    > asks;

    /*
     * Order storage.
     */
    std::vector<OrderNode> orderPool;

    std::vector<std::size_t> freeSlots;

    /*
     * Direct OrderId -> OrderLocation lookup.
     */
    std::vector<OrderLocation> orderIndex;

    /*
     * Cached best prices.
     */
    int highestBidPrice;

    int lowestAskPrice;

    std::size_t allocateOrder(
        const Order& order
    );

    void removeOrderFromLevel(
        std::size_t index
    );

    void ensureOrderIndexSize(
        OrderId orderId
    );

public:

    OrderBook();

    void addOrder(
        const Order& order
    );

    bool empty() const;

    bool hasBids() const;
    bool hasAsks() const;

    int bestBid() const;
    int bestAsk() const;

    Order& bestBidOrder();
    Order& bestAskOrder();

    void removeBestBidOrder();
    void removeBestAskOrder();

    void printBook() const;

    bool cancelOrder(
        OrderId orderId
    );

    bool modifyOrder(
        OrderId orderId,
        int newPrice,
        int newQuantity
    );

    bool validateInvariants() const;
};