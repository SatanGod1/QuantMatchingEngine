#include "OrderBook.h"

#include <iostream>

OrderBook::OrderBook() {
    orderIndex.reserve(1000000);
}

void OrderBook::addOrder(const Order &order)
{

    if (order.side == Side::BUY)
    {

        auto &orders = bids[order.price];
        orders.push_back(order);

        auto it = orders.end();
        --it;

        orderIndex[order.id] = {
            order.side,
            order.price,
            it};
    }
    else
    {

        auto &orders = asks[order.price];
        orders.push_back(order);

        auto it = orders.end();
        --it;

        orderIndex[order.id] = {
            order.side,
            order.price,
            it};
    }
}

bool OrderBook::empty() const {

    return bids.empty() && asks.empty();
}

bool OrderBook::hasBids() const {

    return !bids.empty();
}

bool OrderBook::hasAsks() const {

    return !asks.empty();
}

int OrderBook::bestBid() const {

    if (bids.empty()) {
        return 0;
    }

    return bids.begin()->first;
}

int OrderBook::bestAsk() const {

    if (asks.empty()) {
        return 0;
    }

    return asks.begin()->first;
}

Order& OrderBook::bestBidOrder() {

    return bids.begin()->second.front();
}

Order& OrderBook::bestAskOrder() {

    return asks.begin()->second.front();
}

void OrderBook::removeBestBidOrder() {

    auto& orders = bids.begin()->second;

    OrderId id = orders.front().id;

    orders.pop_front();

    orderIndex.erase(id);

    if (orders.empty()) {
        bids.erase(bids.begin());
    }
}

void OrderBook::removeBestAskOrder() {

    auto& orders = asks.begin()->second;

    OrderId id = orders.front().id;

    orders.pop_front();

    orderIndex.erase(id);

    if (orders.empty()) {
        asks.erase(asks.begin());
    }
}

void OrderBook::printBook() const {

    std::cout << "\n========== ORDER BOOK ==========\n";

    std::cout << "\nASKS\n";
    std::cout << "-----------------\n";

    for (const auto& [price, orders] : asks) {

        int totalQuantity = 0;

        for (const auto& order : orders) {
            totalQuantity += order.quantity;
        }

        std::cout << price
                  << " -> "
                  << totalQuantity
                  << '\n';
    }

    std::cout << "\nBIDS\n";
    std::cout << "-----------------\n";

    for (const auto& [price, orders] : bids) {

        int totalQuantity = 0;

        for (const auto& order : orders) {
            totalQuantity += order.quantity;
        }

        std::cout << price
                  << " -> "
                  << totalQuantity
                  << '\n';
    }

    std::cout << "\n================================\n";
}

bool OrderBook::cancelOrder(OrderId orderId)
{
    auto indexIt = orderIndex.find(orderId);

    if (indexIt == orderIndex.end()) {
        return false;
    }

    OrderLocation location = indexIt->second;

    if (location.side == Side::BUY) {

        auto priceIt = bids.find(location.price);

        if (priceIt == bids.end()) {
            return false;
        }

        auto& orders = priceIt->second;

        orders.erase(location.position);

        if (orders.empty()) {
            bids.erase(priceIt);
        }
    }
    else {

        auto priceIt = asks.find(location.price);

        if (priceIt == asks.end()) {
            return false;
        }

        auto& orders = priceIt->second;

        orders.erase(location.position);

        if (orders.empty()) {
            asks.erase(priceIt);
        }
    }

    orderIndex.erase(indexIt);

    return true;
}

bool OrderBook::modifyOrder(
    OrderId orderId,
    int newPrice,
    int newQuantity
) {
    auto indexIt = orderIndex.find(orderId);

    if (indexIt == orderIndex.end()) {
        return false;
    }

    if (newQuantity <= 0 || newPrice <= 0) {
        return false;
    }

    OrderLocation location = indexIt->second;

    // BUY
    if (location.side == Side::BUY) {

        auto priceIt = bids.find(location.price);

        if (priceIt == bids.end()) {
            return false;
        }

        auto& orders = priceIt->second;
        auto it = location.position;

        // Quantity decreased and price unchanged.
        // Preserve time priority.
        if (
            newPrice == it->price &&
            newQuantity < it->quantity
        ) {
            it->quantity = newQuantity;
            return true;
        }

        // Otherwise remove and reinsert.
        Order updatedOrder(
            it->id,
            it->side,
            newPrice,
            newQuantity
        );

        orders.erase(it);

        if (orders.empty()) {
            bids.erase(priceIt);
        }

        orderIndex.erase(orderId);

        addOrder(updatedOrder);

        return true;
    }

    // SELL
    else {

        auto priceIt = asks.find(location.price);

        if (priceIt == asks.end()) {
            return false;
        }

        auto& orders = priceIt->second;
        auto it = location.position;

        // Quantity decreased and price unchanged.
        // Preserve time priority.
        if (
            newPrice == it->price &&
            newQuantity < it->quantity
        ) {
            it->quantity = newQuantity;
            return true;
        }

        // Otherwise remove and reinsert.
        Order updatedOrder(
            it->id,
            it->side,
            newPrice,
            newQuantity
        );

        orders.erase(it);

        if (orders.empty()) {
            asks.erase(priceIt);
        }

        orderIndex.erase(orderId);

        addOrder(updatedOrder);

        return true;
    }
}

bool OrderBook::validateInvariants() const {

    // --------------------------------------------------------
    // Invariant 1:
    // If both sides exist, best bid must be less than best ask.
    // --------------------------------------------------------

    if (!bids.empty() && !asks.empty()) {

        int bestBidPrice = bids.begin()->first;
        int bestAskPrice = asks.begin()->first;

        if (bestBidPrice >= bestAskPrice) {
            return false;
        }
    }


    // --------------------------------------------------------
    // Invariant 2:
    // Every bid order must have positive quantity.
    // --------------------------------------------------------

    for (const auto& [price, orders] : bids) {

        if (price <= 0) {
            return false;
        }

        for (const auto& order : orders) {

            if (order.quantity <= 0) {
                return false;
            }

            if (order.side != Side::BUY) {
                return false;
            }

            if (order.price != price) {
                return false;
            }
        }
    }


    // --------------------------------------------------------
    // Invariant 3:
    // Every ask order must have positive quantity.
    // --------------------------------------------------------

    for (const auto& [price, orders] : asks) {

        if (price <= 0) {
            return false;
        }

        for (const auto& order : orders) {

            if (order.quantity <= 0) {
                return false;
            }

            if (order.side != Side::SELL) {
                return false;
            }

            if (order.price != price) {
                return false;
            }
        }
    }


    // --------------------------------------------------------
    // Invariant 4:
    // Every indexed order must have a valid location.
    // --------------------------------------------------------

    for (const auto& [orderId, location] : orderIndex) {

        if (orderId == 0) {
            return false;
        }
    }


    return true;
}