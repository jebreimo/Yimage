//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yimage/Image.hpp>
#include <Yimage/YimageException.hpp>
#include <algorithm>

namespace yimage
{
    size_t get_pixel_size(PixelType type)
    {
        switch (type)
        {
        case MONO8:
            return 1;
        case RGB24:
            return 3;
        case ARGB32:
        case RGBA32:
            return 4;
        default:
            return 0;
        }
    }

    Image::Image() = default;

    Image::Image(unsigned int width, unsigned int height, PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_size(get_pixel_size(pixel_type)),
          m_size(m_width * m_height * m_pixel_size),
          m_pixel_type(pixel_type)
    {
        if (m_size == 0)
            YIMAGE_THROW("Image size is 0 bytes.");
        m_buffer.reset(new unsigned char[m_size]);
    }

    Image::Image(const Image& rhs)
        : m_width(rhs.width()),
          m_height(rhs.height()),
          m_pixel_size(rhs.pixel_size()),
          m_size(rhs.size()),
          m_pixel_type(rhs.pixel_type())
    {
        if (m_size)
        {
            m_buffer.reset(new unsigned char[m_size]);
            std::copy(rhs.begin(), rhs.end(), data());
        }
    }

    Image::Image(Image&& rhs) noexcept
        : m_width(rhs.width()),
          m_height(rhs.height()),
          m_pixel_type(rhs.pixel_type()),
          m_size(rhs.size()),
          m_buffer(rhs.release())
    {}

    Image& Image::operator=(const Image& rhs)
    {
        m_width = rhs.width();
        m_height = rhs.height();
        m_pixel_size = rhs.pixel_size();
        m_size = rhs.size();
        m_pixel_type = rhs.pixel_type();
        if (auto size = m_width * m_height * m_pixel_size)
        {
            m_buffer.reset(new unsigned char[size]);
            std::copy(rhs.begin(), rhs.end(), data());
        }
        else
        {
            m_buffer.reset();
        }
        return *this;
    }

    Image& Image::operator=(Image&& rhs) noexcept
    {
        m_width = rhs.width();
        m_height = rhs.height();
        m_pixel_size = rhs.pixel_size();
        m_size = rhs.size();
        m_pixel_type = rhs.pixel_type();
        m_buffer = rhs.release();
        return *this;
    }

    const unsigned char* Image::pixel(unsigned x, unsigned y) const
    {
        auto index = (x + y * m_width) * m_pixel_size;
        if (index >= m_size)
            YIMAGE_THROW("Indexes are out of range.");
        return m_buffer.get() + index;
    }

    unsigned char* Image::pixel(unsigned x, unsigned y)
    {
        auto index = (x + y * m_width) * m_pixel_size;
        if (index >= m_size)
            YIMAGE_THROW("Indexes are out of range.");
        return m_buffer.get() + index;
    }

    const unsigned char* Image::begin() const
    {
        return m_buffer.get();
    }

    const unsigned char* Image::end() const
    {
        return m_buffer.get() + m_size;
    }

    const unsigned char* Image::data() const
    {
        return m_buffer.get();
    }

    unsigned char* Image::data()
    {
        return m_buffer.get();
    }

    unsigned Image::width() const
    {
        return m_width;
    }

    unsigned Image::height() const
    {
        return m_height;
    }

    unsigned Image::size() const
    {
        return m_size;
    }

    unsigned Image::pixel_size() const
    {
        return m_pixel_size;
    }

    PixelType Image::pixel_type() const
    {
        return m_pixel_type;
    }

    std::unique_ptr<unsigned char> Image::release()
    {
        m_width = m_height = m_pixel_size = m_size = 0;
        m_pixel_type = PixelType::NONE;
        return std::move(m_buffer);
    }
}
