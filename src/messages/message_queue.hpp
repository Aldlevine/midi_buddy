#pragma once

#include "concurrent/concurrent.hpp"

#include "hardware/sync.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include <array>

#define MESSAGE_QUEUE_SIZE 256
#define MAX_CONSUMER_COUNT 16

template <typename data_t>
class MessageQueue
{
    std::array<data_t, MESSAGE_QUEUE_SIZE> buffer{};
    data_t *head_address = buffer.begin();
    mutex_t mutex;

public:
    void init()
    {
        mutex_init(&mutex);
    }

    data_t *begin()
    {
        return buffer.begin();
    }

    data_t *end()
    {
        return buffer.end();
    }

    data_t *head() const
    {
        return head_address;
    }

    void enqueue(data_t data)
    {
        *head_address = data;
        if (++head_address == buffer.end())
        {
            head_address = buffer.begin();
        }
    }

    LockGuard lock_guard()
    {
        return LockGuard{mutex};
    }

    data_t operator[](size_t index) const
    {
        return buffer[index];
    }
};
