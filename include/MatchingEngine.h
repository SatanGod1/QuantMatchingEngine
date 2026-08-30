#pragma once

#include "Event.h"
#include "Order.h"
#include "OrderBook.h"
#include "Trade.h"

#include <cstdint>
#include <vector>

class MatchingEngine
{
private:

    OrderBook orderBook;

    std::vector<Trade> trades;
    std::vector<Event> events;

    // ============================================================
    // MATCHING
    // ============================================================

    void matchBuy(Order& order);
    void matchSell(Order& order);

    // ============================================================
    // TRADE
    // ============================================================

    Trade executeTrade(
        const Order& buyOrder,
        const Order& sellOrder,
        int quantity
    );

public:

    MatchingEngine() = default;

    // ============================================================
    // SUBMIT
    // ============================================================

    void submitOrder(Order order);

    // ============================================================
    // CANCEL
    // ============================================================

    bool cancelOrder(uint64_t orderId);

    // ============================================================
    // ACCESS
    // ============================================================

    const std::vector<Trade>& getTrades() const;

    const std::vector<Event>& getEvents() const;

    const OrderBook& getOrderBook() const;

    OrderBook& getOrderBook();
};