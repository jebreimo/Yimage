//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include "ImageView.hpp"
#include "IMutImage.hpp"
#include "MutImageView.hpp"

namespace Yimage
{
    class Image : public IMutImage
    {
    public:
        Image();

        Image(std::unique_ptr<unsigned char> buffer,
              PixelType pixel_type,
              size_t width, size_t height, size_t row_gap_size = 0);

        Image(PixelType pixel_type, size_t width, size_t height,
              size_t row_gap_size = 0);

        Image(const Image& rhs);

        Image(Image&& rhs) noexcept;

        Image& operator=(const Image& rhs);

        Image& operator=(Image&& rhs) noexcept;

        [[nodiscard]]
        explicit operator bool() const final;

        [[nodiscard]]
        const unsigned char* pixel_pointer(size_t x, size_t y) const override;

        [[nodiscard]]
        unsigned char* mut_pixel_pointer(size_t x, size_t y) const override;

        [[nodiscard]]
        std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const override;

        [[nodiscard]]
        std::pair<unsigned char*, unsigned char*>
        mut_row(size_t index) const override;

        [[nodiscard]]
        const unsigned char* data() const final;

        [[nodiscard]]
        unsigned char* mut_data() const final;

        [[nodiscard]]
        size_t width() const override;

        [[nodiscard]]
        size_t height() const override;

        [[nodiscard]]
        size_t row_size() const override;

        [[nodiscard]]
        size_t size() const override;

        [[nodiscard]]
        size_t pixel_size() const override;

        [[nodiscard]]
        PixelType pixel_type() const override;

        [[nodiscard]]
        bool is_contiguous() const override;

        [[nodiscard]]
        size_t row_gap_size() const override;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y) const override;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y,
                           size_t width, size_t height) const override;

        [[nodiscard]]
        MutImageView mut_subimage(size_t x, size_t y) const override;

        [[nodiscard]]
        MutImageView mut_subimage(size_t x, size_t y,
                                  size_t width, size_t height) const override;

        std::unique_ptr<unsigned char> release();
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        std::unique_ptr<unsigned char> buffer_;
    };
}
