//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ImageView.hpp"
#include "IMutImage.hpp"

namespace Yimage
{
    class MutImageView : public IMutImage
    {
    public:
        MutImageView();

        explicit MutImageView(const IMutImage& img);

        MutImageView(unsigned char* buffer,
                     PixelType pixel_type,
                     size_t width,
                     size_t height,
                     size_t row_gap_size = 0);

        explicit operator bool() const final
        {
            return buffer_ && width_ && height_;
        }

        [[nodiscard]]
        constexpr const unsigned char*
        pixel_pointer(size_t x, size_t y) const final
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr unsigned char*
        mut_pixel_pointer(size_t x, size_t y) const final
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const final
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        constexpr std::pair<unsigned char*, unsigned char*>
        mut_row(size_t index) const final
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        constexpr const unsigned char* data() const final
        {
            return buffer_;
        }

        [[nodiscard]]
        constexpr unsigned char* mut_data() const final
        {
            return buffer_;
        }

        [[nodiscard]]
        constexpr size_t width() const final
        {
            return width_;
        }

        [[nodiscard]]
        constexpr size_t height() const final
        {
            return height_;
        }

        [[nodiscard]]
        constexpr size_t row_size() const final
        {
            return gap_size_ + width_ * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr size_t size() const final
        {
            auto last_row = width_ * pixel_size_ / 8;
            return height_  == 0 ? 0 : (height_ - 1) * row_size() + last_row;
        }

        [[nodiscard]]
        constexpr size_t pixel_size() const final
        {
            return pixel_size_;
        }

        [[nodiscard]]
        constexpr PixelType pixel_type() const final
        {
            return pixel_type_;
        }

        [[nodiscard]]
        constexpr bool is_contiguous() const final
        {
            return gap_size_ == 0 || height_ <= 1;
        }

        [[nodiscard]]
        constexpr size_t row_gap_size() const final
        {
            return gap_size_;
        }

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y) const final;

        [[nodiscard]]
        ImageView
        subimage(size_t x, size_t y,
                 size_t width, size_t height) const final;

        [[nodiscard]]
        MutImageView mut_subimage(size_t x, size_t y) const final;

        [[nodiscard]]
        MutImageView
        mut_subimage(size_t x, size_t y,
                     size_t width, size_t height) const final;
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        size_t pixel_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        unsigned char* buffer_ = nullptr;
    };

    bool operator==(const MutImageView& a, const MutImageView& b);
}
