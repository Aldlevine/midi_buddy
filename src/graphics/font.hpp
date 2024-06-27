#pragma once

#include "image.hpp"

#include <array>

/**
 * Describes the location and size of a glyph within the source image
 */
struct GlyphRect
{
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint8_t offs_x = 0;
    uint8_t offs_y = 0;
};

/**
 * Supports ASCII codepoints 0x21 to 0x7E (all printable ASCII characters, except space " ")
 */
struct GlyphRectMap
{
    std::array<GlyphRect, 0x7E - 0x20> map; // we skip all non-visible characters

    GlyphRect get_glyph_rect(uint8_t glyph) const
    {
        assert(glyph >= 0x21 && glyph <= 0xFF);
        return map[glyph - 0x21];
    }
};

/**
 * Encapsulates a GlyphRectMap and an image
 */
template <is_image image_>
struct Font
{
    using image_t = image_;

    const image_t &image;
    const GlyphRectMap glyph_rect_map;

    const uint8_t height = [&]()
    {
        uint8_t result = 0;
        for (auto rect : glyph_rect_map.map)
        {
            result = std::max(result, rect.h);
        }
        return result;
    }();

    const uint8_t width = [&]()
    {
        uint8_t result = 0;
        for (auto rect : glyph_rect_map.map)
        {
            result = std::max(result, rect.w);
        }
        return result;
    }();

    template <is_image dst_image_>
    const GlyphRect &draw_glyph(
        dst_image_ &dst,
        size_t dst_x, size_t dst_y, uint8_t ch,
        size_t value = image_t::max_value,
        image_t::blend_mode_t blend_mode = {}) const
    {
        const auto &glyph_rect = glyph_rect_map.get_glyph_rect(ch);
        dst.blit(
            dst_x + glyph_rect.offs_x,
            dst_y + glyph_rect.offs_y,
            image,
            glyph_rect.x,
            glyph_rect.y,
            glyph_rect.w,
            glyph_rect.h,
            value,
            blend_mode);
        return glyph_rect;
    }
};

template <typename>
constexpr bool is_font_v = false;

template <typename image_>
constexpr bool is_font_v<Font<image_>> = true;

template <typename font_>
concept is_font = is_font_v<std::remove_cvref_t<font_>>;
