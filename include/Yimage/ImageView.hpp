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

        explicit operator bool() const override
        {
            return buffer_ && width_ && height_;
        }

        [[nodiscard]]
        constexpr const unsigned char*
        pixel_pointer(size_t x, size_t y) const override
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const override
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        constexpr const unsigned char* data() const override
        {
            return buffer_;
        }

        [[nodiscard]]
        constexpr size_t width() const override
        {
            return width_;
        }

        [[nodiscard]]
        constexpr size_t height() const override
        {
            return height_;
        }

        [[nodiscard]]
        constexpr size_t row_size() const override
        {
            return gap_size_ + width_ * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr size_t size() const override
        {
            auto last_row = width_ * pixel_size_ / 8;
            return height_ == 0 ? 0 : (height_ - 1) * row_size() + last_row;
        }

        [[nodiscard]]
        constexpr size_t pixel_size() const override
        {
            return pixel_size_;
        }

        [[nodiscard]]
        constexpr PixelType pixel_type() const override
        {
            return pixel_type_;
        }

        [[nodiscard]]
        constexpr bool is_contiguous() const override
        {
            return gap_size_ == 0 || height_ <= 1;
        }

        [[nodiscard]]
        constexpr size_t row_gap_size() const override
        {
            return gap_size_;
        }

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y) const override;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y,
                           size_t width, size_t height) const override;
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
