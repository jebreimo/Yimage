//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ImageView.hpp"

namespace Yimage
{
    class MutableImageView
    {
    public:
        MutableImageView();

        explicit MutableImageView(Image& img);

        MutableImageView(unsigned char* buffer,
                         PixelType pixel_type,
                         size_t width,
                         size_t height,
                         size_t row_gap_size = 0,
                         ImageMetadata* metadata = nullptr);

        explicit constexpr operator bool() const
        {
            return buffer_ && width_ && height_;
        }

        [[nodiscard]]
        ImageMetadata* metadata() const
        {
            return metadata_;
        }

        [[nodiscard]]
        constexpr unsigned char*
        pixel_pointer(size_t x, size_t y) const
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr std::pair<unsigned char*, unsigned char*>
        row(size_t index) const
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        constexpr unsigned char* data() const
        {
            return buffer_;
        }

        [[nodiscard]]
        constexpr size_t width() const
        {
            return width_;
        }

        [[nodiscard]]
        constexpr size_t height() const
        {
            return height_;
        }

        [[nodiscard]]
        constexpr size_t row_size() const
        {
            return gap_size_ + width_ * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr size_t size() const
        {
            auto last_row = width_ * pixel_size_ / 8;
            return height_  == 0 ? 0 : (height_ - 1) * row_size() + last_row;
        }

        [[nodiscard]]
        constexpr size_t pixel_size() const
        {
            return pixel_size_;
        }

        [[nodiscard]]
        constexpr PixelType pixel_type() const
        {
            return pixel_type_;
        }

        [[nodiscard]]
        constexpr bool is_contiguous() const
        {
            return gap_size_ == 0 || height_ <= 1;
        }

        [[nodiscard]]
        constexpr size_t row_gap_size() const
        {
            return gap_size_;
        }

        [[nodiscard]]
        MutableImageView subimage(size_t x, size_t y) const;

        [[nodiscard]]
        MutableImageView
        subimage(size_t x, size_t y, size_t width, size_t height) const;
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        size_t pixel_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        unsigned char* buffer_ = nullptr;
        ImageMetadata* metadata_ = nullptr;
    };

    bool operator==(const MutableImageView& a, const MutableImageView& b);

    void set_rgba8(const MutableImageView& image, size_t x, size_t y, Rgba8 rgba);
}
