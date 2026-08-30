#pragma once

#include <cstdint>

enum class EventType
{
    ORDER_ACCEPTED,
    ORDER_REJECTED,
    TRADE_EXECUTED,
    ORDER_CANCELLED
};

struct Event
{
    EventType type;

    uint64_t orderId;
    uint64_t buyOrderId;
    uint64_t sellOrderId;

    int price;
    int quantity;
};