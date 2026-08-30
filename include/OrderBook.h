#pragma once

#include "Order.h"

#include <cstdint>
#include <deque>
#include <map>
#include <vector>

class OrderBook
{
public:

    using OrderQueue = std::deque<Order>;

    struct PriceLevelInfo
    {
        int price;
        int quantity;
    };

private:

    // Highest bid first.
    std::map<int, OrderQueue, std::greater<int>> bids;

    // Lowest ask first.
    std::map<int, OrderQueue> asks;

public:

    OrderBook() = default;

    // ============================================================
    // ADD ORDER
    // ============================================================

    void addOrder(const Order& order);

    // ============================================================
    // CANCEL ORDER
    // ============================================================

    bool cancelOrder(uint64_t orderId);

    // ============================================================
    // BEST PRICES
    // ============================================================

    bool hasBids() const;
    bool hasAsks() const;

    int getBestBidPrice() const;
    int getBestAskPrice() const;

    // ============================================================
    // BEST PRICE ORDERS
    // ============================================================

    OrderQueue& getBestBidOrders();
    const OrderQueue& getBestBidOrders() const;

    OrderQueue& getBestAskOrders();
    const OrderQueue& getBestAskOrders() const;

    // ============================================================
    // REMOVE BEST LEVEL
    // ============================================================

    void removeBestBid();
    void removeBestAsk();

    // ============================================================
    // MARKET DATA
    // ============================================================

    std::vector<PriceLevelInfo>
    getAllBidLevels() const;

    std::vector<PriceLevelInfo>
    getAllAskLevels() const;

    std::vector<PriceLevelInfo>
    getBidLevels(std::size_t depth) const;

    std::vector<PriceLevelInfo>
    getAskLevels(std::size_t depth) const;

    // ============================================================
    // DEBUG
    // ============================================================

    void printBook() const;

    // ============================================================
    // SIZE
    // ============================================================

    std::size_t bidLevelCount() const;
    std::size_t askLevelCount() const;
};