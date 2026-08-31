#pragma once

#include "OrderBook.h"
#include "Trade.h"
#include "Event.h"

#include <vector>
#include <cstddef>

class MatchingEngine {

private:
    OrderBook orderBook;
    std::vector<Event> events;

public:

    explicit MatchingEngine(
        std::size_t expectedEvents = 0
    );

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