//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <iosfwd>
#include <utility>
#include "IImage.hpp"
#include "PixelType.hpp"

namespace Yimage
{
    class ImageView : public IImage
    {
    public:
        ImageView();

        explicit ImageView(const IImage& img)
            : ImageView(img.data(), img.pixel_type(),
                        img.width(), img.height(),
                        img.row_gap_size())
        {}

        ImageView(const unsigned char* buffer,
                  PixelType pixel_type,
                  size_t width,
                  size_t height,
                  size_t row_gap_size = 0);

        explicit operator bool() const final
        {
            return buffer_ && width_ && height_;
        }

        [[nodiscard]]
        const unsigned char*
        pixel_pointer(size_t x, size_t y) const final
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const final
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        const unsigned char* data() const final
        {
            return buffer_;
        }

        [[nodiscard]]
        size_t width() const final
        {
            return width_;
        }

        [[nodiscard]]
        size_t height() const final
        {
            return height_;
        }

        [[nodiscard]]
        size_t row_size() const final
        {
            return gap_size_ + width_ * pixel_size_ / 8;
        }

        [[nodiscard]]
        size_t size() const final
        {
            auto last_row = width_ * pixel_size_ / 8;
            return height_ == 0 ? 0 : (height_ - 1) * row_size() + last_row;
        }

        [[nodiscard]]
        size_t pixel_size() const final
        {
            return pixel_size_;
        }

        [[nodiscard]]
        PixelType pixel_type() const final
        {
            return pixel_type_;
        }

        [[nodiscard]]
        bool is_contiguous() const final
        {
            return gap_size_ == 0 || height_ <= 1;
        }

        [[nodiscard]]
        size_t row_gap_size() const final
        {
            return gap_size_;
        }

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y) const final;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y,
                           size_t width, size_t height) const final;
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        size_t pixel_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        const unsigned char* buffer_ = nullptr;
    };

    bool operator==(const ImageView& a, const ImageView& b);
}
