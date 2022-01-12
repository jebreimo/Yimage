//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include "PixelType.hpp"

namespace yimage
{
    class ImageView
    {
    public:
        ImageView();

        ImageView(const unsigned char* buffer,
                  unsigned width,
                  unsigned height,
                  PixelType pixel_type);

        [[nodiscard]]
        constexpr const unsigned char* pixel_pointer(unsigned x, unsigned y) const
        {
            return m_buffer + y * row_size() + x * m_pixel_size / 8;
        }


        [[nodiscard]]
        constexpr const unsigned char* begin() const
        {
            return m_buffer;
        }

        [[nodiscard]]
        constexpr const unsigned char* end() const
        {
            return m_buffer + size();
        }

        [[nodiscard]]
        constexpr const unsigned char* data() const
        {
            return m_buffer;
        }

        [[nodiscard]]
        constexpr unsigned width() const
        {
            return m_width;
        }

        [[nodiscard]]
        constexpr unsigned height() const
        {
            return m_height;
        }

        [[nodiscard]]
        constexpr unsigned row_size() const
        {
            return (m_width * m_pixel_size + 7) / 8;
        }

        [[nodiscard]]
        constexpr unsigned size() const
        {
            return m_height * row_size();
        }

        [[nodiscard]]
        constexpr unsigned pixel_size() const
        {
            return m_pixel_size;
        }

        [[nodiscard]]
        constexpr PixelType pixel_type() const
        {
            return m_pixel_type;
        }

    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        unsigned m_pixel_size = 0;
        PixelType m_pixel_type = PixelType::NONE;
        const unsigned char* m_buffer = nullptr;
    };

    bool operator==(const ImageView& a, const ImageView& b);

    struct Rgba8
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    Rgba8 get_rgba8(const ImageView& image, unsigned x, unsigned y);
}
