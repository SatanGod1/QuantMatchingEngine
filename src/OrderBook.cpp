#include "OrderBook.h"

#include <iostream>
#include <stdexcept>

// ============================================================
// ADD ORDER
// ============================================================

void OrderBook::addOrder(const Order& order)
{
    if (order.quantity <= 0)
    {
        return;
    }

    if (order.side == Side::BUY)
    {
        bids[order.price].push_back(order);
    }
    else
    {
        asks[order.price].push_back(order);
    }
}

// ============================================================
// CANCEL ORDER
// ============================================================

bool OrderBook::cancelOrder(uint64_t orderId)
{
    // --------------------------------------------------------
    // Search bids
    // --------------------------------------------------------

    for (auto levelIt = bids.begin(); levelIt != bids.end(); ++levelIt)
    {
        auto& queue = levelIt->second;

        for (auto orderIt = queue.begin(); orderIt != queue.end(); ++orderIt)
        {
            if (orderIt->id == orderId)
            {
                orderIt->status = OrderStatus::CANCELLED;
                queue.erase(orderIt);

                if (queue.empty())
                {
                    bids.erase(levelIt);
                }

                return true;
            }
        }
    }

    // --------------------------------------------------------
    // Search asks
    // --------------------------------------------------------

    for (auto levelIt = asks.begin(); levelIt != asks.end(); ++levelIt)
    {
        auto& queue = levelIt->second;

        for (auto orderIt = queue.begin(); orderIt != queue.end(); ++orderIt)
        {
            if (orderIt->id == orderId)
            {
                orderIt->status = OrderStatus::CANCELLED;
                queue.erase(orderIt);

                if (queue.empty())
                {
                    asks.erase(levelIt);
                }

                return true;
            }
        }
    }

    return false;
}

// ============================================================
// HAS BIDS
// ============================================================

bool OrderBook::hasBids() const
{
    return !bids.empty();
}

// ============================================================
// HAS ASKS
// ============================================================

bool OrderBook::hasAsks() const
{
    return !asks.empty();
}

// ============================================================
// BEST BID PRICE
// ============================================================

int OrderBook::getBestBidPrice() const
{
    if (bids.empty())
    {
        throw std::runtime_error("No bids");
    }

    return bids.begin()->first;
}

// ============================================================
// BEST ASK PRICE
// ============================================================

int OrderBook::getBestAskPrice() const
{
    if (asks.empty())
    {
        throw std::runtime_error("No asks");
    }

    return asks.begin()->first;
}

// ============================================================
// BEST BID ORDERS
// ============================================================

OrderBook::OrderQueue& OrderBook::getBestBidOrders()
{
    if (bids.empty())
    {
        throw std::runtime_error("No bids");
    }

    return bids.begin()->second;
}

const OrderBook::OrderQueue&
OrderBook::getBestBidOrders() const
{
    if (bids.empty())
    {
        throw std::runtime_error("No bids");
    }

    return bids.begin()->second;
}

// ============================================================
// BEST ASK ORDERS
// ============================================================

OrderBook::OrderQueue& OrderBook::getBestAskOrders()
{
    if (asks.empty())
    {
        throw std::runtime_error("No asks");
    }

    return asks.begin()->second;
}

const OrderBook::OrderQueue&
OrderBook::getBestAskOrders() const
{
    if (asks.empty())
    {
        throw std::runtime_error("No asks");
    }

    return asks.begin()->second;
}

// ============================================================
// REMOVE BEST BID
// ============================================================

void OrderBook::removeBestBid()
{
    if (!bids.empty())
    {
        bids.erase(bids.begin());
    }
}

// ============================================================
// REMOVE BEST ASK
// ============================================================

void OrderBook::removeBestAsk()
{
    if (!asks.empty())
    {
        asks.erase(asks.begin());
    }
}

// ============================================================
// ALL BID LEVELS
// ============================================================

std::vector<OrderBook::PriceLevelInfo>
OrderBook::getAllBidLevels() const
{
    std::vector<PriceLevelInfo> result;

    for (const auto& [price, orders] : bids)
    {
        int totalQuantity = 0;

        for (const auto& order : orders)
        {
            totalQuantity += order.quantity;
        }

        result.push_back({price, totalQuantity});
    }

    return result;
}

// ============================================================
// ALL ASK LEVELS
// ============================================================

std::vector<OrderBook::PriceLevelInfo>
OrderBook::getAllAskLevels() const
{
    std::vector<PriceLevelInfo> result;

    for (const auto& [price, orders] : asks)
    {
        int totalQuantity = 0;

        for (const auto& order : orders)
        {
            totalQuantity += order.quantity;
        }

        result.push_back({price, totalQuantity});
    }

    return result;
}

// ============================================================
// BID LEVELS WITH DEPTH
// ============================================================

std::vector<OrderBook::PriceLevelInfo>
OrderBook::getBidLevels(std::size_t depth) const
{
    std::vector<PriceLevelInfo> result;

    std::size_t count = 0;

    for (const auto& [price, orders] : bids)
    {
        if (count >= depth)
        {
            break;
        }

        int totalQuantity = 0;

        for (const auto& order : orders)
        {
            totalQuantity += order.quantity;
        }

        result.push_back({price, totalQuantity});

        ++count;
    }

    return result;
}

// ============================================================
// ASK LEVELS WITH DEPTH
// ============================================================

std::vector<OrderBook::PriceLevelInfo>
OrderBook::getAskLevels(std::size_t depth) const
{
    std::vector<PriceLevelInfo> result;

    std::size_t count = 0;

    for (const auto& [price, orders] : asks)
    {
        if (count >= depth)
        {
            break;
        }

        int totalQuantity = 0;

        for (const auto& order : orders)
        {
            totalQuantity += order.quantity;
        }

        result.push_back({price, totalQuantity});

        ++count;
    }

    return result;
}

// ============================================================
// PRINT BOOK
// ============================================================

void OrderBook::printBook() const
{
    std::cout << "\n========== ORDER BOOK ==========\n";

    std::cout << "\nASKS:\n";

    for (auto it = asks.rbegin(); it != asks.rend(); ++it)
    {
        int totalQuantity = 0;

        for (const auto& order : it->second)
        {
            totalQuantity += order.quantity;
        }

        std::cout
            << "Price: " << it->first
            << " | Quantity: " << totalQuantity
            << '\n';
    }

    std::cout << "\nBIDS:\n";

    for (const auto& [price, orders] : bids)
    {
        int totalQuantity = 0;

        for (const auto& order : orders)
        {
            totalQuantity += order.quantity;
        }

        std::cout
            << "Price: " << price
            << " | Quantity: " << totalQuantity
            << '\n';
    }

    std::cout << "\n===============================\n";
}

// ============================================================
// LEVEL COUNTS
// ============================================================

std::size_t OrderBook::bidLevelCount() const
{
    return bids.size();
}

std::size_t OrderBook::askLevelCount() const
{
    return asks.size();
}