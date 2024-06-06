#pragma once

#include "display/channel_display.hpp"

class DebugChannelDisplay : public virtual ChannelDisplay
{

public:
    DebugChannelDisplay() {}

    ~DebugChannelDisplay() override
    {
    }

    // void draw_channel(uint8_t channel, pico_ssd1306::SSD1306 &ssd1306) override
    void draw_channel(uint8_t channel, DisplayOutput &output) override
    {
        uint8_t cx_start = channel * CHANNEL_WIDTH;
        uint8_t cx_end = cx_start + CHANNEL_WIDTH;
        // uint8_t cx_mid = cx + CHANNEL_WIDTH / 2;
        uint8_t cy_start = 0;
        uint8_t cy_end = CHANNEL_HEIGHT;

        // draw_rect(ssd1306, cx_start, cy_start, cx_end, cy_end, 4);
        output.draw_rect(cx_start, cy_start, cx_end, cy_end, 4);
    }
};