#pragma once

#include "display/channel_display.hpp"

#include "messages/messages.hpp"

// MessageConsumer<ClockMessage> clock_mc;
// ClockMessage cur_clock;

// public:
// ClockChannelDisplay(MessageQueue<ClockMessage> &clock_mq) : clock_mc{clock_mq} {}
class MIDICCChannelDisplay : public virtual ChannelDisplay
{
    uint8_t control_id;

    MessageConsumer<ControlMessage> control_mc;
    ControlMessage cur_control;
    ControlMessage cur_control_unfiltered;

public:
    MIDICCChannelDisplay(MessageQueue<ControlMessage> &control_mq, uint8_t control_id)
        : control_mc{control_mq}, control_id{control_id}
    {
    }

    ~MIDICCChannelDisplay() override
    {
    }

    void draw_channel(uint8_t channel, DisplayOutput &output) override
    {
        uint8_t cx_start = channel * CHANNEL_WIDTH;
        uint8_t cx_mid = cx_start + CHANNEL_WIDTH / 2;
        uint8_t cx_end = cx_start + CHANNEL_WIDTH;
        uint8_t cy_start = 0;
        uint8_t cy_mid = cy_start + CHANNEL_HEIGHT / 2;
        uint8_t cy_end = CHANNEL_HEIGHT;

        uint8_t radius = CHANNEL_WIDTH / 3;
        uint8_t y_min = cy_start + radius;
        uint8_t y_max = cy_end - radius - 1;

        {
            auto control_lock = control_mc.lock_guard();
            while (control_mc.get_message(&cur_control_unfiltered))
            {
                if (cur_control_unfiltered.control_id == control_id)
                {
                    cur_control = cur_control_unfiltered;
                }
            }
        }

        uint8_t y = (cur_control.value * (y_min - y_max)) / 127 + y_max;

        if (y + radius < y_max)
        {
            output.draw_line(cx_mid, y_max, cx_mid, y + radius);
        }
        if (y - radius > y_min)
        {
            output.draw_line(cx_mid, y - radius, cx_mid, y_min);
        }
        // output.draw_circle(cx_mid, y, CHANNEL_WIDTH / 3, 1, pico_ssd1306::WriteMode::SUBTRACT);
        output.draw_circle(cx_mid, y, CHANNEL_WIDTH / 3);
    }
};
