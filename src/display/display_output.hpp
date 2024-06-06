#pragma once

#include "pico-ssd1306/ssd1306.h"

#include <memory>

constexpr uint8_t bayer_4x4[4][4] = {
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}};

class DisplayOutput
{
    std::shared_ptr<pico_ssd1306::SSD1306> ssd1306;

public:
    DisplayOutput() = default;
    DisplayOutput(std::shared_ptr<pico_ssd1306::SSD1306> ssd1306) : ssd1306{ssd1306} {}

    void fill_rect(
        uint8_t x_start, uint8_t y_start,
        uint8_t x_end, uint8_t y_end,
        uint8_t dither = 0,
        pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD)
    {
        for (uint8_t x = x_start; x <= x_end; x++)
        {
            for (uint8_t y = y_start; y <= y_end; y++)
            {
                if (bayer_4x4[y % 4][x % 4] >= dither)
                {
                    ssd1306->setPixel(x, y, mode);
                }
            }
        }
    }

    void draw_rect(
        uint8_t x_start, uint8_t y_start,
        uint8_t x_end, uint8_t y_end,
        uint8_t stroke_width = 1,
        pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD)
    {
        if (stroke_width < 1)
        {
            return;
        }

        for (uint8_t x = x_start; x < x_end; ++x)
        {
            ssd1306->setPixel(x, y_start);
            ssd1306->setPixel(x, y_end);
        }

        for (uint8_t y = y_start; y < y_end; ++y)
        {
            ssd1306->setPixel(x_start, y);
            ssd1306->setPixel(x_end, y);
        }

        draw_rect(x_start + 1, y_start + 1, x_end - 1, y_end - 1, stroke_width - 1, mode);
    }
};