//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>

namespace Yimage
{
    enum class PixelType
    {
        NONE,
        MONO_1,
        MONO_2,
        MONO_4,
        MONO_8,
        MONO_16,
        MONO_FLOAT_32,
        ALPHA_MONO_8,
        MONO_ALPHA_8,
        ALPHA_MONO_16,
        MONO_ALPHA_16,
        RGB_8,
        RGB_16,
        ARGB_8,
        RGBA_8,
        ARGB_16,
        RGBA_16
    };

    size_t get_pixel_size(PixelType type);
}
