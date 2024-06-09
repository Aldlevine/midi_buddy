#pragma once

#include "display/channel_display.hpp"
#include "messages/messages.hpp"
#include "midi/midi.hpp"

class ClockChannelDisplay : public virtual ChannelDisplay
{
    MessageConsumer<ClockMessage> clock_mc;
    ClockMessage cur_clock;

public:
    ClockChannelDisplay(MessageQueue<ClockMessage> &clock_mq) : clock_mc{clock_mq} {}

    ~ClockChannelDisplay() override
    {
    }

    void draw_channel(uint8_t channel, DisplayOutput &output) override
    {
        {
            auto lock_guard = clock_mc.lock_guard();
            clock_mc.get_last_message(&cur_clock);
        }

        uint8_t clock_mod = cur_clock.clock % CLOCKS_PER_BEAT;

        uint8_t cx = channel * CHANNEL_WIDTH;
        uint8_t cx_mid = cx + CHANNEL_WIDTH / 2;

        if (clock_mod < 4)
        {
            output.fill_rect(cx_mid - 4, 8, cx_mid + 4, 16);
        }
        else if (clock_mod < 8)
        {
            uint8_t y_mid = 12;
            output.fill_rect(cx_mid - 4, y_mid - 4, cx_mid - 2, y_mid - 2);
            output.fill_rect(cx_mid + 2, y_mid - 4, cx_mid + 4, y_mid - 2);
            output.fill_rect(cx_mid + 2, y_mid + 2, cx_mid + 4, y_mid + 4);
            output.fill_rect(cx_mid - 4, y_mid + 2, cx_mid - 2, y_mid + 4);
        }

        // beat dropper
        {
            uint8_t clock_y = 11 + 48 - clock_mod * 2;
            uint8_t clock_y_end = clock_y + 2;
            output.fill_rect(cx_mid - 1, clock_y, cx_mid + 1, clock_y_end);
        }
    }
};