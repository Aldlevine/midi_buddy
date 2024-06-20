#pragma once

#include "image.hpp"
#include "font.hpp"

#include <array>

template <is_font font_>
struct TextRenderer
{
    using font_t = font_;

    const font_t &font;
    const size_t letter_spacing = 1;
    const size_t word_spacing = font.width;
    const size_t line_height = font.height + 1;

    template <is_image image_>
    void blit_string(image_ &dst, size_t dst_x, size_t dst_y, const char *str, size_t contrast = font_t::image_t::max_contrast)
    {
        auto start_x = dst_x;
        for (auto ch_ptr = reinterpret_cast<const uint8_t *>(str); *ch_ptr != '\0'; ++ch_ptr)
        {
            const auto &ch = *ch_ptr;

            if (ch == '\n')
            {
                dst_y += line_height;
                dst_x = start_x;
                continue;
            }

            if (ch == ' ')
            {
                dst_x += word_spacing;
                continue;
            }

            if (ch > ' ')
            {
                GlyphRect glyph_rect = font.blit_glyph(dst, dst_x, dst_y, ch);
                dst_x += glyph_rect.w + letter_spacing;
                continue;
            }
        }
    }
};

template <typename>
constexpr bool is_text_renderer_v = false;

template <is_font font_>
constexpr bool is_text_renderer_v<TextRenderer<font_>> = true;

template <typename text_renderer_>
concept is_text_renderer = is_text_renderer_v<std::remove_cvref_t<text_renderer_>>;