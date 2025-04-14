//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ColorBytes.hpp"

#include <string>
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    uint8_t get_max(Rgba8 rgba)
    {
        return std::max(std::max(rgba.r, rgba.g), rgba.b);
    }

    ColorBytes get_color_bytes(Rgba8 rgba, PixelType pixel_type)
    {
        ColorBytes result = {};
        switch (pixel_type)
        {
        case PixelType::MONO_8:
            result.bytes[0] = get_max(rgba);
            result.size = 1;
            break;
        case PixelType::ALPHA_MONO_8:
            result.bytes[0] = rgba.a;
            result.bytes[1] = get_max(rgba);
            result.size = 2;
            break;
        case PixelType::MONO_ALPHA_8:
            result.bytes[0] = get_max(rgba);
            result.bytes[1] = rgba.a;
            result.size = 2;
            break;
        case PixelType::RGB_8:
            result.bytes[0] = rgba.r;
            result.bytes[1] = rgba.g;
            result.bytes[2] = rgba.b;
            result.size = 3;
            break;
        case PixelType::ARGB_8:
            result.bytes[0] = rgba.a;
            result.bytes[1] = rgba.r;
            result.bytes[2] = rgba.g;
            result.bytes[3] = rgba.b;
            result.size = 4;
            break;
        case PixelType::RGBA_8:
            result.bytes[0] = rgba.r;
            result.bytes[1] = rgba.g;
            result.bytes[2] = rgba.b;
            result.bytes[3] = rgba.a;
            result.size = 4;
            break;
        case PixelType::MONO_16:
        {
            result.bytes[0] = result.bytes[1] = get_max(rgba);
            result.size = 2;
            break;
        }
        case PixelType::ALPHA_MONO_16:
        {
            result.bytes[0] = result.bytes[1] = get_max(rgba);
            result.bytes[2] = result.bytes[3] = rgba.a;
            result.size = 4;
            break;
        }
        case PixelType::MONO_ALPHA_16:
        {
            result.bytes[0] = result.bytes[1] = rgba.a;
            result.bytes[2] = result.bytes[3] = get_max(rgba);
            result.size = 4;
            break;
        }
        case PixelType::RGB_16:
        {
            result.bytes[0] = result.bytes[1] = rgba.r;
            result.bytes[2] = result.bytes[3] = rgba.g;
            result.bytes[4] = result.bytes[5] = rgba.b;
            result.size = 6;
            break;
        }
        case PixelType::ARGB_16:
        {
            result.bytes[0] = result.bytes[1] = rgba.a;
            result.bytes[2] = result.bytes[3] = rgba.r;
            result.bytes[4] = result.bytes[5] = rgba.g;
            result.bytes[6] = result.bytes[7] = rgba.b;
            result.size = 6;
            break;
        }
        case PixelType::RGBA_16:
        {
            result.bytes[0] = result.bytes[1] = rgba.r;
            result.bytes[2] = result.bytes[3] = rgba.g;
            result.bytes[4] = result.bytes[5] = rgba.b;
            result.bytes[6] = result.bytes[7] = rgba.a;
            result.size = 8;
            break;
        }
        case PixelType::MONO_1:
        case PixelType::MONO_2:
        case PixelType::MONO_4:
        default:
            YIMAGE_THROW("Unsupported pixel type: "
                         + std::to_string(int(pixel_type)));
        }
        return result;
    }
}
