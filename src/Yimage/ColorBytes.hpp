//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Yimage/PixelType.hpp"
#include "Yimage/Rgba8.hpp"

namespace Yimage
{
    struct ColorBytes
    {
        unsigned char bytes[8];
        size_t size;
    };

    ColorBytes get_color_bytes(Rgba8 rgba, PixelType pixel_type);
}
