#pragma once

#include <cstddef>
#include <vector>

class PriceLevelArray {

public:

    static constexpr std::size_t INVALID_INDEX =
        static_cast<std::size_t>(-1);

    struct Level {

        std::size_t head = INVALID_INDEX;
        std::size_t tail = INVALID_INDEX;

        bool active = false;
    };

private:

    std::vector<Level> levels;

    int minPrice;
    int maxPrice;

    int bestPriceIndex;

    bool isBid;

public:

    PriceLevelArray(
        bool isBid,
        int minPrice,
        int maxPrice
    );

    bool containsPrice(int price) const;

    Level& level(int price);

    const Level& level(int price) const;

    int bestPrice() const;

    void activate(
        int price,
        std::size_t orderIndex
    );

    void deactivate(int price);

    void updateBestAfterActivation(int price);

    void updateBestAfterDeactivation(int price);
};