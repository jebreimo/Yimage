//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PixelType.hpp"

namespace Yimage
{
    size_t get_pixel_size(PixelType type)
    {
        switch (type)
        {
        case PixelType::MONO_1:
            return 1;
        case PixelType::MONO_2:
            return 2;
        case PixelType::MONO_4:
            return 4;
        case PixelType::MONO_8:
            return 8;
        case PixelType::MONO_16:
        case PixelType::ALPHA_MONO_8:
        case PixelType::MONO_ALPHA_8:
            return 16;
        case PixelType::RGB_8:
            return 24;
        case PixelType::MONO_FLOAT_32:
        case PixelType::ALPHA_MONO_16:
        case PixelType::MONO_ALPHA_16:
        case PixelType::ARGB_8:
        case PixelType::RGBA_8:
            return 32;
        case PixelType::RGB_16:
            return 48;
        case PixelType::ARGB_16:
        case PixelType::RGBA_16:
            return 64;
        default:
            return 0;
        }
    }
}
