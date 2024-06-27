#pragma once

#include "hardware/timer.h"

template <uint64_t us_per_sample_, auto callable_>
struct RepeatingTimer
{
    constexpr static uint64_t us_per_sample = us_per_sample_;
    constexpr static auto callable = callable_;

    static uint hw_alarm;

    static void init()
    {

        hw_alarm = hardware_alarm_claim_unused(true);
        hardware_alarm_set_callback(hw_alarm, timer_callback);

        timer_callback(hw_alarm);
    }

    static void timer_callback(uint cb_alarm)
    {
        assert(cb_alarm == hw_alarm);

        auto tick_start = time_us_64();
        hardware_alarm_set_target(hw_alarm, from_us_since_boot(tick_start + us_per_sample));

        callable(tick_start);
    }
};

template <uint64_t us_per_sample_, auto callable_>
uint RepeatingTimer<us_per_sample_, callable_>::hw_alarm{};

struct DeltaTimer
{
    uint64_t min_delta_us;
    uint64_t last_tick{};
    uint64_t delta{};
    uint64_t delta_acc{};

    bool tick()
    {
        uint64_t cur_tick = time_us_64();
        delta_acc += (cur_tick - last_tick);
        last_tick = cur_tick;
        if (delta_acc > min_delta_us)
        {
            delta = delta_acc;
            delta_acc = 0;
            return true;
        }
        return false;
    }
};

struct FixedDeltaTimer
{
    uint64_t step_us;
    uint64_t last_tick{};
    uint64_t delta_acc{};

    void tick()
    {
        uint64_t cur_tick = time_us_64();
        delta_acc += (cur_tick - last_tick);
        last_tick = cur_tick;
    }

    bool step()
    {
        if (delta_acc > step_us)
        {
            delta_acc -= step_us;
            return true;
        }
        return false;
    }
};