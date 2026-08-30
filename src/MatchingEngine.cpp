#include "MatchingEngine.h"

#include <algorithm>

// ============================================================
// SUBMIT ORDER
// ============================================================

void MatchingEngine::submitOrder(Order order)
{
    events.push_back({
        EventType::ORDER_ACCEPTED,
        order.id,
        0,
        0,
        order.price,
        order.quantity
    });

    // ========================================================
    // MATCH
    // ========================================================

    if (order.side == Side::BUY)
    {
        matchBuy(order);
    }
    else
    {
        matchSell(order);
    }

    // ========================================================
    // RESTING LIMIT ORDER
    // ========================================================

    if (order.quantity > 0 &&
        order.type == OrderType::LIMIT)
    {
        if (order.quantity < order.originalQuantity)
        {
            order.status = OrderStatus::PARTIALLY_FILLED;
        }
        else
        {
            order.status = OrderStatus::NEW;
        }

        orderBook.addOrder(order);
    }
    else if (order.quantity == 0)
    {
        order.status = OrderStatus::FILLED;
    }
}

// ============================================================
// BUY MATCHING
// ============================================================

void MatchingEngine::matchBuy(Order& order)
{
    while (
        order.quantity > 0 &&
        orderBook.hasAsks()
    )
    {
        const int bestAsk =
            orderBook.getBestAskPrice();

        // LIMIT order cannot cross this ask.
        if (
            order.type == OrderType::LIMIT &&
            bestAsk > order.price
        )
        {
            break;
        }

        auto& sellOrders =
            orderBook.getBestAskOrders();

        if (sellOrders.empty())
        {
            break;
        }

        Order& sellOrder =
            sellOrders.front();

        const int quantity =
            std::min(
                order.quantity,
                sellOrder.quantity
            );

        Trade trade =
            executeTrade(
                order,
                sellOrder,
                quantity
            );

        trades.push_back(trade);

        events.push_back({
            EventType::TRADE_EXECUTED,
            0,
            trade.buyOrderId,
            trade.sellOrderId,
            trade.price,
            trade.quantity
        });

        order.quantity -= quantity;
        sellOrder.quantity -= quantity;

        if (sellOrder.quantity == 0)
        {
            orderBook.removeBestAsk();
        }
    }
}

// ============================================================
// SELL MATCHING
// ============================================================

void MatchingEngine::matchSell(Order& order)
{
    while (
        order.quantity > 0 &&
        orderBook.hasBids()
    )
    {
        const int bestBid =
            orderBook.getBestBidPrice();

        // LIMIT order cannot cross this bid.
        if (
            order.type == OrderType::LIMIT &&
            bestBid < order.price
        )
        {
            break;
        }

        auto& buyOrders =
            orderBook.getBestBidOrders();

        if (buyOrders.empty())
        {
            break;
        }

        Order& buyOrder =
            buyOrders.front();

        const int quantity =
            std::min(
                order.quantity,
                buyOrder.quantity
            );

        Trade trade =
            executeTrade(
                buyOrder,
                order,
                quantity
            );

        trades.push_back(trade);

        events.push_back({
            EventType::TRADE_EXECUTED,
            0,
            trade.buyOrderId,
            trade.sellOrderId,
            trade.price,
            trade.quantity
        });

        buyOrder.quantity -= quantity;
        order.quantity -= quantity;

        if (buyOrder.quantity == 0)
        {
            orderBook.removeBestBid();
        }
    }
}

// ============================================================
// EXECUTE TRADE
// ============================================================

Trade MatchingEngine::executeTrade(
    const Order& buyOrder,
    const Order& sellOrder,
    int quantity
)
{
    // Trade executes at the resting order's price.
    return Trade(
        buyOrder.id,
        sellOrder.id,
        sellOrder.price,
        quantity
    );
}

// ============================================================
// CANCEL
// ============================================================

bool MatchingEngine::cancelOrder(uint64_t orderId)
{
    bool cancelled =
        orderBook.cancelOrder(orderId);

    if (cancelled)
    {
        events.push_back({
            EventType::ORDER_CANCELLED,
            orderId,
            0,
            0,
            0,
            0
        });
    }

    return cancelled;
}

// ============================================================
// GET TRADES
// ============================================================

const std::vector<Trade>&
MatchingEngine::getTrades() const
{
    return trades;
}

// ============================================================
// GET EVENTS
// ============================================================

const std::vector<Event>&
MatchingEngine::getEvents() const
{
    return events;
}

// ============================================================
// GET ORDER BOOK
// ============================================================

const OrderBook&
MatchingEngine::getOrderBook() const
{
    return orderBook;
}

OrderBook&
MatchingEngine::getOrderBook()
{
    return orderBook;
}