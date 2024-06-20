#pragma once

#include "image_format.hpp"

#include "pico/stdlib.h"

#include <array>

/**
 * Represents the core image logic
 */
template <ImageFormat fmt_, size_t width_, size_t height_>
class ImageBase
{
public:
    using data_t = ImageFormatTraits<fmt_>::data_t;
    static constexpr auto fmt = fmt_;
    static constexpr auto px_ratio = ImageFormatTraits<fmt>::px_ratio;
    static constexpr auto max_contrast = ImageFormatTraits<fmt>::max_contrast;
    static constexpr auto width = width_;
    static constexpr auto height = height_;

protected:
    std::array<data_t, (width * height * px_ratio.numerator) / px_ratio.denominator> buffer{};

public:
    ImageBase() = default;
    ImageBase(const ImageBase &) = delete;
    ImageBase &operator=(const ImageBase &) = delete;
    ImageBase(ImageBase &&) = default;
    ImageBase &operator=(ImageBase &&) = default;

    template <typename... data_args>
    ImageBase(data_args... data) : buffer{data...} {}

    virtual ~ImageBase() {}

    const auto &get_buffer() const
    {
        return buffer;
    }

    auto &get_buffer()
    {
        return buffer;
    }

    virtual void clear()
    {
        this->buffer.fill(data_t{});
    }

    virtual void fill(data_t value)
    {
        for (size_t x = 0; x < width; ++x)
        {
            for (size_t y = 0; y < height; ++y)
            {
                this->set_pixel(x, y, value);
            }
        }
    }

    template <size_t other_width_, size_t other_height_>
    void blit(
        size_t dst_x, size_t dst_y,
        const ImageBase<fmt, other_width_, other_height_> &src,
        size_t src_x, size_t src_y,
        size_t src_w, size_t src_h,
        size_t contrast = max_contrast)
    {
        // clang-format off
        if (src_x == -1) { src_x = 0; }
        if (src_y == -1) { src_y = 0; }
        if (src_w == -1) { src_w = src.width; }
        if (src_h == -1) { src_h = src.height; }
        // clang-format on

        for (size_t sy = 0; sy < src_h; ++sy)
        {
            for (size_t sx = 0; sx < src_w; ++sx)
            {
                auto src_pixel = src.get_pixel(src_x + sx, src_y + sy);
                this->set_pixel(dst_x + sx, dst_y + sy, (src_pixel * contrast) / max_contrast);
            }
        }
    }

    virtual data_t get_pixel(size_t x, size_t y) const
    {
        return data_t{};
    }

    virtual void set_pixel(size_t x, size_t y, data_t value)
    {
    }
};

template <ImageFormat fmt_, size_t width_, size_t height_>
class Image;

template <typename>
constexpr bool is_image_v = false;

template <ImageFormat fmt_, size_t width_, size_t height_>
constexpr bool is_image_v<Image<fmt_, width_, height_>> = true;

template <typename image_>
concept is_image = is_image_v<std::remove_cvref_t<image_>>;

template <ImageFormat, typename>
constexpr bool is_image_fmt_v = false;

template <ImageFormat check_fmt_, ImageFormat img_fmt_, size_t width_, size_t height_>
constexpr bool is_image_fmt_v<check_fmt_, Image<img_fmt_, width_, height_>> = false;

template <ImageFormat fmt_, typename image_>
concept is_image_fmt = is_image_fmt_v<fmt_, image_>;