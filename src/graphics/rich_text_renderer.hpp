#pragma once

#include "font.hpp"

template <is_font normal_font_, is_font bold_font_, is_font italic_font_, is_font bold_italic_font_>
struct RichTextRenderer
{
    using normal_font_t = normal_font_;
    using bold_font_t = bold_font_;
    using italic_font_t = italic_font_;
    using bold_italic_font_t = bold_italic_font_;

    static_assert(std::is_same_v<typename normal_font_t::image_t::data_t, typename bold_font_t::image_t::data_t>);
    static_assert(std::is_same_v<typename normal_font_t::image_t::data_t, typename italic_font_t::image_t::data_t>);
    static_assert(std::is_same_v<typename normal_font_t::image_t::data_t, typename bold_italic_font_t::image_t::data_t>);

    struct Tag
    {
        bool bold = false;
        bool italic = false;
        bool underline = false;
        normal_font_t::image_t::data_t value = normal_font_t::image_t::max_value;
    };

    using tag_stack_t = std::array<Tag, 32>;

    const normal_font_t &normal_font;
    const bold_font_t &bold_font = normal_font;
    const italic_font_t &italic_font = normal_font;
    const bold_italic_font_t &bold_italic_font = normal_font;

    const size_t letter_spacing = 1;
    const size_t word_spacing = normal_font.width;
    const size_t line_height = normal_font.height + 1;
    const size_t underline_height = normal_font.height - 1;

    tag_stack_t tag_stack{};
    Tag *head = tag_stack.begin();

    template <is_image image_>
    void draw_string(
        image_ &dst,
        size_t dst_x,
        size_t dst_y,
        const char *str,
        image_::blend_mode_t blend_mode = {})
    {

        if (!process_tag_and_move(str))
        {
            return;
        }

        auto start_x = dst_x;
        do
        {
            const auto &ch = *str;

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
                GlyphRect glyph_rect = draw_glyph(dst, dst_x, dst_y, ch, blend_mode);
                dst_x += glyph_rect.w + letter_spacing;
                continue;
            }
        } while (move_to_next_char(str));
    }

private:
    auto draw_glyph(
        auto &dst,
        size_t dst_x,
        size_t dst_y,
        uint8_t ch,
        normal_font_t::image_t::blend_mode_t blend_mode = {})
    {
        const auto value = get_value();
        GlyphRect glyph_rect;

        if (head->bold && head->italic)
        {
            glyph_rect = bold_italic_font.draw_glyph(dst, dst_x, dst_y, ch, value, blend_mode);
        }

        else if (head->bold)
        {
            glyph_rect = bold_font.draw_glyph(dst, dst_x, dst_y, ch, value, blend_mode);
        }

        else if (head->italic)
        {
            glyph_rect = italic_font.draw_glyph(dst, dst_x, dst_y, ch, value, blend_mode);
        }

        else
        {
            glyph_rect = normal_font.draw_glyph(dst, dst_x, dst_y, ch, value, blend_mode);
        }

        if (head->underline)
        {
            for (size_t x = dst_x + glyph_rect.offs_x - 1; x < dst_x + glyph_rect.offs_x + glyph_rect.w + letter_spacing; ++x)
            {
                dst.set_pixel(x, dst_y + underline_height, value, blend_mode);
            }
        }

        return glyph_rect;
    }

    normal_font_t::image_t::data_t get_value()
    {
        return head->value;
    }

    void eat_tag(const char *&str)
    {
        while (*(++str) != ']')
            ;
    }

    void process_tag(const char *&str)
    {
        // close
        if (*str == '/')
        {
            --head;
        }

        // bold
        else if (*str == 'b')
        {
            auto cur_tag = *head;
            ++head;
            *head = cur_tag;
            head->bold = true;
        }

        // italic
        else if (*str == 'i')
        {
            auto cur_tag = *head;
            ++head;
            *head = cur_tag;
            head->italic = true;
        }

        // underline
        else if (*str == 'u')
        {
            auto cur_tag = *head;
            ++head;
            *head = cur_tag;
            head->underline = true;
        }

        // value
        else if (*str == 'v')
        {
            auto cur_tag = *head;
            ++head;
            *head = cur_tag;
            ++str;
            head->value = *str;
        }

        eat_tag(str);
        return;
    }

    bool process_tag_and_move(const char *&str)
    {
        if (*str == '[') // maybe tag
        {

            ++str;
            if (*str == '[') // we have an escaped "[" character
            {
                return true;
            }
            process_tag(str);
            return move_to_next_char(str);
        }

        return true;
    }

    bool move_to_next_char(const char *&str)
    {
        if (*str == '\0')
        {
            return false;
        }

        ++str;

        return process_tag_and_move(str);
    }
};

template <typename>
constexpr bool is_rich_text_renderer_v = false;

template <is_font normal_font_, is_font bold_font_, is_font italic_font_, is_font bold_italic_font_>
constexpr bool is_rich_text_renderer_v<RichTextRenderer<normal_font_, bold_font_, italic_font_, bold_italic_font_>> = true;

template <typename rich_text_renderer_>
concept is_rich_text_renderer = is_rich_text_renderer_v<std::remove_cvref_t<rich_text_renderer_>>;