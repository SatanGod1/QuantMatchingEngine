#pragma once

#include <cstdint>

enum class Side
{
    BUY,
    SELL
};

enum class OrderType
{
    LIMIT,
    MARKET
};

enum class OrderStatus
{
    NEW,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

class Order
{
public:
    uint64_t id;
    Side side;
    OrderType type;

    int price;
    int quantity;
    int originalQuantity;

    uint64_t timestamp;
    OrderStatus status;

    Order(
        uint64_t id,
        Side side,
        OrderType type,
        int price,
        int quantity,
        uint64_t timestamp
    );

    bool isBuy() const;
    bool isSell() const;
};