//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
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
        const unsigned char* pixel_pointer(unsigned x, unsigned y) const;

        [[nodiscard]]
        const unsigned char* begin() const;

        [[nodiscard]]
        const unsigned char* end() const;

        [[nodiscard]]
        const unsigned char* data() const;

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
    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        unsigned m_pixel_size = 0;
        PixelType m_pixel_type = PixelType::NONE;
        const unsigned char* m_buffer = nullptr;
    };

    bool operator==(const ImageView& a, const ImageView& b);

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
        const unsigned char* pixel_pointer(unsigned x, unsigned y) const;

        [[nodiscard]]
        unsigned char* pixel_pointer(unsigned x, unsigned y);

        [[nodiscard]]
        const unsigned char* begin() const;

        [[nodiscard]]
        const unsigned char* end() const;

        [[nodiscard]]
        unsigned char* begin();

        [[nodiscard]]
        unsigned char* end();

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

        void paste(ImageView img, int x, int y);
    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        unsigned m_pixel_size = 0;
        PixelType m_pixel_type = PixelType::NONE;
        unsigned char* m_buffer = nullptr;
    };

    bool operator==(const MutableImageView& a, const MutableImageView& b);
}
