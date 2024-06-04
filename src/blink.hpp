#pragma once

#include "pico/stdlib.h"
#include "bsp/board.h"

template <uint8_t LED_PIN, uint8_t BLINK_MS>
class Blinker
{
    bool needs_blink = false;
    bool running = false;
    uint32_t start_ms = 0;

public:
    void blink()
    {
        needs_blink = true;
    }

    void init()
    {
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
    }

    void task()
    {
        if (needs_blink && !running)
        {
            start_ms = board_millis();
            running = true;
            needs_blink = false;
            gpio_put(LED_PIN, true);
        }

        if (running)
        {
            const auto cur_ms = board_millis();
            if (cur_ms - start_ms > BLINK_MS)
            {
                running = false;
                gpio_put(LED_PIN, false);
            }
        }
    }
};
