//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include "MutableImageView.hpp"

namespace yimage
{
    class Image
    {
    public:
        Image();

        Image(std::unique_ptr<unsigned char> buffer,
              PixelType pixel_type,
              size_t width, size_t height, size_t row_gap_size = 0);

        Image(PixelType pixel_type, size_t width, size_t height,
              size_t row_gap_size = 0);

        Image(const Image& rhs);

        Image(Image&& rhs) noexcept;

        Image& operator=(const Image& rhs);

        Image& operator=(Image&& rhs) noexcept;

        [[nodiscard]]
        operator ImageView() const;

        [[nodiscard]]
        operator MutableImageView();

        [[nodiscard]]
        const unsigned char* data() const;

        [[nodiscard]]
        unsigned char* data();

        [[nodiscard]]
        size_t width() const;

        [[nodiscard]]
        size_t height() const;

        [[nodiscard]]
        size_t row_size() const;

        [[nodiscard]]
        size_t size() const;

        [[nodiscard]]
        PixelType pixel_type() const;

        std::unique_ptr<unsigned char> release();
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        std::unique_ptr<unsigned char> buffer_;
    };
}
