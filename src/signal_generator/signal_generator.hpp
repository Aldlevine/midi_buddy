#pragma once

using SignalFn = float (*)(float);

template <SignalFn Fn>
class SignalGenerator
{
    uint32_t current_time_millis;
    float current_t_sec;

public:
    float sample(uint32_t new_time_millis, float frequency_hz)
    {
        uint32_t diff_time_millis = new_time_millis - current_time_millis;
        current_time_millis = new_time_millis;
        current_t_sec += (diff_time_millis / 1000.0) * frequency_hz;
        current_t_sec -= (uint64_t)current_t_sec;
        return Fn(current_t_sec);
    }
};