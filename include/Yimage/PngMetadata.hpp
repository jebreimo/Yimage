//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <list>
#include <optional>
#include <string>
#include <vector>
#include <png.h>
#include "ImageMetadata.hpp"

namespace Yimage
{
    class PngMetadata : public ImageMetadata
    {
    public:
        PngMetadata();

        [[nodiscard]]
        uint32_t width() const;

        void set_width(uint32_t value);

        [[nodiscard]]
        uint32_t height() const;

        void set_height(uint32_t value);

        [[nodiscard]]
        int32_t bit_depth() const;

        void set_bit_depth(int value);

        [[nodiscard]]
        int32_t color_type() const;

        void set_color_type(int value);

        [[nodiscard]]
        int32_t interlace_type() const;

        void set_interlace_type(int value);

        [[nodiscard]]
        int32_t compression_method() const;

        void set_compression_method(int value);

        [[nodiscard]]
        int32_t filter_method() const;

        void set_filter_method(int value);

        [[nodiscard]]
        const std::optional<double>& gamma() const;

        void set_gamma(std::optional<double> gamma);

        using Text = std::tuple<std::string, std::string, int>;

        [[nodiscard]]
        const std::vector<png_text>& texts() const;

        void add_text(std::string key, std::string value);

        void add_text(std::string key, std::string value,
                              int32_t compression);

        [[nodiscard]]
        const std::optional<std::pair<uint32_t, uint32_t>>& pixels_per_meter() const;

        void set_pixels_per_meter(uint32_t value);

        void set_pixels_per_meter(uint32_t hor_value, uint32_t ver_value);

    private:
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        int32_t bit_depth_ = 8;
        int32_t color_type_ = PNG_COLOR_TYPE_RGB_ALPHA;
        int32_t interlace_type_ = PNG_INTERLACE_NONE;
        int32_t compression_method_ = PNG_COMPRESSION_TYPE_DEFAULT;
        int32_t filter_method_ = PNG_FILTER_TYPE_DEFAULT;
        std::optional<double> gamma_;
        std::vector<png_text> texts_;
        std::list<std::string> strings_;
        std::optional<std::pair<uint32_t, uint32_t>> pixels_per_meter_;
    };
}
