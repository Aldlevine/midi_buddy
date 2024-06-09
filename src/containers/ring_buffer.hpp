#pragma once

#include <array>
#include <iterator>

template <typename T, size_t N>
class RingBuffer
{
    std::array<T, N> buffer;
    T *head{std::begin(buffer)};
    T *tail{std::begin(buffer)};

public:
    void append(T data)
    {
        *head = data;
        if (std::next(head) == std::end(buffer))
        {
            head = std::begin(buffer);
            tail = head + 1;
        }
        else
        {
            ++head;
            if (tail == head)
            {
                if (std::next(tail) == std::end(buffer))
                {
                    tail = std::begin(buffer);
                }
                else
                {
                    ++tail;
                }
            }
        }
    }

    class Iterator
    {
        const RingBuffer &ring_buffer;
        const T *current;

    public:
        Iterator(const RingBuffer &ring_buffer, const T *current) : ring_buffer{ring_buffer}, current{current} {}

        Iterator &operator++()
        {
            if (current == ring_buffer.tail)
            {
                this->current = nullptr;
                return *this;
            }

            if (current == std::begin(ring_buffer.buffer))
            {
                this->current = std::prev(ring_buffer.buffer.cend());
                return *this;
            }

            this->current = std::prev(current);
            return *this;
        }

        bool operator==(const Iterator &other) const
        {
            return &ring_buffer == &other.ring_buffer && current == other.current;
        }

        bool operator!=(const Iterator &other) const
        {
            return !(*this == other);
        }

        T operator*()
        {
            return *current;
        }
    };

    Iterator begin()
    {
        if (head == std::begin(buffer))
        {
            return Iterator{*this, std::end(buffer)};
        }
        return Iterator{*this, std::prev(head)};
    }

    Iterator end()
    {
        return Iterator{*this, nullptr};
    }
};