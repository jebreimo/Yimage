//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/IImage.hpp"

#include <string>
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    template <size_t BITS>
    constexpr uint8_t get_bits(uint8_t pixel, size_t index)
    {
        constexpr auto pixels = 8 / BITS;
        auto shift = BITS * (pixels - 1 - (index % pixels));
        constexpr auto mask = uint8_t((1 << BITS) - 1);
        constexpr auto delta = 0xFF / mask;
        return uint8_t(((pixel >> shift) & mask) * delta);
    }

    Rgba8 get_rgba8(const IImage& image, size_t x, size_t y)
    {
        auto ptr = image.pixel_pointer(x, y);
        switch (image.pixel_type())
        {
        case PixelType::MONO_1:
        {
            auto v = get_bits<1>(*ptr, x);
            return {v, v, v, 0xFF};
        }
        case PixelType::MONO_2:
        {
            auto v = get_bits<2>(*ptr, x);
            return {v, v, v, 0xFF};
        }
        case PixelType::MONO_4:
        {
            auto v = get_bits<4>(*ptr, x);
            return {v, v, v, 0xFF};
        }
        case PixelType::MONO_8:
            return {ptr[0], ptr[0], ptr[0], 0xFF};
        case PixelType::ALPHA_MONO_8:
            return {ptr[1], ptr[1], ptr[1], ptr[0]};
        case PixelType::MONO_ALPHA_8:
            return {ptr[0], ptr[0], ptr[0], ptr[1]};
        case PixelType::RGB_8:
            return {ptr[0], ptr[1], ptr[2], 0xFF};
        case PixelType::ARGB_8:
            return {ptr[1], ptr[2], ptr[3], ptr[0]};
        case PixelType::RGBA_8:
            return {ptr[0], ptr[1], ptr[2], ptr[3]};
        case PixelType::MONO_16:
        case PixelType::ALPHA_MONO_16:
        case PixelType::MONO_ALPHA_16:
        case PixelType::RGB_16:
        case PixelType::ARGB_16:
        case PixelType::RGBA_16:
        default:
            break;
        }
        YIMAGE_THROW("Unsupported pixel type: "
                     + std::to_string(int(image.pixel_type())));
    }
}
