//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/MutableImageView.hpp"

#include <algorithm>
#include "Yimage/Image.hpp"
#include "Yimage/YimageException.hpp"
#include "ColorBytes.hpp"
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

    void set_rgba8(const MutableImageView& image, size_t x, size_t y, Rgba8 rgba)
    {
        auto bytes = get_color_bytes(rgba, image.pixel_type());
        std::copy(bytes.bytes, bytes.bytes + bytes.size,
                  image.pixel_pointer(x, y));
    }
}
