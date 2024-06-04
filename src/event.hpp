#pragma once

#include <array>
#include <algorithm>

template <size_t MaxEvents, typename... Args>
class Event
{
    using Handler = void (*)(Args...);

    std::array<Handler, MaxEvents> handlers;
    Handler *begin_addr;
    Handler *end_addr;

public:
    Event() : handlers{}
    {
        begin_addr = handlers.begin();
        end_addr = begin_addr;
    }

    Handler *begin() { return begin_addr; }
    Handler *end() { return end_addr; }

    void connect(Handler handler)
    {
        *end_addr = handler;
        end_addr += 1;
    }

    void disconnect(Handler handler)
    {
        end_addr = std::remove(std::begin(handlers), std::end(handlers), handler);
    }

    void emit(Args... args)
    {
        for (auto it : *this)
        {
            (*it)(args...);
        }
    }
};