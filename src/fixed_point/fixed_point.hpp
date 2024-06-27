#pragma once

#include "pico/stdlib.h"

#include <concepts>
#include <numeric>
#include <cmath>

template <std::integral type_>
struct next_larger
{
};

#define NEXT_LARGER(cur, next) \
    template <>                \
    struct next_larger<cur>    \
    {                          \
        using type = next;     \
    }

NEXT_LARGER(uint8_t, uint16_t);
NEXT_LARGER(uint16_t, uint32_t);
NEXT_LARGER(uint32_t, uint64_t);
NEXT_LARGER(int8_t, int16_t);
NEXT_LARGER(int16_t, int32_t);
NEXT_LARGER(int32_t, int64_t);

#ifdef __SIZEOF_INT128__
NEXT_LARGER(uint64_t, __uint128_t);
NEXT_LARGER(int64_t, __int128_t);
#else
NEXT_LARGER(uint64_t, uint64_t);
NEXT_LARGER(int64_t, int64_t);
#endif

#undef NEXT_LARGER

template <std::integral type_>
using next_larger_t = next_larger<type_>::type;

template <bool is_signed_, size_t int_bits_, size_t fract_bits_>
struct fixed_point_number_data
{
    static constexpr bool valid = false;
};

#define FIXED_POINT_NUMBER_DATA(is_signed, total_bits, data_type)     \
    template <size_t int_bits_, size_t fract_bits_>                   \
        requires(int_bits_ + fract_bits_ == total_bits)               \
    struct fixed_point_number_data<is_signed, int_bits_, fract_bits_> \
    {                                                                 \
        using type = data_type;                                       \
        static constexpr bool valid = true;                           \
    }

FIXED_POINT_NUMBER_DATA(false, 8, uint8_t);
FIXED_POINT_NUMBER_DATA(false, 16, uint16_t);
FIXED_POINT_NUMBER_DATA(false, 32, uint32_t);
FIXED_POINT_NUMBER_DATA(false, 64, uint64_t);
FIXED_POINT_NUMBER_DATA(true, 8, int8_t);
FIXED_POINT_NUMBER_DATA(true, 16, int16_t);
FIXED_POINT_NUMBER_DATA(true, 32, int32_t);
FIXED_POINT_NUMBER_DATA(true, 64, int64_t);

#undef FIXED_POINT_NUMBER_DATA

template <bool is_signed_, size_t int_bits_, size_t fract_bits_>
using fixed_point_number_data_t = fixed_point_number_data<is_signed_, int_bits_, fract_bits_>::type;

template <bool is_signed_, size_t int_bits_, size_t fract_bits_>
    requires fixed_point_number_data<is_signed_, int_bits_, fract_bits_>::valid
struct FixedPointNumber
{
    using this_t = FixedPointNumber<is_signed_, int_bits_, fract_bits_>;
    using type_t = fixed_point_number_data_t<is_signed_, int_bits_, fract_bits_>;

    static constexpr auto is_signed = is_signed_;
    static constexpr auto int_bits = int_bits_;
    static constexpr auto fract_bits = fract_bits_;
    static constexpr size_t scale = (1ULL << (fract_bits - 1));

    type_t value{};

    constexpr FixedPointNumber() = default;
    constexpr FixedPointNumber(const this_t &) = default;
    constexpr FixedPointNumber(this_t &) = default;
    this_t &operator=(const this_t &) = default;
    this_t &operator=(this_t &&) = default;

    template <size_t lhs_fract_bits_, size_t rhs_fract_bits_>
    static constexpr auto shift_value_scale(auto value)
    {
        if constexpr (lhs_fract_bits_ > rhs_fract_bits_)
        {
            if constexpr (std::numeric_limits<decltype(value)>::is_signed)
            {
                const auto num_bits = std::numeric_limits<decltype(value)>::digits;
                const decltype(value) sign = std::signbit(value) ? -1 : 1;
                return sign * ((value * sign) << (lhs_fract_bits_ - rhs_fract_bits_));
            }
            else
            {
                return value << (lhs_fract_bits_ - rhs_fract_bits_);
            }
        }
        else if constexpr (lhs_fract_bits_ < rhs_fract_bits_)
        {
            return value >> (rhs_fract_bits_ - lhs_fract_bits_);
        }
        else
        {
            return value;
        }
    }

    static constexpr struct value_construct_t
    {
    } value_construct{};

    /**
     * constructs from unscaled data directly
     **/
    constexpr FixedPointNumber(const value_construct_t &, const type_t &value)
        : value{value} {}

    /**
     * constructs from floating point
     **/
    template <typename f_type_>
        requires std::floating_point<f_type_>
    constexpr FixedPointNumber(const f_type_ &value)
        : value{static_cast<type_t>(value * scale)}
    {
    }

    /**
     * constructs from integral
     **/
    template <typename f_type_>
        requires std::integral<f_type_>
    constexpr FixedPointNumber(const f_type_ &value)
        : value{static_cast<type_t>(value) * scale}
    {
    }

    /**
     * constructs from a fixed point of another precision
     */
    template <bool rhs_is_signed_, size_t rhs_int_bits_, size_t rhs_fract_bits_>
    constexpr FixedPointNumber(const FixedPointNumber<rhs_is_signed_, rhs_int_bits_, rhs_fract_bits_> &rhs)
        : value{static_cast<type_t>(shift_value_scale<fract_bits, rhs_fract_bits_>(rhs.value))} {}

    /**
     * converts a fixed point to floating point
     */
    template <typename f_type_>
        requires std::floating_point<f_type_> || std::integral<f_type_>
    constexpr operator f_type_() const
    {
        return static_cast<f_type_>(value) / scale;
    }

    constexpr this_t operator+(const this_t &rhs) const
    {
        return this_t{value_construct, static_cast<type_t>(value + rhs.value)};
    }

    constexpr this_t operator-(const this_t &rhs) const
    {
        return this_t{value_construct, static_cast<type_t>(value - rhs.value)};
    }

    constexpr this_t operator-() const
    {
        return this_t{value_construct, static_cast<type_t>(-value)};
    }

    constexpr this_t operator*(const this_t &rhs) const
    {
        return this_t{
            value_construct,
            static_cast<type_t>((static_cast<next_larger_t<type_t>>(value) * static_cast<next_larger_t<type_t>>(rhs.value) >> (fract_bits - 1)))};
    }

    constexpr this_t operator/(const this_t &rhs) const
    {
        return this_t{
            value_construct,
            static_cast<type_t>((static_cast<next_larger_t<type_t>>(value) << fract_bits) / rhs.value)};
    }

    constexpr this_t operator%(const this_t &rhs) const
    {
        return this_t{
            value_construct,
            static_cast<type_t>(
                (
                    (static_cast<next_larger_t<type_t>>(value) << fract_bits) %
                    (static_cast<next_larger_t<type_t>>(rhs.value) << fract_bits)) >>
                fract_bits)};
    }

    constexpr auto operator<=>(const this_t &rhs) const
    {
        return value <=> rhs.value;
    }

    this_t &operator+=(const this_t &rhs)
    {
        value = (*this + rhs).value;
        return *this;
    }

    this_t &operator-=(const this_t &rhs)
    {
        value = (*this - rhs).value;
        return *this;
    }

    this_t &operator*=(const this_t &rhs)
    {
        value = (*this * rhs).value;
        return *this;
    }

    this_t &operator/=(const this_t &rhs)
    {
        value = (*this / rhs).value;
        return *this;
    }

    this_t &operator%=(const this_t &rhs)
    {
        value = (*this % rhs).value;
        return *this;
    }
};

template <typename>
constexpr bool is_fixed_v = false;

template <bool is_signed_, size_t int_bits_, size_t fract_bits_>
constexpr bool is_fixed_v<FixedPointNumber<is_signed_, int_bits_, fract_bits_>> = true;

template <typename fixed_>
concept is_fixed = is_fixed_v<std::remove_cvref_t<fixed_>>;

template <size_t int_bits_, size_t fract_bits_>
using ufixed_t = FixedPointNumber<false, int_bits_, fract_bits_>;

template <size_t int_bits_, size_t fract_bits_>
using ifixed_t = FixedPointNumber<true, int_bits_, fract_bits_>;