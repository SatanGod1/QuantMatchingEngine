#include "MarketData.h"

// ============================================================
// GET SNAPSHOT
// ============================================================

OrderBookSnapshot MarketData::getSnapshot(
    const OrderBook& orderBook,
    std::size_t depth
)
{
    OrderBookSnapshot snapshot;

    snapshot.bids =
        orderBook.getBidLevels(depth);

    snapshot.asks =
        orderBook.getAskLevels(depth);

    return snapshot;
}