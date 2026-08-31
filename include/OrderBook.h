#pragma once

#include "Order.h"
#include "OrderLocation.h"

#include <list>
#include <map>

class OrderBook {

private:

    std::map<
        int,
        std::list<Order>,
        std::greater<int>
    > bids;

    std::map<
        int,
        std::list<Order>
    > asks;

    std::unordered_map<OrderId, OrderLocation> orderIndex;

public:

    OrderBook();

    void addOrder(const Order &order);

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
        int newQuantity);
    
    bool validateInvariants() const;
};