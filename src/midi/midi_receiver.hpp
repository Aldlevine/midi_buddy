#pragma once

#include "midi_cin.hpp"
#include "midi_packet.hpp"

#include "messages/messages.hpp"

#include "bsp/board.h"

class MIDIReceiver
{
    uint32_t clock_count = 0;
    bool is_playing = false;

    uint8_t packet[4];
    MIDIPacket midi_packet;

    MessageQueue<ClockMessage> clock_mq;
    MessageQueue<NoteMessage> note_mq;

public:
    MessageQueue<ClockMessage> &get_clock_mq()
    {
        return clock_mq;
    }

    MessageQueue<NoteMessage> &get_note_mq()
    {
        return note_mq;
    }

    void init()
    {
        clock_mq.init();
        note_mq.init();
    }

    void task()
    {
        // auto clock_lock_guard = clock_mq.lock_guard();
        // auto note_lock_guard = note_mq.lock_guard();

        while (tud_midi_available())
        {
            tud_midi_packet_read(packet);
            midi_packet = MIDIPacket{packet};

            if (midi_packet.cin == MIDICin::rt_start)
            {
                clock_count = 0;
                is_playing = true;
                continue;
            }

            if (midi_packet.cin == MIDICin::rt_continue)
            {
                is_playing = true;
                continue;
            }

            if (midi_packet.cin == MIDICin::rt_stop)
            {
                is_playing = false;
                continue;
            }

            if (midi_packet.cin == MIDICin::rt_clock)
            {
                auto clock_index = clock_count;
                clock_count += 1;

                if (is_playing)
                {
                    auto clock_lock_guard = clock_mq.lock_guard();
                    clock_mq.enqueue(ClockMessage{clock_count});
                }

                continue;
            }

            if (midi_packet.cin == MIDICin::sys_song_position)
            {
                uint16_t song_position = combine_14b(midi_packet.data0, midi_packet.data1);
                clock_count = song_position * CLOCKS_PER_QUARTER_BEAT;
                continue;
            }

            if (midi_packet.cin == MIDICin::note_on)
            {
                auto note_lock_guard = note_mq.lock_guard();
                note_mq.enqueue(NoteMessage{.note = midi_packet.data0, .on = true});
                continue;
            }

            if (midi_packet.cin == MIDICin::note_off)
            {
                auto note_lock_guard = note_mq.lock_guard();
                note_mq.enqueue(NoteMessage{.note = midi_packet.data0, .on = false});
                continue;
            }
        }
    }

private:
    uint16_t combine_14b(uint8_t first, uint8_t second)
    {
        uint16_t result;
        result = second;
        result <<= 7;
        result |= first;
        return result;
    }
};