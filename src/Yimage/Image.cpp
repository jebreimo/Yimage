//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yimage/Image.hpp>

#include <algorithm>
#include <Yimage/YimageException.hpp>

namespace yimage
{
    Image::Image() = default;

    Image::Image(std::unique_ptr<unsigned char> buffer,
                 unsigned int width, unsigned int height,
                 PixelType pixel_type)
        : m_buffer(move(buffer)),
          m_width(width),
          m_height(height),
          m_pixel_type(pixel_type)
    {

    }

    Image::Image(unsigned int width, unsigned int height, PixelType pixel_type)
        : m_width(width),
          m_height(height),
          m_pixel_type(pixel_type)
    {
        auto size = this->size();
        if (size == 0)
            YIMAGE_THROW("Image size is 0 bytes.");
        m_buffer.reset(new unsigned char[size]);
    }

    Image::Image(const Image& rhs)
        : m_width(rhs.width()),
          m_height(rhs.height()),
          m_pixel_type(rhs.pixel_type())
    {
        auto size = this->size();
        if (size)
        {
            m_buffer.reset(new unsigned char[size]);
            std::copy(rhs.data(), rhs.data() + size, data());
        }
    }

    Image::Image(Image&& rhs) noexcept
        : m_width(rhs.width()),
          m_height(rhs.height()),
          m_pixel_type(rhs.pixel_type()),
          m_buffer(rhs.release())
    {}

    Image& Image::operator=(const Image& rhs)
    {
        if (&rhs == this)
            return *this;

        m_width = rhs.width();
        m_height = rhs.height();
        m_pixel_type = rhs.pixel_type();
        if (auto size = this->size())
        {
            m_buffer.reset(new unsigned char[size]);
            std::copy(rhs.data(), rhs.data() + size, data());
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
        m_pixel_type = rhs.pixel_type();
        m_buffer = rhs.release();
        return *this;
    }

    Image::operator ImageView() const
    {
        return {m_buffer.get(), width(), height(), pixel_type()};
    }

    Image::operator MutableImageView()
    {
        return {m_buffer.get(), width(), height(), pixel_type()};
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

    unsigned Image::row_size() const
    {
        return (m_width * get_pixel_size(m_pixel_type) + 7) / 8;
    }

    unsigned Image::size() const
    {
        return m_height * row_size();
    }

    PixelType Image::pixel_type() const
    {
        return m_pixel_type;
    }

    std::unique_ptr<unsigned char> Image::release()
    {
        m_width = m_height = 0;
        m_pixel_type = PixelType::NONE;
        m_buffer = nullptr;
        return std::move(m_buffer);
    }
}
