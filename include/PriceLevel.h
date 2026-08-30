#pragma once

#include "Order.h"

#include <list>


class PriceLevel {

public:

    using OrderList =
        std::list<Order>;


private:

    int price;

    OrderList orders;


public:

    explicit PriceLevel(
        int price
    )
        : price(price)
    {
    }


    int getPrice() const
    {
        return price;
    }


    OrderList& getOrders()
    {
        return orders;
    }


    const OrderList& getOrders() const
    {
        return orders;
    }


    bool empty() const
    {
        return orders.empty();
    }
};