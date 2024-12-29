//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yimage/Image.hpp>

#include <algorithm>
#include "Yimage/YimageException.hpp"
#include "ImageUtilities.hpp"

namespace Yimage
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
          buffer_(std::move(buffer))
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

    Image::operator bool() const
    {
        return bool(buffer_);
    }

    const ImageMetadata* Image::metadata() const
    {
        return metadata_.get();
    }

    ImageMetadata* Image::metadata()
    {
        return metadata_.get();
    }

    void Image::set_metadata(std::unique_ptr<ImageMetadata> metadata)
    {
        metadata_ = std::move(metadata);
    }

    const unsigned char* Image::pixel_pointer(size_t x, size_t y) const
    {
        return ImageView(*this).pixel_pointer(x, y);
    }

    unsigned char* Image::pixel_pointer(size_t x, size_t y)
    {
        return MutableImageView(*this).pixel_pointer(x, y);
    }

    std::pair<const unsigned char*, const unsigned char*>
    Image::row(size_t index) const
    {
        return ImageView(*this).row(index);
    }

    std::pair<unsigned char*, unsigned char*>
    Image::row(size_t index)
    {
        return MutableImageView(*this).row(index);
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

    size_t Image::pixel_size() const
    {
        return get_pixel_size(pixel_type_);
    }

    bool Image::is_contiguous() const
    {
        return gap_size_ == 0 || height_ <= 1;
    }

    size_t Image::row_gap_size() const
    {
        return gap_size_;
    }

    ImageView Image::view() const
    {
        return ImageView(*this);
    }

    ImageView Image::subimage(size_t x, size_t y) const
    {
        return subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    ImageView
    Image::subimage(size_t x, size_t y, size_t width, size_t height) const
    {
        return make_subimage<ImageView>(*this, x, y, width, height);
    }

    MutableImageView Image::mutable_view()
    {
        return MutableImageView(*this);
    }

    MutableImageView Image::mutable_subimage(size_t x, size_t y)
    {
        return mutable_subimage(x, y, SIZE_MAX, SIZE_MAX);
    }

    MutableImageView
    Image::mutable_subimage(size_t x, size_t y, size_t width, size_t height)
    {
        return make_subimage<MutableImageView>(*this, x, y, width, height);
    }

    std::unique_ptr<unsigned char> Image::release()
    {
        width_ = height_ = gap_size_ = 0;
        pixel_type_ = PixelType::NONE;
        return std::move(buffer_);
    }
}
