#pragma once

#include "Order.h"

struct Trade {
    OrderId buyOrderId;
    OrderId sellOrderId;

    int price;
    int quantity;

    Trade(
        OrderId buyOrderId,
        OrderId sellOrderId,
        int price,
        int quantity
    )
        : buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          price(price),
          quantity(quantity) {}
};