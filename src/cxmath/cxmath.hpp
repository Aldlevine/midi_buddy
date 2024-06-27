#pragma once

#include <concepts>

namespace cx
{
    constexpr auto sqrt_helper(auto x, auto curr, auto prev)
    {
        if (curr == prev)
        {
            return curr;
        }
        return sqrt_helper(x, 0.5 * (curr + x / curr), curr);
    }

    template <std::floating_point f_type_>
    constexpr f_type_ sqrt(f_type_ x)
    {
        return sqrt_helper(x, x, 0);
    }

    template <std::floating_point f_type_>
    constexpr f_type_ sin(f_type_ x)
    {
        double result{0}, pow{x};
        for (auto fac{1LLU}, n{1ULL}; n != 20; fac *= ++n, pow *= x)
            if (n & 1)
                result += (n & 2 ? -pow : pow) / fac;
        return result;
    }

    template <std::floating_point f_type_>
    constexpr f_type_ cos(f_type_ x)
    {
        auto sin = cx::sin(x);
        return cx::sqrt(1.0 - (sin * sin));
    }

    template <std::floating_point f_type_>
    constexpr f_type_ tan(f_type_ x)
    {
        auto sin = cx::sin(x);
        return sin / cx::sqrt(1 - (sin * sin));
    }
} // namespace cx