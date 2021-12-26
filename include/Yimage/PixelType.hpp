//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>

namespace yimage
{
    enum class PixelType
    {
        NONE,
        MONO8,
        ALPHA_MONO16,
        MONO_ALPHA16,
        RGB24,
        ARGB32,
        RGBA32
    };

    size_t get_pixel_size(PixelType type);
}
