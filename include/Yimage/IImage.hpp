//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>
#include "PixelType.hpp"
#include "Rgba8.hpp"

namespace Yimage
{
    class ImageView;

    class IImage
    {
    public:
        [[nodiscard]]
        virtual explicit operator bool() const = 0;

        [[nodiscard]]
        virtual const unsigned char*
        pixel_pointer(size_t x, size_t y) const = 0;

        [[nodiscard]]
        virtual std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const = 0;

        [[nodiscard]]
        virtual const unsigned char* data() const = 0;

        [[nodiscard]]
        virtual size_t width() const = 0;

        [[nodiscard]]
        virtual size_t height() const = 0;

        [[nodiscard]]
        virtual size_t row_size() const = 0;

        [[nodiscard]]
        virtual size_t size() const = 0;

        [[nodiscard]]
        virtual size_t pixel_size() const = 0;

        [[nodiscard]]
        virtual PixelType pixel_type() const = 0;

        [[nodiscard]]
        virtual bool is_contiguous() const = 0;

        [[nodiscard]]
        virtual size_t row_gap_size() const = 0;

        [[nodiscard]]
        virtual ImageView subimage(size_t x, size_t y) const = 0;

        [[nodiscard]]
        virtual ImageView subimage(size_t x, size_t y,
                                   size_t width, size_t height) const = 0;
    };

    Rgba8 get_rgba8(const IImage& image, size_t x, size_t y);
}
