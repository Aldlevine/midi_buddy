#pragma once

#include "messages/messages.hpp"

#include <algorithm>

constexpr size_t MAX_NOTES = 64;

class NoteTracker
{

    MessageConsumer<NoteMessage> note_mc;
    size_t max_voices;

    std::array<uint8_t, MAX_NOTES> notes_stack{};
    uint8_t *notes_tail{std::begin(notes_stack)};

public:
    NoteTracker(MessageQueue<NoteMessage> &note_mq, uint8_t max_voices = 1)
        : note_mc{note_mq}, max_voices{max_voices}
    {
    }

    void task()
    {
        NoteMessage note_msg;
        auto lock_guard = note_mc.lock_guard();
        while (note_mc.get_message(&note_msg))
        {
            if (note_msg.on)
            {
                if (notes_tail != std::end(notes_stack))
                {
                    *notes_tail = note_msg.note;
                    ++notes_tail;
                }
            }
            else
            {
                notes_tail = std::remove(std::begin(notes_stack), notes_tail, note_msg.note);
            }
        }
    }

    class VoicesIterator
    {
        NoteTracker &note_tracker;

    public:
        VoicesIterator(NoteTracker &note_tracker) : note_tracker{note_tracker} {}

        auto begin()
        {
            return std::reverse_iterator{note_tracker.notes_tail};
        }

        auto end()
        {
            return std::reverse_iterator{std::begin(note_tracker.notes_stack)};
        }
    };

    VoicesIterator voices()
    {
        return VoicesIterator{*this};
    }
};