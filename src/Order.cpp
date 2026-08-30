#include "Order.h"

Order::Order(
    uint64_t id,
    Side side,
    OrderType type,
    int price,
    int quantity,
    uint64_t timestamp
)
    : id(id),
      side(side),
      type(type),
      price(price),
      quantity(quantity),
      originalQuantity(quantity),
      timestamp(timestamp),
      status(OrderStatus::NEW)
{
}

bool Order::isBuy() const
{
    return side == Side::BUY;
}

bool Order::isSell() const
{
    return side == Side::SELL;
}