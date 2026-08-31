#include "PriceLevelArray.h"

#include <stdexcept>

PriceLevelArray::PriceLevelArray(
    bool isBid,
    int minPrice,
    int maxPrice
)
    : levels(
          static_cast<std::size_t>(
              maxPrice - minPrice + 1
          )
      ),
      minPrice(minPrice),
      maxPrice(maxPrice),
      bestPriceIndex(-1),
      isBid(isBid)
{
}

bool PriceLevelArray::containsPrice(int price) const
{
    return price >= minPrice &&
           price <= maxPrice;
}

PriceLevelArray::Level&
PriceLevelArray::level(int price)
{
    if (!containsPrice(price)) {
        throw std::out_of_range(
            "Price outside PriceLevelArray range"
        );
    }

    return levels[
        static_cast<std::size_t>(
            price - minPrice
        )
    ];
}

const PriceLevelArray::Level&
PriceLevelArray::level(int price) const
{
    if (!containsPrice(price)) {
        throw std::out_of_range(
            "Price outside PriceLevelArray range"
        );
    }

    return levels[
        static_cast<std::size_t>(
            price - minPrice
        )
    ];
}

int PriceLevelArray::bestPrice() const
{
    if (bestPriceIndex == -1) {
        return 0;
    }

    return minPrice + bestPriceIndex;
}

void PriceLevelArray::activate(
    int price,
    std::size_t orderIndex
)
{
    Level& current = level(price);

    current.head = orderIndex;
    current.tail = orderIndex;
    current.active = true;

    updateBestAfterActivation(price);
}

void PriceLevelArray::deactivate(int price)
{
    Level& current = level(price);

    current.head = INVALID_INDEX;
    current.tail = INVALID_INDEX;
    current.active = false;

    updateBestAfterDeactivation(price);
}

void PriceLevelArray::updateBestAfterActivation(int price)
{
    const int index = price - minPrice;

    if (bestPriceIndex == -1) {
        bestPriceIndex = index;
        return;
    }

    if (isBid) {
        if (index > bestPriceIndex) {
            bestPriceIndex = index;
        }
    }
    else {
        if (index < bestPriceIndex) {
            bestPriceIndex = index;
        }
    }
}

void PriceLevelArray::updateBestAfterDeactivation(int price)
{
    const int removedIndex = price - minPrice;

    if (removedIndex != bestPriceIndex) {
        return;
    }

    if (isBid) {
        for (int i = removedIndex - 1; i >= 0; --i) {
            if (levels[i].active) {
                bestPriceIndex = i;
                return;
            }
        }
    }
    else {
        const int lastIndex = maxPrice - minPrice;

        for (int i = removedIndex + 1;
             i <= lastIndex;
             ++i) {

            if (levels[i].active) {
                bestPriceIndex = i;
                return;
            }
        }
    }

    bestPriceIndex = -1;
}