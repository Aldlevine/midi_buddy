#pragma once

#include "pico/stdlib.h"

#include <array>

/**
 * An enum of all possible image formats
 * Currently only GS4_HMSB is supported.
 *
 * - GS4_HMSB: 4 bit grayscale, with upper-nibble on the left, and lower-nibble on the right
 */
enum class ImageFormat
{
    GS4_HMSB,
};

/**
 * Used to specify at what ratio an image's `data_t` maps to pixels.
 * For example, if `data_t = uint8_t` and each pixel is 4 bits,
 * we can specify a ratio of 1/2 or `ImagePxRatio{1, 2}` (1 byte / 2 pixels).
 */
struct ImagePxRatio
{
    size_t numerator{1};
    size_t denominator{1};
};

/**
 * Should be specialized for each image format
 */
template <ImageFormat fmt_>
struct ImageFormatTraits
{
    using blend_mode_t = void;
    using data_t = uint8_t;
    static constexpr ImagePxRatio px_ratio{};
    static constexpr data_t max_value{};
};

/**
 * Represents the core image logic
 * Uses CRTP to support specialization
 * Additional image formats must implement get_pixel and set_pixel
 * Other methods may be specialized as well
 */
template <ImageFormat fmt_, size_t width_, size_t height_, typename derived_ = void>
struct Image
{
    using derived_t = derived_;
    using data_t = ImageFormatTraits<fmt_>::data_t;
    using blend_mode_t = ImageFormatTraits<fmt_>::blend_mode_t;
    static constexpr auto fmt = fmt_;
    static constexpr auto px_ratio = ImageFormatTraits<fmt>::px_ratio;
    static constexpr auto max_value = ImageFormatTraits<fmt>::max_value;
    static constexpr auto width = width_;
    static constexpr auto height = height_;

    std::array<data_t, (width * height * px_ratio.numerator) / px_ratio.denominator> buffer{};

    Image() = default;
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;
    Image(Image &&) = default;
    Image &operator=(Image &&) = default;

    template <typename... data_args>
    Image(data_args... data) : buffer{data...} {}

    data_t blend_pixels(blend_mode_t blend_mode, data_t dst, data_t src) = delete;
    data_t get_pixel(size_t x, size_t y) const = delete;
    void set_pixel(size_t x, size_t y, data_t value, blend_mode_t blend_mode = {}) = delete;

    const auto &get_buffer() const
    {
        return buffer;
    }

    auto &get_buffer()
    {
        return buffer;
    }

    void clear();

    void fill(data_t value, blend_mode_t blend_mode = {});

    template <size_t other_width_, size_t other_height_>
    void blit(
        size_t dst_x, size_t dst_y,
        const Image<fmt, other_width_, other_height_> &src,
        size_t src_x, size_t src_y,
        size_t src_w, size_t src_h,
        size_t value = max_value,
        blend_mode_t blend_mode = {});
};

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

template <ImageFormat fmt_, size_t width_, size_t height_, typename derived_>
inline void Image<fmt_, width_, height_, derived_>::clear()
{
    this->buffer.fill(data_t{});
}

template <ImageFormat fmt_, size_t width_, size_t height_, typename derived_>
inline void Image<fmt_, width_, height_, derived_>::fill(data_t value, blend_mode_t blend_mode)
{
    for (size_t x = 0; x < width; ++x)
    {
        for (size_t y = 0; y < height; ++y)
        {
            static_cast<derived_t *>(this)->set_pixel(x, y, value);
        }
    }
}

template <ImageFormat fmt_, size_t width_, size_t height_, typename derived_>
template <size_t other_width_, size_t other_height_>
inline void Image<fmt_, width_, height_, derived_>::blit(
    size_t dst_x, size_t dst_y,
    const Image<fmt, other_width_, other_height_> &src,
    size_t src_x, size_t src_y,
    size_t src_w, size_t src_h,
    size_t value,
    blend_mode_t blend_mode)
{
    // clang-format off
    if (src_x == -1) { src_x = 0; }
    if (src_y == -1) { src_y = 0; }
    if (src_w == -1) { src_w = src.width; }
    if (src_h == -1) { src_h = src.height; }
    // clang-format on

    assert(dst_x + src_w <= width);
    assert(dst_y + src_h <= height);

    for (size_t sy = 0; sy < src_h; ++sy)
    {
        for (size_t sx = 0; sx < src_w; ++sx)
        {
            auto src_pixel = src.get_pixel(src_x + sx, src_y + sy);
            static_cast<derived_t *>(this)->set_pixel(dst_x + sx, dst_y + sy, (src_pixel * value) / max_value, blend_mode);
        }
    }
}
