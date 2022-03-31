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
    {}

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
        auto gap_size = gap_size_ + ((width_ - width) * pixel_size_ + 7) / 8;
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

    void set_rgba8(MutableImageView& image, size_t x, size_t y, Rgba8 rgba)
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
