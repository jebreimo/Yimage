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

    ImageView
    ImageView::subimage(size_t x, size_t y, size_t width, size_t height) const
    {
        x = std::min(x, width_);
        y = std::min(y, height_);
        width = std::min(width, width_ - x);
        height = std::min(height, height_ - y);
        auto gap_size = gap_size_ + ((width_ - width) * pixel_size_ + 7) / 8;
        auto buffer = pixel_pointer(x, y);
        return {buffer, pixel_type_, width, height, gap_size};
    }

    bool operator==(const ImageView& a, const ImageView& b)
    {
        if (a.width() != b.width()
            || a.height() != b.height()
            || a.pixel_type() != b.pixel_type())
        {
            return false;
        }
        if (a.is_contiguous() && b.is_contiguous())
        {
            return std::equal(a.data(), a.data() + a.size(),
                              b.data(), b.data() + b.size());
        }

        for (size_t i = 0; i < a.height(); ++i)
        {
            auto [ab, ae] = a.row(i);
            auto [bb, be] = a.row(i);
            if (!std::equal(ab, ae, bb, be))
                return false;
        }

        return true;
    }

    Rgba8 get_rgba8(const ImageView& image, unsigned int x, unsigned int y)
    {
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
    }
}
