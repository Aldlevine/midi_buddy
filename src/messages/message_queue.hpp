#pragma once

#include "concurrent/concurrent.hpp"

#include "hardware/sync.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include <array>

#define MESSAGE_QUEUE_SIZE 256
#define MAX_CONSUMER_COUNT 16

template <typename Data>
class MessageQueue
{
    std::array<Data, MESSAGE_QUEUE_SIZE> buffer{};
    Data *head_address = buffer.begin();
    mutex_t mutex;

public:
    void init()
    {
        mutex_init(&mutex);
    }

    Data *begin()
    {
        return buffer.begin();
    }

    Data *end()
    {
        return buffer.end();
    }

    Data *head() const
    {
        return head_address;
    }

    void enqueue(Data data)
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

    Data operator[](size_t index) const
    {
        return buffer[index];
    }
};
