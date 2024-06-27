#pragma once

#include "wave_tables.hpp"

#include "fixed_point/fixed_point.hpp"

#include <array>

struct WaveSamplerDebug
{
    double offset;
    size_t resolution;
    double scaled_offset;
    size_t lower_index;
    size_t upper_index;
    double fract;
    double lower_value;
    double upper_value;
    double diff_value;
    double lerp_value;
    double result;
};

template <is_wave_table wave_table_>
struct WaveSampler
{
    using wave_table_t = wave_table_;
    using type_t = wave_table_traits<wave_table_t>::type_t;
    static constexpr size_t resolution = wave_table_traits<wave_table_t>::resolution;

    const wave_table_t &wave_table;

    template <typename offset_>
    constexpr type_t sample(const offset_ &offset) const
    {
        const offset_ scaled_offset = ((offset % 1) * resolution) % resolution;
        const size_t lower_index = scaled_offset;
        const size_t upper_index = (lower_index + 1) % resolution;
        const offset_ fract = scaled_offset - static_cast<offset_>(lower_index);

        const offset_ lower_value = wave_table[lower_index];
        const offset_ upper_value = wave_table[upper_index];
        const offset_ diff_value = upper_value - lower_value;
        const offset_ lerp_value = diff_value * fract;
        const offset_ result = lower_value + lerp_value;

        return result;
    }
};