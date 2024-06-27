#pragma once

#include "graphics/image.hpp"

#include <algorithm>

template <>
struct ImageFormatTraits<ImageFormat::GS4_HMSB>
{
    using data_t = uint8_t;
    static constexpr ImagePxRatio px_ratio{1, 2};
    static constexpr uint8_t max_value{0xF};

    enum class blend_mode_t
    {
        SRC,
        ADD,
        SUB,
        MUL,
        DIV,
        SCISSOR,
    };
};

template <size_t width_, size_t height_>
struct Image<ImageFormat::GS4_HMSB, width_, height_>
    : public Image<ImageFormat::GS4_HMSB, width_, height_, Image<ImageFormat::GS4_HMSB, width_, height_>>
{
    using this_t = Image<ImageFormat::GS4_HMSB, width_, height_>;
    using base_t = Image<ImageFormat::GS4_HMSB, width_, height_, this_t>;

    using typename base_t::blend_mode_t;
    using typename base_t::data_t;

    using base_t::Image;

    using base_t::buffer;
    using base_t::fmt;
    using base_t::height;
    using base_t::max_value;
    using base_t::px_ratio;
    using base_t::width;

    void fill(data_t value, blend_mode_t blend_mode = {})
    {
        if (blend_mode == blend_mode_t::SRC)
        {
            value = (value & 0xF) | (value << 4);
            std::fill(std::begin(buffer), std::end(buffer), value);
        }
        else
        {
            static_cast<base_t *>(this)->fill(value, blend_mode);
        }
    }

    data_t blend_pixels(blend_mode_t blend_mode, data_t dst, data_t src)
    {
        switch (blend_mode)
        {
        case blend_mode_t::SRC:
            return src;
            break;

        case blend_mode_t::ADD:
            return std::min(data_t(dst + src), max_value);
            break;

        case blend_mode_t::SUB:
            if (src > dst)
            {
                return 0;
            }
            return dst - src;
            break;

        case blend_mode_t::MUL:
            return std::min(data_t((dst * src) / max_value), max_value);
            break;

        case blend_mode_t::DIV:
            return std::min(data_t((dst * max_value) / src), max_value);
            break;

        case blend_mode_t::SCISSOR:
            if (src > 0)
            {
                return src;
            }
            return dst;
        }

        return src;
    }

    data_t get_pixel(size_t x, size_t y) const
    {
        assert(x < width);
        assert(y < height);

        auto index = ((width * y) + x) / 2;

        if (x % 2 == 0)
        {
            return this->buffer[index] >> 4;
        }
        else
        {
            return this->buffer[index] & 0x0F;
        }
    }

    void set_pixel(size_t x, size_t y, data_t value, blend_mode_t blend_mode = {})
    {
        assert(x < width);
        assert(y < height);
        assert(value <= 0xF);

        auto index = ((width * y) + x) / 2;

        if (x % 2 == 0)
        {
            auto dst = (this->buffer[index] & 0xF0) >> 4;
            auto tgt = blend_pixels(blend_mode, dst, value);
            this->buffer[index] = (this->buffer[index] & 0x0F) | (tgt << 4);
        }
        else
        {
            auto dst = (this->buffer[index] & 0x0F);
            auto tgt = blend_pixels(blend_mode, dst, value);
            this->buffer[index] = (this->buffer[index] & 0xF0) | (tgt & 0x0F);
        }
    }
};
