#pragma once

#include "concurrent/concurrent.hpp"

#include "hardware/sync.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include <array>

#define MESSAGE_QUEUE_SIZE 128
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

template <typename Data>
class MessageConsumer
{
    MessageQueue<Data> &message_queue;
    Data *index;

public:
    MessageConsumer(MessageQueue<Data> &message_queue)
        : message_queue{message_queue}, index{message_queue.head()}
    {
    }

    LockGuard lock_guard()
    {
        return message_queue.lock_guard();
    }

    bool has_messages()
    {

        return index != message_queue.head();
    }

    bool get_message(Data *message)
    {
        if (has_messages())
        {
            *message = *index;
            if (++index == message_queue.end())
            {
                index = message_queue.begin();
            }
            return true;
        }
        return false;
    }

    bool get_last_message(Data *message)
    {
        if (has_messages())
        {
            index = message_queue.head();
            if (index == message_queue.begin())
            {
                *message = *(message_queue.end() - 1);
            }
            else
            {
                *message = *(index - 1);
            }
            return true;
        }
        return false;
    }
};
