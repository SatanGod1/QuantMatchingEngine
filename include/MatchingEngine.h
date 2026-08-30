#pragma once

#include "OrderBook.h"
#include "Trade.h"
#include "Event.h"

#include <vector>

class MatchingEngine {

private:
    OrderBook orderBook;
    std::vector<Event> events;

public:

    std::vector<Trade> submitOrder(
        const Order& order
    );

    const std::vector<Event>& getEvents() const;

    bool cancelOrder(OrderId orderId);

    bool modifyOrder(
        OrderId orderId,
        int newPrice,
        int newQuantity
    );

    const OrderBook& getOrderBook() const;
};