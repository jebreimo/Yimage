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
#include "Yimage/Image.hpp"
#include "Yimage/MutableImageView.hpp"
#include "Yimage/YimageException.hpp"
#include "ImageUtilities.hpp"

namespace Yimage
{
    ImageView::ImageView() = default;

    ImageView::ImageView(const Image& img)
        : ImageView(img.data(), img.pixel_type(),
                    img.width(), img.height(),
                    img.row_gap_size())
    {}

    ImageView::ImageView(const MutableImageView& view)
        : ImageView(view.data(), view.pixel_type(),
                    view.width(), view.height(),
                    view.row_gap_size())
    {}

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
        return make_subimage<ImageView>(*this, x, y, width, height);
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

    template <size_t BITS>
    constexpr uint8_t get_bits(uint8_t pixel, size_t index)
    {
        constexpr auto pixels = 8 / BITS;
        auto shift = BITS * (pixels - 1 - (index % pixels));
        constexpr auto mask = uint8_t((1 << BITS) - 1);
        constexpr auto delta = 0xFF / mask;
        return uint8_t(((pixel >> shift) & mask) * delta);
    }

    Rgba8 get_rgba8(const ImageView& image, size_t x, size_t y)
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
