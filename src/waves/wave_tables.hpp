#pragma once

#include "cxmath/cxmath.hpp"
#include "fixed_point/fixed_point.hpp"

#include <array>
#include <numbers>

constexpr size_t default_wave_resolution = 512;

template <size_t resolution = default_wave_resolution>
constexpr std::array<ifixed_t<2, 14>, resolution> sin_table = []()
{
    std::array<ifixed_t<2, 14>, resolution> result{};

    for (size_t i = 0; i < resolution; ++i)
    {
        result[i] = cx::sin((std::numbers::pi * 2 * i) / resolution);
    }
    return result;
}();

template <size_t resolution = default_wave_resolution>
constexpr std::array<ifixed_t<2, 14>, resolution> tri_table = []()
{
    std::array<ifixed_t<2, 14>, resolution> result{};

    size_t quarter_offset = resolution / 4;
    for (size_t i = 0; i < quarter_offset; ++i)
    {
        result[i + quarter_offset * 0] = double(i) / quarter_offset;
        result[i + quarter_offset * 1] = (double(quarter_offset - i) / quarter_offset);
        result[i + quarter_offset * 2] = -result[i + quarter_offset * 0];
        result[i + quarter_offset * 3] = -result[i + quarter_offset * 1];
    }

    return result;
}();

template <typename>
struct wave_table_traits
{
    static constexpr bool valid = false;
};

template <typename type_, size_t resolution_>
struct wave_table_traits<std::array<type_, resolution_>>
{
    using type_t = type_;
    static constexpr size_t resolution = resolution_;
    static constexpr bool valid = true;
};

template <typename wave_table_>
concept is_wave_table = wave_table_traits<std::remove_cvref_t<wave_table_>>::valid;
