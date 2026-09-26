//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the Zero-Clause BSD License.
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
                    img.row_gap_size(),
                    img.metadata(),
                    img.palette())
    {
    }

    ImageView::ImageView(const MutableImageView& view)
        : ImageView(view.data(), view.pixel_type(),
                    view.width(), view.height(),
                    view.row_gap_size(),
                    view.metadata(),
                    view.palette()
                        ? std::span<const Rgba8>(*view.palette())
                        : std::span<const Rgba8>())
    {
    }

    ImageView::ImageView(const unsigned char* buffer,
                         PixelType pixel_type,
                         size_t width,
                         size_t height,
                         size_t row_gap_size,
                         const ImageMetadata* metadata,
                         std::span<const Rgba8> palette)
        : width_(width),
          height_(height),
          gap_size_(row_gap_size),
          pixel_size_(get_pixel_size(pixel_type)),
          pixel_type_(pixel_type),
          buffer_(buffer),
          metadata_(metadata),
          palette_(palette)
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

    namespace
    {
        template <size_t BITS>
        constexpr uint8_t get_bits(uint8_t pixel, size_t index)
        {
            constexpr auto pixels = 8 / BITS;
            const auto shift = BITS * (pixels - 1 - (index % pixels));
            constexpr auto mask = uint8_t((1 << BITS) - 1);
            return uint8_t((pixel >> shift) & mask);
        }

        /**
         * @brief Scales a pixel value to the range [0, 255].
         *
         * @tparam BITS The number of bits per pixel.
         * @param pixel The pixel value.
         * @param index The index of the pixel in the row.
         * @return The scaled pixel value.
         */
        template <size_t BITS>
        constexpr uint8_t get_scaled_bits(uint8_t pixel, size_t index)
        {
            auto bits = get_bits<BITS>(pixel, index);
            return static_cast<uint8_t>(bits * 255 / ((1 << BITS) - 1));
        }

        Rgba8 get_palette_entry(const ImageView& image, size_t index)
        {
            if (index >= image.palette().size())
            {
                YIMAGE_THROW("Palette index " + std::to_string(index)
                    + " is out of range.");
            }
            return image.palette()[index];
        }
    }

    Rgba8 get_rgba8(const ImageView& image, size_t x, size_t y)
    {
        auto ptr = image.pixel_pointer(x, y);
        switch (image.pixel_type())
        {
        case PixelType::MONO_1:
        {
            const auto v = get_scaled_bits<1>(*ptr, x);
            return {v, v, v, 0xFF};
        }
        case PixelType::MONO_2:
        {
            const auto v = get_scaled_bits<2>(*ptr, x);
            return {v, v, v, 0xFF};
        }
        case PixelType::MONO_4:
        {
            const auto v = get_scaled_bits<4>(*ptr, x);
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
        case PixelType::INDEX_1:
            return get_palette_entry(image, get_bits<1>(*ptr, x));
        case PixelType::INDEX_2:
            return get_palette_entry(image, get_bits<2>(*ptr, x));
        case PixelType::INDEX_4:
            return get_palette_entry(image, get_bits<4>(*ptr, x));
        case PixelType::INDEX_8:
            return get_palette_entry(image, ptr[0]);
        default:
            break;
        }
        YIMAGE_THROW("Unsupported pixel type: "
            + std::to_string(int(image.pixel_type())));
    }
}
