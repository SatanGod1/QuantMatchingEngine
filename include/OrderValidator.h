#pragma once

#include "Order.h"

class OrderValidator {

public:

    static bool validate(const Order& order) {

        if (order.id == 0) {
            return false;
        }

        if (order.quantity <= 0) {
            return false;
        }

        if (order.type == OrderType::LIMIT) {

            if (order.price <= 0) {
                return false;
            }
        }

        if (order.type == OrderType::MARKET) {

            if (order.price != 0) {
                return false;
            }
        }

        return true;
    }
};