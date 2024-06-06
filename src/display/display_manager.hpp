#pragma once

#include "channel_display.hpp"
#include "display_output.hpp"

#include "messages/messages.hpp"
#include "midi/midi.hpp"

#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/shapeRenderer/ShapeRenderer.h"

#include "hardware/i2c.h"

#include <memory>

constexpr auto SSD1306_I2C_SDA_PIN = 7;
constexpr auto SSD1306_I2C_SCL_PIN = 6;
constexpr auto SSD1306_ADDR = 0x3C;

constexpr auto NUM_CHANNELS = 4;

class DisplayManager
{
    std::shared_ptr<pico_ssd1306::SSD1306> ssd1306;
    std::array<ChannelDisplay *, NUM_CHANNELS> channel_displays;
    DisplayOutput output;

public:
    void set_channel(uint8_t channel, ChannelDisplay *channel_display)
    {
        channel_displays[channel] = channel_display;
    }

    void init()
    {
        i2c_init(i2c1, 1000000);
        gpio_set_function(SSD1306_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(SSD1306_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(SSD1306_I2C_SDA_PIN);
        gpio_pull_up(SSD1306_I2C_SCL_PIN);

        ssd1306 = std::make_unique<pico_ssd1306::SSD1306>(i2c1, SSD1306_ADDR, pico_ssd1306::Size::W128xH64);
        ssd1306->setOrientation(false);
        output = DisplayOutput{ssd1306};
    }

    void task()
    {
        ssd1306->clear();

        draw_channel_frames();

        for (size_t i = 0; i < NUM_CHANNELS; ++i)
        {
            auto channel_display = channel_displays[i];
            if (channel_display != nullptr)
            {
                channel_display->draw_channel(i, output);
            }
        }

        ssd1306->sendBuffer();
    }

private:
    void draw_channel_frames()
    {
        pico_ssd1306::drawLine(ssd1306.get(), CHANNEL_WIDTH * 1, 0, CHANNEL_WIDTH * 1, CHANNEL_HEIGHT);
        pico_ssd1306::drawLine(ssd1306.get(), CHANNEL_WIDTH * 2, 0, CHANNEL_WIDTH * 2, CHANNEL_HEIGHT);
        pico_ssd1306::drawLine(ssd1306.get(), CHANNEL_WIDTH * 3, 0, CHANNEL_WIDTH * 3, CHANNEL_HEIGHT);
        pico_ssd1306::drawLine(ssd1306.get(), CHANNEL_WIDTH * 4, 0, CHANNEL_WIDTH * 4, CHANNEL_HEIGHT);
    }
};