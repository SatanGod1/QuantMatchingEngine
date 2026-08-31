#pragma once

#include "Order.h"
#include <cstddef>

struct OrderLocation {
    Side side;
    int price;
    std::size_t index;
};