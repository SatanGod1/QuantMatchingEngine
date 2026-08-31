#pragma once

#include "Order.h"
#include <list>

struct OrderLocation {
    Side side;
    int price;
    std::list<Order>::iterator position;
};