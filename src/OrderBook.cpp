#include "OrderBook.h"

#include <iostream>


OrderBook::OrderBook()
    : highestBidPrice(-1),
      lowestAskPrice(-1)
{
    /*
     * IDs are 1-based.
     *
     * Start with enough space for the current
     * development workload. The vector grows
     * automatically if a larger ID appears.
     */
    orderIndex.resize(10001);

    /*
     * Reserve order storage for the current
     * development workload.
     */
    orderPool.reserve(10000);
}


/*
 * Make sure an OrderId can be used as a direct
 * vector index.
 */
void OrderBook::ensureOrderIndexSize(
    OrderId orderId
)
{
    if (orderId >= orderIndex.size()) {

        orderIndex.resize(
            static_cast<std::size_t>(orderId) + 1
        );
    }
}


std::size_t OrderBook::allocateOrder(
    const Order& order
)
{
    std::size_t index;

    if (!freeSlots.empty()) {

        index = freeSlots.back();
        freeSlots.pop_back();

        orderPool[index].order = order;

        orderPool[index].prev =
            INVALID_INDEX;

        orderPool[index].next =
            INVALID_INDEX;

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


void OrderBook::addOrder(
    const Order& order
)
{
    /*
     * We need a valid direct-index entry.
     */
    ensureOrderIndexSize(order.id);

    const std::size_t index =
        allocateOrder(order);

    OrderNode& node =
        orderPool[index];


    /*
     * BUY side.
     */
    if (order.side == Side::BUY) {

        auto priceIt =
            bids.find(order.price);

        if (priceIt == bids.end()) {

            priceIt =
                bids.emplace(
                    order.price,
                    PriceLevel{}
                ).first;
        }

        PriceLevel& level =
            priceIt->second;

        /*
         * First order at this price.
         */
        if (level.head == INVALID_INDEX) {

            level.head = index;
            level.tail = index;
        }

        /*
         * Append to FIFO queue.
         */
        else {

            orderPool[level.tail].next =
                index;

            node.prev =
                level.tail;

            level.tail =
                index;
        }

        /*
         * Update best bid.
         */
        if (
            highestBidPrice == -1 ||
            order.price > highestBidPrice
        ) {

            highestBidPrice =
                order.price;
        }
    }


    /*
     * SELL side.
     */
    else {

        auto priceIt =
            asks.find(order.price);

        if (priceIt == asks.end()) {

            priceIt =
                asks.emplace(
                    order.price,
                    PriceLevel{}
                ).first;
        }

        PriceLevel& level =
            priceIt->second;

        /*
         * First order at this price.
         */
        if (level.head == INVALID_INDEX) {

            level.head = index;
            level.tail = index;
        }

        /*
         * Append to FIFO queue.
         */
        else {

            orderPool[level.tail].next =
                index;

            node.prev =
                level.tail;

            level.tail =
                index;
        }

        /*
         * Update best ask.
         */
        if (
            lowestAskPrice == -1 ||
            order.price < lowestAskPrice
        ) {

            lowestAskPrice =
                order.price;
        }
    }


    /*
     * Direct OrderId -> pool index.
     *
     * No hashing.
     */
    orderIndex[
        static_cast<std::size_t>(order.id)
    ] = {
        order.side,
        order.price,
        index
    };
}


bool OrderBook::empty() const
{
    return highestBidPrice == -1 &&
           lowestAskPrice == -1;
}


bool OrderBook::hasBids() const
{
    return highestBidPrice != -1;
}


bool OrderBook::hasAsks() const
{
    return lowestAskPrice != -1;
}


int OrderBook::bestBid() const
{
    if (highestBidPrice == -1) {
        return 0;
    }

    return highestBidPrice;
}


int OrderBook::bestAsk() const
{
    if (lowestAskPrice == -1) {
        return 0;
    }

    return lowestAskPrice;
}


Order& OrderBook::bestBidOrder()
{
    return orderPool[
        bids.at(highestBidPrice).head
    ].order;
}


Order& OrderBook::bestAskOrder()
{
    return orderPool[
        asks.at(lowestAskPrice).head
    ].order;
}


void OrderBook::removeOrderFromLevel(
    std::size_t index
)
{
    if (index >= orderPool.size()) {
        return;
    }

    OrderNode& node =
        orderPool[index];

    if (!node.active) {
        return;
    }

    const OrderId id =
        node.order.id;

    const Side side =
        node.order.side;

    const int price =
        node.order.price;

    const std::size_t prev =
        node.prev;

    const std::size_t next =
        node.next;


    /*
     * Remove from the appropriate price level.
     */
    if (side == Side::BUY) {

        auto priceIt =
            bids.find(price);

        if (priceIt == bids.end()) {
            return;
        }

        PriceLevel& level =
            priceIt->second;

        if (prev != INVALID_INDEX) {

            orderPool[prev].next =
                next;
        }
        else {

            level.head =
                next;
        }

        if (next != INVALID_INDEX) {

            orderPool[next].prev =
                prev;
        }
        else {

            level.tail =
                prev;
        }

        /*
         * Entire price level disappeared.
         */
        if (level.head == INVALID_INDEX) {

            bids.erase(priceIt);

            /*
             * Recalculate best bid only when the
             * current best level disappeared.
             */
            if (price == highestBidPrice) {

                if (bids.empty()) {

                    highestBidPrice =
                        -1;
                }
                else {

                    highestBidPrice =
                        bids.begin()->first;
                }
            }
        }
    }


    else {

        auto priceIt =
            asks.find(price);

        if (priceIt == asks.end()) {
            return;
        }

        PriceLevel& level =
            priceIt->second;

        if (prev != INVALID_INDEX) {

            orderPool[prev].next =
                next;
        }
        else {

            level.head =
                next;
        }

        if (next != INVALID_INDEX) {

            orderPool[next].prev =
                prev;
        }
        else {

            level.tail =
                prev;
        }

        /*
         * Entire price level disappeared.
         */
        if (level.head == INVALID_INDEX) {

            asks.erase(priceIt);

            /*
             * Recalculate best ask only when the
             * current best level disappeared.
             */
            if (price == lowestAskPrice) {

                if (asks.empty()) {

                    lowestAskPrice =
                        -1;
                }
                else {

                    lowestAskPrice =
                        asks.begin()->first;
                }
            }
        }
    }


    /*
     * Mark the OrderId entry as invalid.
     *
     * We do NOT erase the vector element.
     */
    orderIndex[
        static_cast<std::size_t>(id)
    ].index = INVALID_INDEX;


    /*
     * Return the order slot to the free list.
     */
    node.active = false;

    node.prev =
        INVALID_INDEX;

    node.next =
        INVALID_INDEX;

    freeSlots.push_back(index);
}


void OrderBook::removeBestBidOrder()
{
    if (highestBidPrice == -1) {
        return;
    }

    const std::size_t index =
        bids.at(highestBidPrice).head;

    removeOrderFromLevel(index);
}


void OrderBook::removeBestAskOrder()
{
    if (lowestAskPrice == -1) {
        return;
    }

    const std::size_t index =
        asks.at(lowestAskPrice).head;

    removeOrderFromLevel(index);
}


void OrderBook::printBook() const
{
    std::cout
        << "\n========== ORDER BOOK ==========\n";

    std::cout
        << "\nASKS\n";

    std::cout
        << "-----------------\n";

    for (
        const auto& [price, level]
        : asks
    ) {

        int totalQuantity = 0;

        std::size_t index =
            level.head;

        while (
            index != INVALID_INDEX
        ) {

            const OrderNode& node =
                orderPool[index];

            if (node.active) {

                totalQuantity +=
                    node.order.quantity;
            }

            index =
                node.next;
        }

        std::cout
            << price
            << " -> "
            << totalQuantity
            << '\n';
    }


    std::cout
        << "\nBIDS\n";

    std::cout
        << "-----------------\n";

    for (
        const auto& [price, level]
        : bids
    ) {

        int totalQuantity = 0;

        std::size_t index =
            level.head;

        while (
            index != INVALID_INDEX
        ) {

            const OrderNode& node =
                orderPool[index];

            if (node.active) {

                totalQuantity +=
                    node.order.quantity;
            }

            index =
                node.next;
        }

        std::cout
            << price
            << " -> "
            << totalQuantity
            << '\n';
    }

    std::cout
        << "\n================================\n";
}


bool OrderBook::cancelOrder(
    OrderId orderId
)
{
    /*
     * Direct vector lookup.
     */
    if (
        orderId == 0 ||
        orderId >= orderIndex.size()
    ) {

        return false;
    }

    const OrderLocation& location =
        orderIndex[
            static_cast<std::size_t>(orderId)
        ];

    if (location.index == INVALID_INDEX) {
        return false;
    }

    const std::size_t index =
        location.index;

    if (
        index >= orderPool.size() ||
        !orderPool[index].active
    ) {

        return false;
    }

    if (
        orderPool[index].order.id !=
        orderId
    ) {

        return false;
    }

    removeOrderFromLevel(index);

    return true;
}


bool OrderBook::modifyOrder(
    OrderId orderId,
    int newPrice,
    int newQuantity
)
{
    /*
     * Direct vector lookup.
     */
    if (
        orderId == 0 ||
        orderId >= orderIndex.size()
    ) {

        return false;
    }

    const OrderLocation& location =
        orderIndex[
            static_cast<std::size_t>(orderId)
        ];

    if (location.index == INVALID_INDEX) {
        return false;
    }

    const std::size_t index =
        location.index;

    if (
        index >= orderPool.size() ||
        !orderPool[index].active
    ) {

        return false;
    }

    if (
        newQuantity <= 0 ||
        newPrice <= 0
    ) {

        return false;
    }

    Order& currentOrder =
        orderPool[index].order;


    /*
     * Quantity decrease at the same price:
     * preserve time priority.
     */
    if (
        newPrice == currentOrder.price &&
        newQuantity < currentOrder.quantity
    ) {

        currentOrder.quantity =
            newQuantity;

        return true;
    }


    /*
     * Price change or quantity increase:
     * remove and append at the new level.
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
     * Best bid < best ask.
     */
    if (
        !bids.empty() &&
        !asks.empty()
    ) {

        if (
            highestBidPrice >=
            lowestAskPrice
        ) {

            return false;
        }
    }


    /*
     * Best bid cache.
     */
    if (!bids.empty()) {

        if (
            highestBidPrice !=
            bids.begin()->first
        ) {

            return false;
        }
    }
    else {

        if (highestBidPrice != -1) {
            return false;
        }
    }


    /*
     * Best ask cache.
     */
    if (!asks.empty()) {

        if (
            lowestAskPrice !=
            asks.begin()->first
        ) {

            return false;
        }
    }
    else {

        if (lowestAskPrice != -1) {
            return false;
        }
    }


    /*
     * Validate every bid level.
     */
    for (
        const auto& [price, level]
        : bids
    ) {

        if (price <= 0) {
            return false;
        }

        std::size_t index =
            level.head;

        std::size_t previous =
            INVALID_INDEX;

        while (
            index != INVALID_INDEX
        ) {

            if (index >= orderPool.size()) {
                return false;
            }

            const OrderNode& node =
                orderPool[index];

            if (!node.active) {
                return false;
            }

            if (
                node.order.quantity <= 0
            ) {

                return false;
            }

            if (
                node.order.side !=
                Side::BUY
            ) {

                return false;
            }

            if (
                node.order.price !=
                price
            ) {

                return false;
            }

            if (
                node.prev != previous
            ) {

                return false;
            }

            previous =
                index;

            index =
                node.next;
        }

        if (
            level.tail !=
            previous
        ) {

            return false;
        }
    }


    /*
     * Validate every ask level.
     */
    for (
        const auto& [price, level]
        : asks
    ) {

        if (price <= 0) {
            return false;
        }

        std::size_t index =
            level.head;

        std::size_t previous =
            INVALID_INDEX;

        while (
            index != INVALID_INDEX
        ) {

            if (index >= orderPool.size()) {
                return false;
            }

            const OrderNode& node =
                orderPool[index];

            if (!node.active) {
                return false;
            }

            if (
                node.order.quantity <= 0
            ) {

                return false;
            }

            if (
                node.order.side !=
                Side::SELL
            ) {

                return false;
            }

            if (
                node.order.price !=
                price
            ) {

                return false;
            }

            if (
                node.prev != previous
            ) {

                return false;
            }

            previous =
                index;

            index =
                node.next;
        }

        if (
            level.tail !=
            previous
        ) {

            return false;
        }
    }


    /*
     * Validate direct OrderId index.
     *
     * Unlike unordered_map, invalid entries
     * remain in the vector.
     */
    for (
        std::size_t id = 1;
        id < orderIndex.size();
        ++id
    ) {

        const OrderLocation& location =
            orderIndex[id];

        if (
            location.index ==
            INVALID_INDEX
        ) {

            continue;
        }

        if (
            location.index >=
            orderPool.size()
        ) {

            return false;
        }

        const OrderNode& node =
            orderPool[location.index];

        if (!node.active) {
            return false;
        }

        if (
            node.order.id != id
        ) {

            return false;
        }

        if (
            node.order.side !=
            location.side
        ) {

            return false;
        }

        if (
            node.order.price !=
            location.price
        ) {

            return false;
        }
    }


    return true;
}