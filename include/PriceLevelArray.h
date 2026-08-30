#pragma once

#include "Order.h"

#include <cstddef>
#include <list>
#include <vector>
#include <limits>

class PriceLevelArray
{
public:

    using OrderList = std::list<Order*>;

    struct PriceLevel
    {
        int price = 0;
        OrderList orders;
        bool active = false;
    };

private:

    std::vector<PriceLevel> levels;

    int minPrice = 0;
    int maxPrice = -1;

    std::size_t activeLevels = 0;

    // ========================================================
    // FAST CACHE
    // ========================================================

    PriceLevel* cachedLevel = nullptr;
    int cachedPrice = 0;

    // ========================================================
    // O(1) BEST PRICE CACHE
    // ========================================================

    int bestBidPrice = 0;
    int bestAskPrice = 0;

    bool hasBestBid = false;
    bool hasBestAsk = false;

    // ========================================================
    // RECALCULATE BEST BID
    // ========================================================

    void recalculateBestBid()
    {
        if (activeLevels == 0)
        {
            hasBestBid = false;
            bestBidPrice = 0;
            return;
        }

        for (int price = maxPrice;
             price >= minPrice;
             --price)
        {
            std::size_t index =
                static_cast<std::size_t>(
                    price - minPrice
                );

            if (
                index < levels.size() &&
                levels[index].active
            )
            {
                bestBidPrice = price;
                hasBestBid = true;
                return;
            }
        }

        hasBestBid = false;
        bestBidPrice = 0;
    }

    // ========================================================
    // RECALCULATE BEST ASK
    // ========================================================

    void recalculateBestAsk()
    {
        if (activeLevels == 0)
        {
            hasBestAsk = false;
            bestAskPrice = 0;
            return;
        }

        for (int price = minPrice;
             price <= maxPrice;
             ++price)
        {
            std::size_t index =
                static_cast<std::size_t>(
                    price - minPrice
                );

            if (
                index < levels.size() &&
                levels[index].active
            )
            {
                bestAskPrice = price;
                hasBestAsk = true;
                return;
            }
        }

        hasBestAsk = false;
        bestAskPrice = 0;
    }

public:

    PriceLevelArray() = default;

    // ========================================================
    // FIND
    // ========================================================

    PriceLevel* find(int price)
    {
        if (
            levels.empty() ||
            price < minPrice ||
            price > maxPrice
        )
        {
            return nullptr;
        }

        const std::size_t index =
            static_cast<std::size_t>(
                price - minPrice
            );

        if (!levels[index].active)
        {
            return nullptr;
        }

        return &levels[index];
    }

    const PriceLevel* find(int price) const
    {
        if (
            levels.empty() ||
            price < minPrice ||
            price > maxPrice
        )
        {
            return nullptr;
        }

        const std::size_t index =
            static_cast<std::size_t>(
                price - minPrice
            );

        if (!levels[index].active)
        {
            return nullptr;
        }

        return &levels[index];
    }

    // ========================================================
    // GET OR CREATE
    // ========================================================

    PriceLevel& getOrCreate(int price)
    {
        // ====================================================
        // FAST CACHE PATH
        // ====================================================

        if (
            cachedLevel != nullptr &&
            cachedPrice == price &&
            cachedLevel->active
        )
        {
            return *cachedLevel;
        }

        // ====================================================
        // FIRST PRICE LEVEL
        // ====================================================

        if (levels.empty())
        {
            minPrice = price;
            maxPrice = price;

            levels.resize(1);

            levels[0].price = price;
            levels[0].active = true;

            activeLevels = 1;

            cachedLevel = &levels[0];
            cachedPrice = price;

            // First level is both best bid and best ask.
            bestBidPrice = price;
            bestAskPrice = price;

            hasBestBid = true;
            hasBestAsk = true;

            return levels[0];
        }

        // ====================================================
        // PRICE BELOW CURRENT RANGE
        // ====================================================

        if (price < minPrice)
        {
            const std::size_t amount =
                static_cast<std::size_t>(
                    minPrice - price
                );

            levels.insert(
                levels.begin(),
                amount,
                PriceLevel{}
            );

            minPrice = price;

            levels[0].price = price;
            levels[0].active = true;

            ++activeLevels;

            // Pointers may have been invalidated.
            cachedLevel = &levels[0];
            cachedPrice = price;

            // A new lowest level is always the best ASK.
            if (
                !hasBestAsk ||
                price < bestAskPrice
            )
            {
                bestAskPrice = price;
                hasBestAsk = true;
            }

            // Bid only changes if there was no bid.
            if (!hasBestBid)
            {
                bestBidPrice = price;
                hasBestBid = true;
            }

            return levels[0];
        }

        // ====================================================
        // PRICE ABOVE CURRENT RANGE
        // ====================================================

        if (price > maxPrice)
        {
            const std::size_t oldSize =
                levels.size();

            levels.resize(
                oldSize +
                static_cast<std::size_t>(
                    price - maxPrice
                )
            );

            maxPrice = price;

            const std::size_t index =
                static_cast<std::size_t>(
                    price - minPrice
                );

            levels[index].price = price;
            levels[index].active = true;

            ++activeLevels;

            // resize() may invalidate pointers.
            cachedLevel = &levels[index];
            cachedPrice = price;

            // A new highest level is always the best BID.
            if (
                !hasBestBid ||
                price > bestBidPrice
            )
            {
                bestBidPrice = price;
                hasBestBid = true;
            }

            // Ask only changes if there was no ask.
            if (!hasBestAsk)
            {
                bestAskPrice = price;
                hasBestAsk = true;
            }

            return levels[index];
        }

        // ====================================================
        // PRICE ALREADY INSIDE RANGE
        // ====================================================

        const std::size_t index =
            static_cast<std::size_t>(
                price - minPrice
            );

        PriceLevel& level =
            levels[index];

        if (!level.active)
        {
            level.price = price;
            level.active = true;

            ++activeLevels;

            // Update cached best prices.

            if (
                !hasBestBid ||
                price > bestBidPrice
            )
            {
                bestBidPrice = price;
                hasBestBid = true;
            }

            if (
                !hasBestAsk ||
                price < bestAskPrice
            )
            {
                bestAskPrice = price;
                hasBestAsk = true;
            }
        }

        cachedLevel = &level;
        cachedPrice = price;

        return level;
    }

    // ========================================================
    // DEACTIVATE
    // ========================================================

    void deactivate(int price)
    {
        PriceLevel* level = find(price);

        if (level == nullptr)
        {
            return;
        }

        level->orders.clear();
        level->active = false;

        if (activeLevels > 0)
        {
            --activeLevels;
        }

        // ====================================================
        // INVALIDATE NORMAL CACHE
        // ====================================================

        if (
            cachedLevel == level &&
            cachedPrice == price
        )
        {
            cachedLevel = nullptr;
        }

        // ====================================================
        // UPDATE BEST PRICE CACHE
        // ====================================================

        if (
            hasBestBid &&
            price == bestBidPrice
        )
        {
            recalculateBestBid();
        }

        if (
            hasBestAsk &&
            price == bestAskPrice
        )
        {
            recalculateBestAsk();
        }
    }

    // ========================================================
    // SIZE
    // ========================================================

    std::size_t size() const
    {
        return activeLevels;
    }

    // ========================================================
    // EMPTY
    // ========================================================

    bool empty() const
    {
        return activeLevels == 0;
    }

    // ========================================================
    // RAW RANGE
    // ========================================================

    int getMinPrice() const
    {
        return minPrice;
    }

    int getMaxPrice() const
    {
        return maxPrice;
    }

    // ========================================================
    // HAS LEVEL
    // ========================================================

    bool hasLevel(int price) const
    {
        return find(price) != nullptr;
    }

    // ========================================================
    // GET LEVEL AT INDEX
    // ========================================================

    const PriceLevel* getLevelAtIndex(
        std::size_t index
    ) const
    {
        if (index >= levels.size())
        {
            return nullptr;
        }

        if (!levels[index].active)
        {
            return nullptr;
        }

        return &levels[index];
    }

    // ========================================================
    // O(1) BEST BID PRICE
    // ========================================================

    int getBestBidPrice() const
    {
        if (!hasBestBid)
        {
            return 0;
        }

        return bestBidPrice;
    }

    // ========================================================
    // O(1) BEST ASK PRICE
    // ========================================================

    int getBestAskPrice() const
    {
        if (!hasBestAsk)
        {
            return 0;
        }

        return bestAskPrice;
    }
};