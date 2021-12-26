//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PixelType.hpp"

namespace yimage
{
    size_t get_pixel_size(PixelType type)
    {
        switch (type)
        {
        case PixelType::MONO8:
            return 1;
        case PixelType::ALPHA_MONO16:
        case PixelType::MONO_ALPHA16:
            return 2;
        case PixelType::RGB24:
            return 3;
        case PixelType::ARGB32:
        case PixelType::RGBA32:
            return 4;
        default:
            return 0;
        }
    }
}
