#include "channel_displays/channel_displays.hpp"
#include "concurrent/concurrent.hpp"
#include "display/display.hpp"
#include "messages/messages.hpp"
#include "midi/midi.hpp"
#include "note_tracker/note_tracker.hpp"

#include "bsp/board.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <stdio.h>

auto midi_receiver = MIDIReceiver{};
auto note_tracker = NoteTracker{midi_receiver.get_note_mq(), 2};

auto display_manager = DisplayManager{};
auto clock_channel_display = ClockChannelDisplay{midi_receiver.get_clock_mq()};
auto note_channel_display = NoteChannelDisplay{note_tracker};
auto debug_channel_display = DebugChannelDisplay{};

void secondary_main()
{
    while (true)
    {
        display_manager.task();
    }
}

int main()
{
    stdio_init_all();
    board_init();

    tusb_init();
    tud_init(BOARD_TUD_RHPORT);

    display_manager.init();
    display_manager.set_channel(0, &note_channel_display);
    display_manager.set_channel(1, &debug_channel_display);
    display_manager.set_channel(3, &clock_channel_display);

    midi_receiver.init();

    multicore_launch_core1(secondary_main);

    while (true)
    {
        tud_task();
        midi_receiver.task();
        note_tracker.task();
    }
}
