#pragma once

#include "Order.h"

#include <cstddef>
#include <memory>
#include <vector>


class OrderPool {

private:

    std::vector<std::unique_ptr<Order>> storage;

    std::size_t capacity;

public:

    explicit OrderPool(std::size_t capacity)
        : capacity(capacity)
    {
        storage.reserve(capacity);
    }


    Order* create(const Order& order)
    {
        if (storage.size() >= capacity) {
            return nullptr;
        }

        storage.push_back(
            std::make_unique<Order>(order)
        );

        return storage.back().get();
    }


    void reset()
    {
        storage.clear();
    }


    std::size_t size() const
    {
        return storage.size();
    }


    std::size_t getCapacity() const
    {
        return capacity;
    }
};