#include "OrderBook.h"

#include <iostream>

OrderBook::OrderBook()
{
    orderIndex.reserve(1000000);

    // Reserve some initial order storage so small workloads
    // don't repeatedly grow the vector.
    orderPool.reserve(1000000);
}


std::size_t OrderBook::allocateOrder(const Order& order)
{
    std::size_t index;

    if (!freeSlots.empty()) {

        index = freeSlots.back();
        freeSlots.pop_back();

        orderPool[index].order = order;
        orderPool[index].prev = INVALID_INDEX;
        orderPool[index].next = INVALID_INDEX;
        orderPool[index].active = true;
    }
    else {

        index = orderPool.size();

        orderPool.push_back({
            order,
            INVALID_INDEX,
            INVALID_INDEX,
            true
        });
    }

    return index;
}


void OrderBook::addOrder(const Order& order)
{
    std::size_t index = allocateOrder(order);

    OrderNode& node = orderPool[index];

    if (order.side == Side::BUY) {

        auto priceIt = bids.find(order.price);

        if (priceIt == bids.end()) {

            priceIt = bids.emplace(
                order.price,
                PriceLevel{}
            ).first;
        }

        PriceLevel& level = priceIt->second;

        if (level.head == INVALID_INDEX) {

            level.head = index;
            level.tail = index;
        }
        else {

            orderPool[level.tail].next = index;
            node.prev = level.tail;

            level.tail = index;
        }
    }
    else {

        auto priceIt = asks.find(order.price);

        if (priceIt == asks.end()) {

            priceIt = asks.emplace(
                order.price,
                PriceLevel{}
            ).first;
        }

        PriceLevel& level = priceIt->second;

        if (level.head == INVALID_INDEX) {

            level.head = index;
            level.tail = index;
        }
        else {

            orderPool[level.tail].next = index;
            node.prev = level.tail;

            level.tail = index;
        }
    }

    orderIndex[order.id] = {
        order.side,
        order.price,
        index
    };
}


bool OrderBook::empty() const
{
    return bids.empty() && asks.empty();
}


bool OrderBook::hasBids() const
{
    return !bids.empty();
}


bool OrderBook::hasAsks() const
{
    return !asks.empty();
}


int OrderBook::bestBid() const
{
    if (bids.empty()) {
        return 0;
    }

    return bids.begin()->first;
}


int OrderBook::bestAsk() const
{
    if (asks.empty()) {
        return 0;
    }

    return asks.begin()->first;
}


Order& OrderBook::bestBidOrder()
{
    return orderPool[
        bids.begin()->second.head
    ].order;
}


Order& OrderBook::bestAskOrder()
{
    return orderPool[
        asks.begin()->second.head
    ].order;
}


void OrderBook::removeOrderFromLevel(
    std::size_t index
)
{
    OrderNode& node = orderPool[index];

    OrderId id = node.order.id;

    Side side = node.order.side;
    int price = node.order.price;

    std::size_t prev = node.prev;
    std::size_t next = node.next;

    if (side == Side::BUY) {

        auto priceIt = bids.find(price);

        if (priceIt == bids.end()) {
            return;
        }

        PriceLevel& level = priceIt->second;

        if (prev != INVALID_INDEX) {
            orderPool[prev].next = next;
        }
        else {
            level.head = next;
        }

        if (next != INVALID_INDEX) {
            orderPool[next].prev = prev;
        }
        else {
            level.tail = prev;
        }

        if (level.head == INVALID_INDEX) {
            bids.erase(priceIt);
        }
    }
    else {

        auto priceIt = asks.find(price);

        if (priceIt == asks.end()) {
            return;
        }

        PriceLevel& level = priceIt->second;

        if (prev != INVALID_INDEX) {
            orderPool[prev].next = next;
        }
        else {
            level.head = next;
        }

        if (next != INVALID_INDEX) {
            orderPool[next].prev = prev;
        }
        else {
            level.tail = prev;
        }

        if (level.head == INVALID_INDEX) {
            asks.erase(priceIt);
        }
    }

    orderIndex.erase(id);

    node.active = false;
    node.prev = INVALID_INDEX;
    node.next = INVALID_INDEX;

    freeSlots.push_back(index);
}


void OrderBook::removeBestBidOrder()
{
    if (bids.empty()) {
        return;
    }

    std::size_t index =
        bids.begin()->second.head;

    removeOrderFromLevel(index);
}


void OrderBook::removeBestAskOrder()
{
    if (asks.empty()) {
        return;
    }

    std::size_t index =
        asks.begin()->second.head;

    removeOrderFromLevel(index);
}


void OrderBook::printBook() const
{
    std::cout << "\n========== ORDER BOOK ==========\n";

    std::cout << "\nASKS\n";
    std::cout << "-----------------\n";

    for (const auto& [price, level] : asks) {

        int totalQuantity = 0;

        std::size_t index = level.head;

        while (index != INVALID_INDEX) {

            const OrderNode& node = orderPool[index];

            totalQuantity += node.order.quantity;

            index = node.next;
        }

        std::cout
            << price
            << " -> "
            << totalQuantity
            << '\n';
    }

    std::cout << "\nBIDS\n";
    std::cout << "-----------------\n";

    for (const auto& [price, level] : bids) {

        int totalQuantity = 0;

        std::size_t index = level.head;

        while (index != INVALID_INDEX) {

            const OrderNode& node = orderPool[index];

            totalQuantity += node.order.quantity;

            index = node.next;
        }

        std::cout
            << price
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

    std::size_t index = indexIt->second.index;

    removeOrderFromLevel(index);

    return true;
}


bool OrderBook::modifyOrder(
    OrderId orderId,
    int newPrice,
    int newQuantity
)
{
    auto indexIt = orderIndex.find(orderId);

    if (indexIt == orderIndex.end()) {
        return false;
    }

    if (newQuantity <= 0 || newPrice <= 0) {
        return false;
    }

    std::size_t index = indexIt->second.index;

    Order& currentOrder =
        orderPool[index].order;

    /*
     * Quantity decrease with unchanged price
     * preserves time priority.
     */
    if (
        newPrice == currentOrder.price &&
        newQuantity < currentOrder.quantity
    ) {

        currentOrder.quantity = newQuantity;

        return true;
    }

    /*
     * Price change or quantity increase:
     * remove and reinsert at the back of
     * the new price level.
     */

    Order updatedOrder(
        currentOrder.id,
        currentOrder.side,
        newPrice,
        newQuantity
    );

    removeOrderFromLevel(index);

    addOrder(updatedOrder);

    return true;
}


bool OrderBook::validateInvariants() const
{
    /*
     * Invariant 1:
     * Best bid must be strictly less than best ask.
     */

    if (!bids.empty() && !asks.empty()) {

        int bestBidPrice = bids.begin()->first;
        int bestAskPrice = asks.begin()->first;

        if (bestBidPrice >= bestAskPrice) {
            return false;
        }
    }


    /*
     * Invariant 2:
     * Validate all bid price levels and FIFO links.
     */

    for (const auto& [price, level] : bids) {

        if (price <= 0) {
            return false;
        }

        std::size_t index = level.head;

        std::size_t previous = INVALID_INDEX;

        while (index != INVALID_INDEX) {

            if (index >= orderPool.size()) {
                return false;
            }

            const OrderNode& node =
                orderPool[index];

            if (!node.active) {
                return false;
            }

            if (node.order.quantity <= 0) {
                return false;
            }

            if (node.order.side != Side::BUY) {
                return false;
            }

            if (node.order.price != price) {
                return false;
            }

            if (node.prev != previous) {
                return false;
            }

            previous = index;

            index = node.next;
        }

        if (level.tail != previous) {
            return false;
        }
    }


    /*
     * Invariant 3:
     * Validate all ask price levels and FIFO links.
     */

    for (const auto& [price, level] : asks) {

        if (price <= 0) {
            return false;
        }

        std::size_t index = level.head;

        std::size_t previous = INVALID_INDEX;

        while (index != INVALID_INDEX) {

            if (index >= orderPool.size()) {
                return false;
            }

            const OrderNode& node =
                orderPool[index];

            if (!node.active) {
                return false;
            }

            if (node.order.quantity <= 0) {
                return false;
            }

            if (node.order.side != Side::SELL) {
                return false;
            }

            if (node.order.price != price) {
                return false;
            }

            if (node.prev != previous) {
                return false;
            }

            previous = index;

            index = node.next;
        }

        if (level.tail != previous) {
            return false;
        }
    }


    /*
     * Invariant 4:
     * Every indexed order must point to an
     * active order with matching metadata.
     */

    for (const auto& [orderId, location] : orderIndex) {

        if (orderId == 0) {
            return false;
        }

        if (location.index >= orderPool.size()) {
            return false;
        }

        const OrderNode& node =
            orderPool[location.index];

        if (!node.active) {
            return false;
        }

        if (node.order.id != orderId) {
            return false;
        }

        if (node.order.side != location.side) {
            return false;
        }

        if (node.order.price != location.price) {
            return false;
        }
    }


    return true;
}