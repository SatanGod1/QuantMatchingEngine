#pragma once

#include "Order.h"

#include <cstdint>

enum class EventType {

    ORDER_ACCEPTED,

    ORDER_REJECTED,

    ORDER_ADDED,

    ORDER_FILLED,

    ORDER_CANCELLED,

    ORDER_MODIFIED,

    TRADE_EXECUTED
};


struct Event {

    EventType type;

    OrderId orderId;

    OrderId relatedOrderId;

    // Add these two
    OrderId buyOrderId;
    OrderId sellOrderId;

    int price;
    int quantity;

    Event(
        EventType type,
        OrderId orderId = 0,
        OrderId relatedOrderId = 0,
        int price = 0,
        int quantity = 0
    )
        : type(type),
          orderId(orderId),
          relatedOrderId(relatedOrderId),
          buyOrderId(0),
          sellOrderId(0),
          price(price),
          quantity(quantity)
    {}
};