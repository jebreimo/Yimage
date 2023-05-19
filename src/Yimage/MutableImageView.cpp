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
#include <vector>
#include "Yimage/Image.hpp"
#include "Yimage/YimageException.hpp"
#include "ImageUtilities.hpp"

namespace Yimage
{
    MutableImageView::MutableImageView() = default;

    MutableImageView::MutableImageView(Image& img)
        : MutableImageView(img.data(), img.pixel_type(),
                           img.width(), img.height(),
                           img.row_gap_size())
    {}

    MutableImageView::MutableImageView(unsigned char* buffer,
                                       PixelType pixel_type,
                                       size_t width,
                                       size_t height,
                                       size_t row_gap_size)
        : width_(width),
          height_(height),
          gap_size_(row_gap_size),
          pixel_size_(get_pixel_size(pixel_type)),
          pixel_type_(pixel_type),
          buffer_(buffer)
    {
        if (pixel_size_ % 8 != 0 && (width_ * pixel_size_) % 8)
            YIMAGE_THROW("The size of a row of pixels must be divisible by 8.");
    }

    MutableImageView MutableImageView::subimage(size_t x, size_t y) const
    {
        return subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    MutableImageView
    MutableImageView::subimage(size_t x, size_t y,
                               size_t width, size_t height) const
    {
        return make_subimage<MutableImageView>(*this, x, y, width, height);
    }

    bool operator==(const MutableImageView& a, const MutableImageView& b)
    {
        return static_cast<ImageView>(a) == static_cast<ImageView>(b);
    }

    void paste(ImageView src, MutableImageView dst, ptrdiff_t x, ptrdiff_t y)
    {
        if (src.pixel_type() != dst.pixel_type())
            YIMAGE_THROW("Source and destination images can't have different pixel types.");
        if (src.pixel_size() < 8)
            YIMAGE_THROW("Pixel sizes less than 8 bits are not supported.");

        if (x < 0)
            src = src.subimage(size_t(-x), 0);
        else
            dst = dst.subimage(size_t(x), 0);

        if (y < 0)
            src = src.subimage(0, size_t(-y));
        else
            dst = dst.subimage(0, size_t(y));

        auto width = std::min(src.width(), dst.width());
        auto height = std::min(src.height(), dst.height());
        src = src.subimage(0, 0, width, height);
        dst = dst.subimage(0, 0, width, height);

        if (src.is_contiguous() && dst.is_contiguous())
        {
            std::copy(src.data(), src.data() + src.size(), dst.data());
            return;
        }

        for (size_t i = 0; i < src.height(); ++i)
        {
            auto [i_b, i_e] = src.row(i);
            auto [m_b, m_e] = dst.row(i);
            std::copy(i_b, i_e, m_b);
        }
    }

    struct ColorBytes
    {
        unsigned char bytes[8];
        size_t size;
    };

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

    void set_rgba8(const MutableImageView& image, size_t x, size_t y, Rgba8 rgba)
    {
        auto bytes = get_color_bytes(rgba, image.pixel_type());
        std::copy(bytes.bytes, bytes.bytes + bytes.size,
                  image.pixel_pointer(x, y));
    }

    void fill_rgba8(const MutableImageView& image, Rgba8 rgba)
    {
        fill_rgba8(image, &rgba, 1);
    }

    void fill_rgba8(const MutableImageView& image, const Rgba8* rgba, size_t num_rgba)
    {
        if (!image)
            return;

        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < num_rgba; ++i)
        {
            auto cb = get_color_bytes(rgba[i], image.pixel_type());
            bytes.insert(bytes.end(), cb.bytes, cb.bytes + cb.size);
        }

        auto src_it = bytes.begin();
        for (size_t y = 0; y < image.height(); ++y)
        {
            auto [beg, end] = image.row(y);
            while (std::distance(src_it, bytes.end())
                   <= std::distance(beg, end))
            {
                beg = std::copy(src_it, bytes.end(), beg);
                src_it = bytes.begin();
            }
            if (beg != end)
            {
                std::copy(src_it, src_it + (end - beg), beg);
                std::advance(src_it, end - beg);
            }
        }
    }
}
