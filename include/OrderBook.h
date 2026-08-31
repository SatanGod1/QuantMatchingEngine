#pragma once

#include "Order.h"
#include "OrderLocation.h"

#include <cstddef>
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

class OrderBook {

private:

    static constexpr std::size_t INVALID_INDEX =
        static_cast<std::size_t>(-1);

    struct OrderNode {
        Order order;
        std::size_t prev;
        std::size_t next;
        bool active;
    };

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

    std::vector<OrderNode> orderPool;

    std::vector<std::size_t> freeSlots;

    std::unordered_map<OrderId, OrderLocation> orderIndex;

    std::size_t allocateOrder(const Order& order);

    void removeOrderFromLevel(
        std::size_t index
    );

public:

    OrderBook();

    void addOrder(const Order& order);

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

    bool cancelOrder(OrderId orderId);

    bool modifyOrder(
        OrderId orderId,
        int newPrice,
        int newQuantity
    );

    bool validateInvariants() const;
};