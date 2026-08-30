#pragma once

#include "OrderBook.h"
#include "Trade.h"

#include <vector>

class MatchingEngine {

private:
    OrderBook orderBook;

public:

    std::vector<Trade> submitOrder(
        const Order& order
    );

    bool cancelOrder(OrderId orderId);

    bool modifyOrder(
        OrderId orderId,
        int newPrice,
        int newQuantity
    );

    const OrderBook& getOrderBook() const;
};