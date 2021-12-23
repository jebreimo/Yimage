//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdio>
#include <memory>
#include <string>
#include "ImageView.hpp"

namespace yimage
{
    class Image
    {
    public:
        Image();

        Image(std::unique_ptr<unsigned char> buffer,
              unsigned width, unsigned height,
              PixelType pixel_type);

        Image(unsigned width, unsigned height, PixelType pixel_type);

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
        unsigned width() const;

        [[nodiscard]]
        unsigned height() const;

        [[nodiscard]]
        unsigned size() const;

        [[nodiscard]]
        PixelType pixel_type() const;

        std::unique_ptr<unsigned char> release();
    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        PixelType m_pixel_type = PixelType::NONE;
        std::unique_ptr<unsigned char> m_buffer;
    };
}
