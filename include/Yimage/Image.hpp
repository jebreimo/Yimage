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

namespace yimage
{
    enum PixelType
    {
        NONE,
        MONO8,
        RGB24,
        ARGB32,
        RGBA32
    };

    size_t get_pixel_size(PixelType type);

    class Image
    {
    public:
        Image();

        Image(unsigned width, unsigned height, PixelType pixel_type);

        Image(const Image& rhs);

        Image(Image&& rhs) noexcept;

        Image& operator=(const Image& rhs);

        Image& operator=(Image&& rhs) noexcept;

        [[nodiscard]]
        const unsigned char* pixel(unsigned x, unsigned y) const;

        [[nodiscard]]
        unsigned char* pixel(unsigned x, unsigned y);

        [[nodiscard]]
        const unsigned char* begin() const;

        [[nodiscard]]
        const unsigned char* end() const;

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
        unsigned pixel_size() const;

        [[nodiscard]]
        PixelType pixel_type() const;

        std::unique_ptr<unsigned char> release();
    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        unsigned m_pixel_size = 0;
        unsigned m_size = 0;
        PixelType m_pixel_type = PixelType::NONE;
        std::unique_ptr<unsigned char> m_buffer;
    };
}
