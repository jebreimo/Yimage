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
#include "ImageMetadata.hpp"
#include "ImageView.hpp"
#include "MutableImageView.hpp"

namespace Yimage
{
    class Image
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
        explicit operator bool() const;

        [[nodiscard]]
        const ImageMetadata* metadata() const;

        [[nodiscard]]
        ImageMetadata* metadata();

        void set_metadata(std::unique_ptr<ImageMetadata> metadata);

        [[nodiscard]]
        const unsigned char* pixel_pointer(size_t x, size_t y) const;

        [[nodiscard]]
        unsigned char* pixel_pointer(size_t x, size_t y);

        [[nodiscard]]
        std::pair<const unsigned char*, const unsigned char*>
        row(size_t index) const;

        [[nodiscard]]
        std::pair<unsigned char*, unsigned char*>
        row(size_t index);

        [[nodiscard]]
        const unsigned char* data() const;

        [[nodiscard]]
        unsigned char* data();

        [[nodiscard]]
        size_t width() const;

        [[nodiscard]]
        size_t height() const;

        [[nodiscard]]
        size_t row_size() const;

        [[nodiscard]]
        size_t size() const;

        /**
         * @brief Returns the pixel size measured in bits.
         */
        [[nodiscard]]
        size_t pixel_size() const;

        [[nodiscard]]
        PixelType pixel_type() const;

        [[nodiscard]]
        bool is_contiguous() const;

        [[nodiscard]]
        size_t row_gap_size() const;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y) const;

        [[nodiscard]]
        ImageView subimage(size_t x, size_t y,
                           size_t width, size_t height) const;

        [[nodiscard]]
        MutableImageView mutable_subimage(size_t x, size_t y);

        [[nodiscard]]
        MutableImageView mutable_subimage(size_t x, size_t y,
                                          size_t width, size_t height);

        std::unique_ptr<unsigned char> release();
    private:
        size_t width_ = 0;
        size_t height_ = 0;
        size_t gap_size_ = 0;
        PixelType pixel_type_ = PixelType::NONE;
        std::unique_ptr<unsigned char> buffer_;
        std::unique_ptr<ImageMetadata> metadata_;
    };
}
