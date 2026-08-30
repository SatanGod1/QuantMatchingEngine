#pragma once

#include <cstdint>

using OrderId = std::uint64_t;

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

struct Order {

    OrderId id;

    Side side;

    OrderType type;

    int price;

    int quantity;

    // Existing limit-order constructor
    Order(
        OrderId id,
        Side side,
        int price,
        int quantity
    )
        : id(id),
          side(side),
          type(OrderType::LIMIT),
          price(price),
          quantity(quantity) {}

    // Market/general constructor
    Order(
        OrderId id,
        Side side,
        OrderType type,
        int price,
        int quantity
    )
        : id(id),
          side(side),
          type(type),
          price(price),
          quantity(quantity) {}
};