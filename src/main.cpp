#include "fixed_point/fixed_point.hpp"
#include "fonts/fonts.hpp"
#include "images/images.hpp"
#include "waves/waves.hpp"

#include "drivers/sh1122/sh1122.hpp"
#include "graphics/graphics.hpp"
#include "time/time.hpp"

#include "bsp/board.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include <inttypes.h>
#include <stdio.h>

using sh1122_t = SH1122Driver<256, 64, 18, 19, 16, 17, 20>;
sh1122_t sh1122{spi0};
using screen_t = sh1122_t::frame_buffer_t;

using utime_t = ufixed_t<40, 24>;

auto text_renderer = RichTextRenderer<
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
auto shape_renderer = ShapeRenderer{};

int64_t buffered_us{};
int64_t buffered_samples{};
uint64_t total_us{};
uint64_t total_samples{};
ifixed_t<2, 14> cur_sample{};
uint64_t missed_samples = 0;

std::array<ifixed_t<2, 14>, 8> cur_samples{};

constexpr WaveSampler wave_sampler{sin_table<>};
constexpr WaveSampler wave_sampler2{tri_table<>};

constexpr const char display_fmt_str[] =
    "BUFFERED_SAMPLES: %" PRIi64 "sp\n"
    "CURRENT: %f\n"
    "[v\x09]"
    "The [v\x0F]Quick[/v], [b][v\x04]Brown[/v][/b] Fox\n"
    "Jumped Over The [u]Lazy[/u] Dog...[/v]";

/**
 * display thread main
 */
void display_main()
{
    sh1122.init();

    DeltaTimer display_timer{.min_delta_us = 1000 * 1000 / 60};

    const auto &image = assets::star;
    int64_t image_x = 0;
    size_t max_image_x = screen_t::width - image.width;
    int8_t image_dir = 1;
    uint8_t image_speed_px_s = 64;

    while (true)
    {
        if (display_timer.tick())
        {
            auto &screen = sh1122.get_back_buffer();
            screen.clear();

            char output_str[sizeof(display_fmt_str) + 20];
            snprintf(
                output_str, sizeof(output_str), display_fmt_str,
                buffered_samples,
                double(cur_samples[4]));
            text_renderer.draw_string(screen, 0, 1, output_str);

            image_x += int64_t(image_speed_px_s) * image_dir * int64_t(display_timer.delta) / (1000 * 1000);

            if (image_dir > 0 && image_x > max_image_x)
            {
                image_dir *= -1;
                image_x = max_image_x;
            }

            else if (image_dir < 0 && image_x < 0)
            {
                image_dir *= -1;
                image_x = 0;
            }

            uint8_t value = ((image_x * 0xE) / max_image_x) + 1;

            screen.blit(image_x, text_renderer.line_height * 4 + 1, image, -1, -1, -1, -1, value);

            sh1122.swap_buffers();
        }
    }
}

/**
 * realtime dac timer
 */
using dac_timer = RepeatingTimer<
    22,
    [](uint64_t tick_start)
    {
        buffered_us -= 22;
        buffered_samples -= 1;
    }>;

constexpr utime_t sec_per_sample = static_cast<double>(dac_timer::us_per_sample) / (1000 * 1000);

/**
 * realtime thread main
 */
void realtime_main()
{
    /** TODO: if we don't have this the timer doesn't always stay running. Figure out why and fix this. */
    sleep_ms(100);
    dac_timer::init();

    utime_t sample_idx{0};
    utime_t freq_hz{0.05};

    while (true)
    {

        while (buffered_samples < 2)
        {
            buffered_us += dac_timer::us_per_sample;
            buffered_samples += 1;

            sample_idx += sec_per_sample * freq_hz;
            for (size_t i = 0; i < cur_samples.size(); ++i)
            {
                auto cur_sample = wave_sampler.sample(sample_idx);
                cur_samples[i] = cur_sample;
            }
        }
    }
}

int main()
{
    stdio_init_all();
    board_init();

    multicore_launch_core1(display_main);
    realtime_main();

    return 0;
}
