#include "MatchingEngine.h"
#include "OrderValidator.h"

#include <algorithm>

const OrderBook& MatchingEngine::getOrderBook() const {

    return orderBook;
}

std::vector<Trade>
MatchingEngine::submitOrder(const Order& incomingOrder) {

    std::vector<Trade> trades;

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

            incoming.quantity -= tradeQuantity;
            sellOrder.quantity -= tradeQuantity;

            if (sellOrder.quantity == 0) {
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

            incoming.quantity -= tradeQuantity;
            buyOrder.quantity -= tradeQuantity;

            if (buyOrder.quantity == 0) {
                orderBook.removeBestBidOrder();
            }
        }
    }

    // Only LIMIT orders can rest in the book
    if (
        incoming.quantity > 0 &&
        incoming.type == OrderType::LIMIT
    ) {
        orderBook.addOrder(incoming);
    }

    return trades;
}

bool MatchingEngine::cancelOrder(OrderId orderId) {

    return orderBook.cancelOrder(orderId);
}

bool MatchingEngine::modifyOrder(
    OrderId orderId,
    int newPrice,
    int newQuantity
) {

    return orderBook.modifyOrder(
        orderId,
        newPrice,
        newQuantity
    );
}