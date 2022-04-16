//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/MutableImageView.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "Yimage/YimageException.hpp"

namespace yimage
{
    MutableImageView::MutableImageView() = default;

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

    MutableImageView::operator ImageView() const
    {
        return {buffer_, pixel_type_, width_, height_, gap_size_};
    }

    MutableImageView
    MutableImageView::subimage(size_t x, size_t y,
                               size_t width, size_t height) const
    {
        x = std::min(x, width_);
        y = std::min(y, height_);
        width = std::min(width, width_ - x);
        height = std::min(height, height_ - y);
        auto gap_size = gap_size_ + ((width_ - width) * pixel_size_) / 8;
        auto buffer = buffer_ + y * row_size() + x * pixel_size_ / 8;
        return {buffer, pixel_type_, width, height, gap_size};
    }

    bool operator==(const MutableImageView& a, const MutableImageView& b)
    {
        return static_cast<ImageView>(a) == static_cast<ImageView>(b);
    }

    void paste(ImageView img, ptrdiff_t x, ptrdiff_t y, MutableImageView mut_img)
    {
        if (img.pixel_type() != mut_img.pixel_type())
            YIMAGE_THROW("Source image has a different pixel type.");
        if (img.pixel_size() < 8)
            YIMAGE_THROW("Pixel sizes less than 8 bits are not supported.");

        if (x < 0)
            img = img.subimage(size_t(-x), 0);
        else
            mut_img = mut_img.subimage(size_t(x), 0);

        if (y < 0)
            img = img.subimage(0, size_t(-y));
        else
            mut_img = mut_img.subimage(0, size_t(y));

        auto width = std::min(img.width(), mut_img.width());
        auto height = std::min(img.height(), mut_img.height());
        img = img.subimage(0, 0, width, height);
        mut_img = mut_img.subimage(0, 0, width, height);

        if (img.is_contiguous() && mut_img.is_contiguous())
        {
            std::copy(img.data(), img.data() + img.size(), mut_img.data());
            return;
        }

        for (size_t i = 0; i < img.height(); ++i)
        {
            auto [i_b, i_e] = img.row(i);
            auto [m_b, m_e] = mut_img.row(i);
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
        ColorBytes result;
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

    void set_rgba8(MutableImageView& image, size_t x, size_t y, Rgba8 rgba)
    {
        auto bytes = get_color_bytes(rgba, image.pixel_type());
        std::copy(bytes.bytes, bytes.bytes + bytes.size,
                  image.pixel_pointer(x, y));
    }


    void fill_rgba8(MutableImageView& image, Rgba8 rgba)
    {
        fill_rgba8(image, &rgba, 1);
    }

    void fill_rgba8(MutableImageView image, const Rgba8* rgba, size_t num_rgba)
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
            while (std::distance(src_it, bytes.end()) <= std::distance(beg, end))
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
