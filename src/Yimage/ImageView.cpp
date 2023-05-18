//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageView.hpp"

#include <algorithm>
#include <string>
#include "Yimage/YimageException.hpp"
#include "ImageUtilities.hpp"

namespace Yimage
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
    {
        if (pixel_size_ % 8 != 0 && (width_ * pixel_size_) % 8)
            YIMAGE_THROW("The size of a row of pixels must be divisible by 8.");
    }

    ImageView ImageView::subimage(size_t x, size_t y) const
    {
        return subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    ImageView
    ImageView::subimage(size_t x, size_t y, size_t width, size_t height) const
    {
        return make_subimage(*this, x, y, width, height);
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
}
