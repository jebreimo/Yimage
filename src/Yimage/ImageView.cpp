//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageView.hpp"

#include <algorithm>
#include "Yimage/YimageException.hpp"

namespace yimage
{
    ImageView::ImageView() = default;

    ImageView::ImageView(const unsigned char* buffer,
                         unsigned int width,
                         unsigned int height,
                         PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_size(get_pixel_size(pixel_type)),
          m_pixel_type(pixel_type),
          m_buffer(buffer)
    {}

    const unsigned char*
    ImageView::pixel_pointer(unsigned int x, unsigned int y) const
    {
        return m_buffer + (y * m_width + x) * m_pixel_size;
    }

    const unsigned char* ImageView::begin() const
    {
        return m_buffer;
    }

    const unsigned char* ImageView::end() const
    {
        return m_buffer + size();
    }

    const unsigned char* ImageView::data() const
    {
        return m_buffer;
    }

    unsigned ImageView::width() const
    {
        return m_width;
    }

    unsigned ImageView::height() const
    {
        return m_height;
    }

    unsigned ImageView::size() const
    {
        return m_width * m_height * m_pixel_size;
    }

    unsigned ImageView::pixel_size() const
    {
        return m_pixel_size;
    }

    PixelType ImageView::pixel_type() const
    {
        return m_pixel_type;
    }

    bool operator==(const ImageView& a, const ImageView& b)
    {
        return a.width() == b.width()
               && a.height() == b.height()
               && a.pixel_type() == b.pixel_type()
               && std::equal(a.begin(), a.end(), b.begin(), b.end());
    }

    MutableImageView::MutableImageView() = default;

    MutableImageView::MutableImageView(unsigned char* buffer,
                                       unsigned width,
                                       unsigned height,
                                       PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_size(get_pixel_size(pixel_type)),
          m_pixel_type(pixel_type),
          m_buffer(buffer)
    {}

    MutableImageView::operator ImageView() const
    {
        return {m_buffer, m_width, m_height, m_pixel_type};
    }

    const unsigned char*
    MutableImageView::pixel_pointer(unsigned x, unsigned y) const
    {
        return m_buffer + (y * m_width + x) * m_pixel_size;
    }

    unsigned char* MutableImageView::pixel_pointer(unsigned x, unsigned y)
    {
        return m_buffer + (y * m_width + x) * m_pixel_size;
    }

    const unsigned char* MutableImageView::begin() const
    {
        return m_buffer;
    }

    const unsigned char* MutableImageView::end() const
    {
        return m_buffer + size();
    }

    unsigned char* MutableImageView::begin()
    {
        return m_buffer;
    }

    unsigned char* MutableImageView::end()
    {
        return m_buffer + size();
    }

    const unsigned char* MutableImageView::data() const
    {
        return m_buffer;
    }

    unsigned char* MutableImageView::data()
    {
        return m_buffer;
    }

    unsigned MutableImageView::width() const
    {
        return m_width;
    }

    unsigned MutableImageView::height() const
    {
        return m_height;
    }

    unsigned MutableImageView::size() const
    {
        return m_width * m_height * m_pixel_size;
    }

    unsigned MutableImageView::pixel_size() const
    {
        return m_pixel_size;
    }

    PixelType MutableImageView::pixel_type() const
    {
        return m_pixel_type;
    }

    void MutableImageView::paste(ImageView img, int x, int y)
    {
        if (img.pixel_type() != pixel_type())
            YIMAGE_THROW("Source image has a different pixel type.");

        unsigned src_x = 0, dst_x = unsigned(x);
        if (x < 0)
        {
            src_x = std::min(unsigned(-x), img.width());
            dst_x = 0;
        }
        else
        {
            dst_x = std::min(dst_x, width());
        }
        auto n_copy = std::min(img.width() - src_x, width() - dst_x);

        unsigned src_y = 0,  dst_y = unsigned(y);
        if (y < 0)
        {
            src_y = std::min(unsigned(-y), img.height());
            dst_y = 0;
        }
        else
        {
            dst_y = std::min(dst_y, height());
        }
        auto n_rows = std::min(img.height() - src_y, height() - dst_y);

        auto src = img.pixel_pointer(src_x, src_y);
        auto dst = pixel_pointer(dst_x, dst_y);

        for (unsigned i = 0; i < n_rows; ++i)
        {
            std::copy(src, src + n_copy * pixel_size(), dst);
            src += img.width();
            dst += width();
        }
    }

    bool operator==(const MutableImageView& a, const MutableImageView& b)
    {
        return static_cast<ImageView>(a) == static_cast<ImageView>(b);
    }
}
