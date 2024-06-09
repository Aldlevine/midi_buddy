#pragma once

#include "display_output.hpp"

#include "pico-ssd1306/ssd1306.h"

constexpr uint8_t CHANNEL_WIDTH = 28;
constexpr uint8_t CHANNEL_HEIGHT = 64;

class ChannelDisplay
{
public:
    virtual ~ChannelDisplay() {}
    virtual void draw_channel(uint8_t channel, DisplayOutput &output) {}
};