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
#include "PixelType.hpp"

namespace yimage
{
    class ImageView
    {
    public:
        ImageView();

        ImageView(const unsigned char* buffer,
                  PixelType pixel_type,
                  size_t width,
                  size_t height,
                  size_t row_gap_size = 0);

        explicit operator bool()
        {
            return buffer_ && width_ && height_;
        }

        [[nodiscard]]
        constexpr const unsigned char* pixel_pointer(size_t x, size_t y) const
        {
            return buffer_ + y * row_size() + x * pixel_size_ / 8;
        }

        [[nodiscard]]
        constexpr std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const
        {
            auto start = buffer_ + index * row_size();
            return {start, start + width_ * pixel_size_ / 8};
        }

        [[nodiscard]]
        constexpr const unsigned char* data() const
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
            return height_ == 0 ? 0 : (height_ - 1) * row_size() + last_row;
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
        ImageView subimage(size_t x, size_t y,
                           size_t width = SIZE_MAX,
                           size_t height = SIZE_MAX) const;
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        size_t pixel_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        const unsigned char* buffer_ = nullptr;
    };

    bool operator==(const ImageView& a, const ImageView& b);

    struct Rgba8
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    bool operator==(const Rgba8& a, const Rgba8& b);

    bool operator!=(const Rgba8& a, const Rgba8& b);

    std::string to_string(const Rgba8& rgba);

    std::ostream& operator<<(std::ostream& os, const Rgba8& rgba);

    Rgba8 get_rgba8(const ImageView& image, size_t x, size_t y);
}
