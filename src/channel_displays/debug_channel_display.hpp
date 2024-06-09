#pragma once

#include "display/channel_display.hpp"

class DebugChannelDisplay : public virtual ChannelDisplay
{

public:
    DebugChannelDisplay() {}

    ~DebugChannelDisplay() override
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

        output.draw_circle(cx_mid, cy_mid, CHANNEL_WIDTH / 3);
    }
};