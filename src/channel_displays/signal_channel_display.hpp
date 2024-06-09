#pragma once

#include "display/channel_display.hpp"

#include "containers/containers.hpp"
#include "signal_generator/signal_generator.hpp"

#include <cmath>

constexpr size_t SIGNAL_BUFFER_SIZE = CHANNEL_HEIGHT + 1;

class SignalChannelDisplay : public virtual ChannelDisplay
{
    static float sin_generator(float t)
    {
        return std::sin(t * M_TWOPI);
    }

public:
    RingBuffer<uint8_t, SIGNAL_BUFFER_SIZE> signal_buffer;
    SignalGenerator<sin_generator> signal_generator;

    SignalChannelDisplay()
    {
    }

    ~SignalChannelDisplay() override
    {
    }

    void draw_channel(uint8_t channel, DisplayOutput &output) override
    {
        uint8_t cx_start = channel * CHANNEL_WIDTH;
        uint8_t cx_mid = cx_start + CHANNEL_WIDTH / 2;
        uint8_t cx_end = cx_start + CHANNEL_WIDTH;
        uint8_t cy_start = 0;
        uint8_t cy_mid = cy_start + CHANNEL_HEIGHT / 2;
        uint8_t cy_end = CHANNEL_HEIGHT;

        // dummy fill signal buffer
        {
            uint32_t cur_time_millis = board_millis();
            float cur_time_sec = cur_time_millis / 10000.0;
            cur_time_sec -= (uint64_t)cur_time_sec;
            float freq_hz = (std::sin(cur_time_sec * M_TWOPI) + 1) * 4;
            float sample_float = signal_generator.sample(cur_time_millis, freq_hz);
            uint8_t sample = (sample_float + 1) * 127;
            signal_buffer.append(sample);
        }

        // draw signal
        {
            uint8_t x_start;
            uint8_t y_start;
            size_t y = 0;
            for (const auto &sample : signal_buffer)
            {
                if (y >= CHANNEL_HEIGHT)
                {
                    break;
                }

                uint8_t x_end = (cx_start + 4) + ((CHANNEL_WIDTH - 8) * sample) / 255;
                uint8_t y_end = cy_start + y;

                if (y > 1)
                {
                    output.draw_line(x_start, y_start, x_end, y_end);
                }

                x_start = x_end;
                y_start = y_end;
                y += 1;
            }
        }
    }
};
