#pragma once

#include "Order.h"

#include <cstddef>

struct OrderLocation {

    static constexpr std::size_t INVALID_INDEX =
        static_cast<std::size_t>(-1);

    Side side = Side::BUY;

    int price = 0;

    std::size_t index = INVALID_INDEX;
};