#pragma once

#include "OrderBook.h"

#include <cstddef>
#include <vector>

struct OrderBookSnapshot
{
    std::vector<OrderBook::PriceLevelInfo> bids;
    std::vector<OrderBook::PriceLevelInfo> asks;
};

class MarketData
{
public:

    static OrderBookSnapshot getSnapshot(
        const OrderBook& orderBook,
        std::size_t depth
    );
};