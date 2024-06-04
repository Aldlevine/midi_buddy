#include "blink.hpp"
#include "midi.hpp"
#include "event.hpp"

#include "pico/stdlib.h"
#include "bsp/board.h"

#include <stdio.h>

constexpr auto LED_PIN = 16;

auto blinker = Blinker<LED_PIN, 100>{};

void midi_task()
{
    uint8_t packet[4];
    MIDIPacket midi_packet;

    static uint8_t beat_clock_count = 0;
    static bool is_playing = false;

    while (tud_midi_available())
    {
        tud_midi_packet_read(packet);
        midi_packet = process_midi_packet(packet);

        if (midi_packet.cin == MIDICin::rt_start)
        {
            beat_clock_count = 0;
            is_playing = true;
            printf("MIDI START\n");
            continue;
        }

        if (midi_packet.cin == MIDICin::rt_continue)
        {
            is_playing = true;
            printf("MIDI CONTINUE\n");
            continue;
        }

        if (midi_packet.cin == MIDICin::rt_stop)
        {
            is_playing = false;
            printf("MIDI STOP\n");
            continue;
        }

        if (midi_packet.cin == MIDICin::rt_clock)
        {
            if (is_playing && beat_clock_count == 0)
            {
                blinker.blink();
                printf("BEAT\n");
            }

            beat_clock_count = (beat_clock_count + 1) % CLOCKS_PER_BEAT; // clocks_per_beat
            continue;
        }

        if (midi_packet.cin == MIDICin::sys_song_position)
        {
            uint16_t song_position = combine_14b(midi_packet.data0, midi_packet.data1);
            beat_clock_count = (song_position * CLOCKS_PER_QUARTER_BEAT) % CLOCKS_PER_BEAT;
            printf("SONG POSITION: %d\n", song_position);
            continue;
        }
    }
}

Event<32> test_event{};

void handle_event()
{
    printf("Handle Event!\n");
}

void handle_event_2()
{
    printf("Handle Event 2!\n");
}

int main()
{
    stdio_init_all();
    board_init();

    tusb_init();
    tud_init(BOARD_TUD_RHPORT);

    blinker.init();

    test_event.connect(handle_event);
    test_event.connect(handle_event_2);
    test_event.emit();

    while (true)
    {
        tud_task();
        midi_task();
        blinker.task();
    }
}
