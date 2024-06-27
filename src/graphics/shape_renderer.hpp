#pragma once

#include "image.hpp"

#include <math.h>

struct ShapeRenderer
{
    template <is_image image_>
    void draw_line(
        image_ &dst,
        size_t x0, size_t y0,
        size_t x1, size_t y1,
        image_::data_t value = image_::max_value,
        image_::blend_mode_t blend_mode = {})
    {
        // normalize left-right top-down

        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        else if (x0 == x1 && y0 > y1)
        {
            std::swap(y0, y1);
        }

        // edge case horizontal

        if (y0 == y1)
        {
            for (size_t x = x0; x <= x1; ++x)
            {
                dst.set_pixel(x, y0, value, blend_mode);
            }
            return;
        }

        // edge case vertical

        if (x0 == x1)
        {
            for (size_t y = y0; y <= y1; ++y)
            {
                dst.set_pixel(x0, y, value, blend_mode);
            }
            return;
        }

        // base case DDA

        float_t x_current = static_cast<float_t>(x0);
        float_t y_current = static_cast<float_t>(y0);

        float_t x_delta = static_cast<float_t>(x1) - x_current;
        float_t y_delta = static_cast<float_t>(y1) - y_current;
        float_t step = std::max(x_delta, std::abs(y_delta));

        float_t x_increment = x_delta / step;
        float_t y_increment = y_delta / step;

        for (size_t i = 0; i <= step; ++i)
        {
            size_t x_trunc = std::floor(x_current + 0.5);
            size_t y_trunc = std::floor(y_current + 0.5);

            dst.set_pixel(x_trunc, y_trunc, value, blend_mode);

            x_current += x_increment;
            y_current += y_increment;
        }
    }

    template <is_image image_>
    void draw_rect(
        image_ &dst,
        size_t x, size_t y,
        size_t w, size_t h,
        image_::data_t value = image_::max_value,
        image_::blend_mode_t blend_mode = {})
    {
        draw_line(dst, x + 0, y + 0, x + w, y + 0, value, blend_mode);
        draw_line(dst, x + w, y + 0, x + w, y + h, value, blend_mode);
        draw_line(dst, x + w, y + h, x + 0, y + h, value, blend_mode);
        draw_line(dst, x + 0, y + h, x + 0, y + 0, value, blend_mode);
    }

    template <is_image image_>
    void fill_rect(
        image_ &dst,
        size_t x, size_t y,
        size_t w, size_t h,
        image_::data_t value = image_::max_value,
        image_::blend_mode_t blend_mode = {})
    {
        for (size_t x_current = x; x_current <= x + w; ++x_current)
        {
            for (size_t y_current = y; y_current <= y + h; ++y_current)
            {
                dst.set_pixel(x_current, y_current, value, blend_mode);
            }
        }
    }

    template <is_image image_>
    void draw_circle(
        image_ &dst,
        size_t x_center, size_t y_center, size_t radius,
        image_::data_t value = image_::max_value,
        image_::blend_mode_t blend_mode = {})
    {
        int32_t x = 0;
        int32_t y = radius;
        int32_t p = 1 - radius;

        do
        {
            dst.set_pixel(x_center + x, y_center + y, value, blend_mode);
            dst.set_pixel(x_center - x, y_center + y, value, blend_mode);
            dst.set_pixel(x_center + x, y_center - y, value, blend_mode);
            dst.set_pixel(x_center - x, y_center - y, value, blend_mode);
            dst.set_pixel(x_center + y, y_center + x, value, blend_mode);
            dst.set_pixel(x_center - y, y_center + x, value, blend_mode);
            dst.set_pixel(x_center + y, y_center - x, value, blend_mode);
            dst.set_pixel(x_center - y, y_center - x, value, blend_mode);

            ++x;
            if (p < 0)
            {
                p += 2 * x + 1;
            }
            else
            {
                --y;
                p += 2 * (x - y) + 1;
            }

        } while (x <= y);
    }

    template <is_image image_>
    void fill_circle(
        image_ &dst,
        size_t x_center, size_t y_center, size_t radius,
        image_::data_t value = image_::max_value,
        image_::blend_mode_t blend_mode = {})
    {
        int32_t x = 0;
        int32_t y = radius;
        int32_t p = 1 - radius;

        std::array<bool, image_::height> y_handled{};

        do
        {

            auto y_ys = y_center - y;
            auto y_ya = y_center + y;
            auto y_xs = y_center - x;
            auto y_xa = y_center + x;

            if (!y_handled[y_ys])
            {
                draw_line(dst, x_center - x, y_ys, x_center + x, y_ys, value, blend_mode);
                y_handled[y_ys] = true;
            }

            if (!y_handled[y_ya])
            {
                draw_line(dst, x_center - x, y_ya, x_center + x, y_ya, value, blend_mode);
                y_handled[y_ya] = true;
            }

            if (!y_handled[y_xs])
            {
                draw_line(dst, x_center - y, y_xs, x_center + y, y_xs, value, blend_mode);
                y_handled[y_xs] = true;
            }

            if (!y_handled[y_xa])
            {
                draw_line(dst, x_center - y, y_xa, x_center + y, y_xa, value, blend_mode);
                y_handled[y_xa] = true;
            }

            ++x;
            if (p < 0)
            {
                p += 2 * x + 1;
            }
            else
            {
                --y;
                p += 2 * (x - y) + 1;
            }

        } while (x <= y);
    }
};
