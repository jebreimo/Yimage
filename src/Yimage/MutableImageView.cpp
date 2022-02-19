//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/MutableImageView.hpp"

#include <algorithm>
#include <string>
#include "Yimage/YimageException.hpp"

namespace yimage
{
    MutableImageView::MutableImageView() = default;

    MutableImageView::MutableImageView(unsigned char* buffer,
                                       unsigned width,
                                       unsigned height,
                                       PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_size(get_pixel_size(pixel_type)),
          m_pixel_type(pixel_type),
          m_buffer(buffer)
    {
    }

    MutableImageView::operator ImageView() const
    {
        return {m_buffer, m_width, m_height, m_pixel_type};
    }

    bool operator==(const MutableImageView& a, const MutableImageView& b)
    {
        return static_cast<ImageView>(a) == static_cast<ImageView>(b);
    }

    void paste(ImageView img, int x, int y, MutableImageView mut_img)
    {
        if (img.pixel_type() != mut_img.pixel_type())
            YIMAGE_THROW("Source image has a different pixel type.");
        if (img.pixel_size() < 8)
            YIMAGE_THROW("Pixel sizes less than 8 bits are not supported.");

        unsigned src_x = 0, dst_x = unsigned(x);
        if (x < 0)
        {
            src_x = std::min(unsigned(-x), img.width());
            dst_x = 0;
        }
        else
        {
            dst_x = std::min(dst_x, mut_img.width());
        }
        auto n_copy = std::min(img.width() - src_x, mut_img.width() - dst_x);

        unsigned src_y = 0, dst_y = unsigned(y);
        if (y < 0)
        {
            src_y = std::min(unsigned(-y), img.height());
            dst_y = 0;
        }
        else
        {
            dst_y = std::min(dst_y, mut_img.height());
        }
        auto n_rows = std::min(img.height() - src_y, mut_img.height() - dst_y);


        auto src = img.pixel_pointer(src_x, src_y);
        auto dst = mut_img.pixel_pointer(dst_x, dst_y);

        for (unsigned i = 0; i < n_rows; ++i)
        {
            std::copy(src, src + n_copy * mut_img.pixel_size() / 8, dst);
            src += img.width();
            dst += mut_img.width();
        }
    }

    void set_rgba8(MutableImageView& image, unsigned int x, unsigned int y,
                   Rgba8 rgba)
    {
        auto* ptr = image.pixel_pointer(x, y);
        switch (image.pixel_type())
        {
        case PixelType::MONO_8:
            ptr[0] = std::max(std::max(rgba.r, rgba.g), rgba.b);
            break;
        case PixelType::ALPHA_MONO_8:
            ptr[0] = rgba.a;
            ptr[1] = std::max(std::max(rgba.r, rgba.g), rgba.b);
            break;
        case PixelType::MONO_ALPHA_8:
            ptr[0] = std::max(std::max(rgba.r, rgba.g), rgba.b);
            ptr[1] = rgba.a;
            break;
        case PixelType::RGB_8:
            ptr[0] = rgba.r;
            ptr[1] = rgba.g;
            ptr[2] = rgba.b;
            break;
        case PixelType::ARGB_8:
            ptr[0] = rgba.a;
            ptr[1] = rgba.r;
            ptr[2] = rgba.g;
            ptr[3] = rgba.b;
            break;
        case PixelType::RGBA_8:
            ptr[0] = rgba.r;
            ptr[1] = rgba.g;
            ptr[2] = rgba.b;
            ptr[3] = rgba.a;
            break;
        case PixelType::MONO_1:
        case PixelType::MONO_2:
        case PixelType::MONO_4:
        case PixelType::MONO_16:
        case PixelType::ALPHA_MONO_16:
        case PixelType::MONO_ALPHA_16:
        case PixelType::RGB_16:
        case PixelType::ARGB_16:
        case PixelType::RGBA_16:
        default:
            YIMAGE_THROW("Unsupported pixel type: "
                         + std::to_string(int(image.pixel_type())));
        }
    }
}
