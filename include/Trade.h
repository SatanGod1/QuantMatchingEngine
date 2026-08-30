#pragma once

#include <cstdint>

struct Trade
{
    uint64_t buyOrderId;
    uint64_t sellOrderId;

    int price;
    int quantity;

    Trade(
        uint64_t buyOrderId,
        uint64_t sellOrderId,
        int price,
        int quantity
    )
        : buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          price(price),
          quantity(quantity)
    {
    }
};