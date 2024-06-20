#include "images/images.hpp"
#include "fonts/fonts.hpp"

#include "graphics/graphics.hpp"
#include "drivers/sh1122/sh1122.hpp"

#include "bsp/board.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <stdio.h>

SH1122Driver<256, 64, 18, 19, 16, 17, 20> sh1122{spi0};
auto &screen = sh1122.frame_buffer;

auto text = RichTextRenderer<
    decltype(assets::font_5x10),
    decltype(assets::font_7x10),
    decltype(assets::font_5x10),
    decltype(assets::font_7x10)>{
    .normal_font = assets::font_5x10,
    .bold_font = assets::font_7x10,
    .italic_font = assets::font_5x10,
    .bold_italic_font = assets::font_7x10,
    .letter_spacing = 1,
    .line_height = 13,
};

const char *string_1 =
    "[v\x09]\n"
    "THE [v\x0F]QUICK[/v], [b][v\x04]BROWN[/v][/b] FOX\n"
    "JUMPED OVER THE [u]LAZY[/u] DOG!\n"
    "0123456789\n"
    "~!@#$%^&*()-_=+\n[/v]";

const char *string_2 =
    "[v\x09]\n"
    "The [v\x0F]Quick[/v], [b][v\x04]Brown[/v][/b] Fox\n"
    "Jumped Over The [u]Lazy[/u] Dog...\n"
    "(((1 * x[[y]) + 234) / 456) ^ 789\n"
    "[[]{}\\|;:'\",<.>/?\n[/v]";

int main()
{
    stdio_init_all();
    board_init();

    sh1122.init();

    while (true)
    {
        for (size_t x = 0; x < 256 - 8; ++x)
        {
            screen.clear();

            auto img_contrast = (x * 0xE) / (256 - 8) + 0x1;
            screen.blit(x, 1, assets::heart, -1, -1, -1, -1, img_contrast);
            text.blit_string(screen, 0, 1, string_1);

            sh1122.show();
        }

        for (int16_t x = 256 - 8 - 1; x >= 0; --x)
        {
            screen.clear();

            auto img_contrast = (x * 0xE) / (256 - 8) + 0x1;
            screen.blit(x, 1, assets::star, -1, -1, -1, -1, img_contrast);
            text.blit_string(screen, 0, 1, string_2);

            sh1122.show();
        }
    }
}
