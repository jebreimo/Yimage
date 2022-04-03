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
                 PixelType pixel_type,
                 size_t width, size_t height,
                 size_t row_gap_size)
        : width_(width),
          height_(height),
          gap_size_(row_gap_size),
          pixel_type_(pixel_type),
          buffer_(move(buffer))
    {
        auto pixel_size = get_pixel_size(pixel_type);
        if (pixel_size % 8 != 0 && (width_ * pixel_size) % 8)
            YIMAGE_THROW("The size of a row of pixels must be divisible by 8.");
    }

    Image::Image(PixelType pixel_type,
                 size_t width, size_t height,
                 size_t row_gap_size)
        : width_(width),
          height_(height),
          gap_size_(row_gap_size),
          pixel_type_(pixel_type)
    {
        auto size = this->size();
        if (size == 0)
            YIMAGE_THROW("Image size is 0 bytes.");
        auto pixel_size = get_pixel_size(pixel_type);
        if (pixel_size % 8 != 0 && (width_ * pixel_size) % 8)
            YIMAGE_THROW("The size of a row of pixels must be divisible by 8.");
        buffer_.reset(new unsigned char[size]);
    }

    Image::Image(const Image& rhs)
        : width_(rhs.width()),
          height_(rhs.height()),
          gap_size_(rhs.gap_size_),
          pixel_type_(rhs.pixel_type())
    {
        auto size = this->size();
        if (size)
        {
            buffer_.reset(new unsigned char[size]);
            std::copy(rhs.data(), rhs.data() + size, data());
        }
    }

    Image::Image(Image&& rhs) noexcept
        : width_(rhs.width()),
          height_(rhs.height()),
          pixel_type_(rhs.pixel_type()),
          buffer_(rhs.release())
    {}

    Image& Image::operator=(const Image& rhs)
    {
        if (&rhs == this)
            return *this;

        width_ = rhs.width();
        height_ = rhs.height();
        gap_size_ = rhs.gap_size_;
        pixel_type_ = rhs.pixel_type();
        if (auto size = this->size())
        {
            buffer_.reset(new unsigned char[size]);
            std::copy(rhs.data(), rhs.data() + size, data());
        }
        else
        {
            buffer_.reset();
        }
        return *this;
    }

    Image& Image::operator=(Image&& rhs) noexcept
    {
        width_ = rhs.width();
        height_ = rhs.height();
        gap_size_ = rhs.gap_size_;
        pixel_type_ = rhs.pixel_type();
        buffer_ = rhs.release();
        return *this;
    }

    Image::operator ImageView() const
    {
        return {buffer_.get(), pixel_type_, width_, height_, gap_size_};
    }

    Image::operator MutableImageView()
    {
        return {buffer_.get(), pixel_type_, width_, height_, gap_size_};
    }

    const unsigned char* Image::data() const
    {
        return buffer_.get();
    }

    unsigned char* Image::data()
    {
        return buffer_.get();
    }

    size_t Image::width() const
    {
        return width_;
    }

    size_t Image::height() const
    {
        return height_;
    }

    size_t Image::row_size() const
    {
        return gap_size_ + (width_ * get_pixel_size(pixel_type_)) / 8;
    }

    size_t Image::size() const
    {
        return height_ * row_size();
    }

    PixelType Image::pixel_type() const
    {
        return pixel_type_;
    }

    std::unique_ptr<unsigned char> Image::release()
    {
        width_ = height_ = gap_size_ = 0;
        pixel_type_ = PixelType::NONE;
        return std::move(buffer_);
    }
}
