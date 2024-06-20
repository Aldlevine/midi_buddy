#pragma once

#include "pico/stdlib.h"

enum class ImageFormat
{
    GS4_HSB,
};

struct ImagePxRatio
{
    size_t numerator;
    size_t denominator;
};

template <ImageFormat fmt_>
struct ImageFormatTraits;
