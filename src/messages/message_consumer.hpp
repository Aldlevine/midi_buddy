#pragma once

#include "concurrent/concurrent.hpp"

template <typename data_t>
class MessageConsumer
{
    MessageQueue<data_t> &message_queue;
    data_t *index;

public:
    MessageConsumer(MessageQueue<data_t> &message_queue)
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

    bool get_message(data_t *message)
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

    bool get_last_message(data_t *message)
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