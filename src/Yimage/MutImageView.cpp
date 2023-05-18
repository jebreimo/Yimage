//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/MutImageView.hpp"

#include <string>
#include "Yimage/IMutImage.hpp"
#include "Yimage/YimageException.hpp"
#include "ImageUtilities.hpp"

namespace Yimage
{
    MutImageView::MutImageView() = default;

    MutImageView::MutImageView(IMutImage& img)
        : MutImageView(img.data(), img.pixel_type(),
                       img.width(), img.height(), img.row_gap_size())
    {}

    MutImageView::MutImageView(unsigned char* buffer,
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

    ImageView MutImageView::subimage(size_t x, size_t y) const
    {
        return subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    ImageView
    MutImageView::subimage(size_t x, size_t y,
                           size_t width, size_t height) const
    {
        return make_subimage<ImageView>(*this, x, y, width, height);
    }

    MutImageView MutImageView::mut_subimage(size_t x, size_t y)
    {
        return mut_subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    MutImageView
    MutImageView::mut_subimage(size_t x, size_t y,
                               size_t width, size_t height)
    {
        return make_subimage<MutImageView>(*this, x, y, width, height);
    }

    bool operator==(const MutImageView& a, const MutImageView& b)
    {
        return static_cast<ImageView>(a) == static_cast<ImageView>(b);
    }
}
