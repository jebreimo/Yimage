//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ImageView.hpp"

namespace yimage
{
    class MutableImageView
    {
    public:
        MutableImageView();

        MutableImageView(unsigned char* buffer,
                         unsigned width, unsigned height,
                         PixelType pixel_type);

        [[nodiscard]]
        operator ImageView() const;

        [[nodiscard]]
        constexpr unsigned char* pixel_pointer(unsigned x, unsigned y)
        {
            return m_buffer + y * row_size() + x * m_pixel_size / 8;
        }

        [[nodiscard]]
        constexpr unsigned char* begin()
        {
            return m_buffer;
        }

        [[nodiscard]]
        constexpr unsigned char* end()
        {
            return m_buffer + size();
        }

        [[nodiscard]]
        constexpr unsigned char* data()
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
        unsigned char* m_buffer = nullptr;
    };

    bool operator==(const MutableImageView& a, const MutableImageView& b);

    void paste(ImageView img, int x, int y, MutableImageView mut_img);

    void set_rgba8(MutableImageView& image, unsigned x, unsigned y, Rgba8 rgba);
}
