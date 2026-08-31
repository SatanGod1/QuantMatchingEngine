#include "MatchingEngine.h"
#include "OrderValidator.h"

#include <algorithm>

MatchingEngine::MatchingEngine(
    std::size_t expectedEvents
) {
    if (expectedEvents > 0) {
        events.reserve(expectedEvents);
    }
}

const OrderBook& MatchingEngine::getOrderBook() const {

    return orderBook;
}

const std::vector<Event>& MatchingEngine::getEvents() const {
    return events;
}

std::vector<Trade>
MatchingEngine::submitOrder(const Order& incomingOrder) {

    std::vector<Trade> trades;

    if (!OrderValidator::validate(incomingOrder)) {

        events.emplace_back(
            EventType::ORDER_REJECTED,
            incomingOrder.id,
            0,
            incomingOrder.price,
            incomingOrder.quantity
        );

        return trades;
    }

    events.emplace_back(
        EventType::ORDER_ACCEPTED,
        incomingOrder.id,
        0,
        incomingOrder.price,
        incomingOrder.quantity
    );

    Order incoming = incomingOrder;

    // BUY
    if (incoming.side == Side::BUY) {

        while (
            incoming.quantity > 0 &&
            orderBook.hasAsks() &&
            (
                incoming.type == OrderType::MARKET ||
                orderBook.bestAsk() <= incoming.price
            )
        ) {

            Order& sellOrder =
                orderBook.bestAskOrder();

            int tradeQuantity =
                std::min(
                    incoming.quantity,
                    sellOrder.quantity
                );

            int tradePrice =
                sellOrder.price;

            trades.emplace_back(
                incoming.id,
                sellOrder.id,
                tradePrice,
                tradeQuantity
            );

            events.emplace_back(
                EventType::TRADE_EXECUTED,
                incoming.id,
                sellOrder.id,
                tradePrice,
                tradeQuantity,
                incoming.id,
                sellOrder.id
            );

            incoming.quantity -= tradeQuantity;
            sellOrder.quantity -= tradeQuantity;

            if (sellOrder.quantity == 0)
            {

                events.emplace_back(
                    EventType::ORDER_FILLED,
                    sellOrder.id,
                    incoming.id,
                    tradePrice,
                    tradeQuantity);

                orderBook.removeBestAskOrder();
            }
        }
    }

    // SELL
    else {

        while (
            incoming.quantity > 0 &&
            orderBook.hasBids() &&
            (
                incoming.type == OrderType::MARKET ||
                orderBook.bestBid() >= incoming.price
            )
        ) {

            Order& buyOrder =
                orderBook.bestBidOrder();

            int tradeQuantity =
                std::min(
                    incoming.quantity,
                    buyOrder.quantity
                );

            int tradePrice =
                buyOrder.price;

            trades.emplace_back(
                buyOrder.id,
                incoming.id,
                tradePrice,
                tradeQuantity
            );

            events.emplace_back(
                EventType::TRADE_EXECUTED,
                incoming.id,
                buyOrder.id,
                tradePrice,
                tradeQuantity,
                buyOrder.id,
                incoming.id
            );

            incoming.quantity -= tradeQuantity;
            buyOrder.quantity -= tradeQuantity;

            if (buyOrder.quantity == 0)
            {

                events.emplace_back(
                    EventType::ORDER_FILLED,
                    buyOrder.id,
                    incoming.id,
                    tradePrice,
                    tradeQuantity);

                orderBook.removeBestBidOrder();
            }
        }
    }

    if (incoming.quantity == 0)
    {

        events.emplace_back(
            EventType::ORDER_FILLED,
            incoming.id,
            0,
            incoming.price,
            incomingOrder.quantity);
    }

    // Only LIMIT orders can rest in the book
    if (
        incoming.quantity > 0 &&
        incoming.type == OrderType::LIMIT)
    {

        orderBook.addOrder(incoming);

        events.emplace_back(
            EventType::ORDER_ADDED,
            incoming.id,
            0,
            incoming.price,
            incoming.quantity);
    }

    return trades;
}

bool MatchingEngine::cancelOrder(OrderId orderId) {

    bool cancelled = orderBook.cancelOrder(orderId);

    if (cancelled) {

        events.emplace_back(
            EventType::ORDER_CANCELLED,
            orderId
        );
    }

    return cancelled;
}

bool MatchingEngine::modifyOrder(
    OrderId orderId,
    int newPrice,
    int newQuantity
) {

    bool modified = orderBook.modifyOrder(
        orderId,
        newPrice,
        newQuantity
    );

    if (modified) {

        events.emplace_back(
            EventType::ORDER_MODIFIED,
            orderId,
            0,
            newPrice,
            newQuantity
        );
    }

    return modified;
}