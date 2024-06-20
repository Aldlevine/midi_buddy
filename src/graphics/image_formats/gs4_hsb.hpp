#pragma once

#include "graphics/image.hpp"

template <>
struct ImageFormatTraits<ImageFormat::GS4_HSB>
{
    using data_t = uint8_t;
    static constexpr ImagePxRatio px_ratio{1, 2};
    static constexpr uint8_t max_contrast{0xF};
};

// Specialize for GS4_HSB
template <size_t width_, size_t height_>
class Image<ImageFormat::GS4_HSB, width_, height_>
    : public ImageBase<ImageFormat::GS4_HSB, width_, height_>
{
public:
    using ImageBase<ImageFormat::GS4_HSB, width_, height_>::ImageBase;

    ~Image() {}

    uint8_t get_pixel(size_t x, size_t y) const override
    {
        auto index = ((Image<ImageFormat::GS4_HSB, width_, height_>::width * y) + x) / 2;

        if (x % 2 == 0)
        {
            return this->buffer[index] >> 4;
        }
        else
        {
            return this->buffer[index] & 0x0F;
        }
    }

    void set_pixel(size_t x, size_t y, uint8_t value) override
    {
        auto index = ((Image<ImageFormat::GS4_HSB, width_, height_>::width * y) + x) / 2;

        if (x % 2 == 0)
        {
            this->buffer[index] = (this->buffer[index] & 0x0F) | (value << 4);
        }
        else
        {
            this->buffer[index] = (this->buffer[index] & 0xF0) | (value & 0x0F);
        }
    }
};
