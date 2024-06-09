#pragma once

#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/shapeRenderer/ShapeRenderer.h"

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

    void set_pixel(
        uint8_t x, uint8_t y,
        pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD)
    {
        ssd1306->setPixel(x, y, mode);
    }

    void draw_line(
        uint8_t x_start, uint8_t y_start,
        uint8_t x_end, uint8_t y_end,
        pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD)
    {
        pico_ssd1306::drawLine(ssd1306.get(), x_start, y_start, x_end, y_end, mode);
    }

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
                    set_pixel(x, y, mode);
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

        for (uint8_t x = x_start; x <= x_end; ++x)
        {
            set_pixel(x, y_start);
            set_pixel(x, y_end);
        }

        for (uint8_t y = y_start; y <= y_end; ++y)
        {
            set_pixel(x_start, y);
            set_pixel(x_end, y);
        }

        draw_rect(x_start + 1, y_start + 1, x_end - 1, y_end - 1, stroke_width - 1, mode);
    }

    void draw_circle(
        uint8_t cx, uint8_t cy, uint8_t r,
        uint8_t stroke_width = 1,
        pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD)
    {
        int16_t t1 = r / 16;
        int16_t x = r;
        int16_t y = 0;
        while (x > y)
        {
            set_pixel(cx + x, cy + y, mode);
            set_pixel(cx - x, cy + y, mode);
            set_pixel(cx + x, cy - y, mode);
            set_pixel(cx - x, cy - y, mode);
            set_pixel(cx + y, cy + x, mode);
            set_pixel(cx - y, cy + x, mode);
            set_pixel(cx + y, cy - x, mode);
            set_pixel(cx - y, cy - x, mode);

            ++y;
            t1 = t1 + y;
            int16_t t2 = t1 - x;
            if (t2 > 0)
            {
                t1 = t2;
                --x;
            }
        }
    }
};