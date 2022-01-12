//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageView.hpp"
#include "Yimage/YimageException.hpp"

#include <algorithm>
#include <string>

namespace yimage
{
    ImageView::ImageView() = default;

    ImageView::ImageView(const unsigned char* buffer,
                         unsigned int width,
                         unsigned int height,
                         PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_size(get_pixel_size(pixel_type)),
          m_pixel_type(pixel_type),
          m_buffer(buffer)
    {}

    bool operator==(const ImageView& a, const ImageView& b)
    {
        return a.width() == b.width()
               && a.height() == b.height()
               && a.pixel_type() == b.pixel_type()
               && std::equal(a.begin(), a.end(), b.begin(), b.end());
    }

    Rgba8 get_rgba8(const ImageView& image, unsigned int x, unsigned int y)
    {
        auto pixel_size = get_pixel_size(image.pixel_type());
        auto ptr = image.pixel_pointer(x, y);
        switch (image.pixel_type())
        {
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
        return {};
    }
}
