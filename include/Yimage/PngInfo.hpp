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

namespace Yimage
{
    class PngInfo
    {
    public:
        [[nodiscard]]
        uint32_t width() const;

        PngInfo& width(uint32_t value);

        [[nodiscard]]
        uint32_t height() const;

        PngInfo& height(uint32_t value);

        [[nodiscard]]
        int32_t bit_depth() const;

        PngInfo& bit_depth(int value);

        [[nodiscard]]
        int32_t color_type() const;

        PngInfo& color_type(int value);

        [[nodiscard]]
        int32_t interlace_type() const;

        PngInfo& interlace_type(int value);

        [[nodiscard]]
        int32_t compression_method() const;

        PngInfo& compression_method(int value);

        [[nodiscard]]
        int32_t filter_method() const;

        PngInfo& filter_method(int value);

        [[nodiscard]]
        const std::optional<double>& gamma() const;

        PngInfo& gamma(std::optional<double> gamma);

        using Text = std::tuple<std::string, std::string, int>;

        [[nodiscard]]
        const std::vector<png_text>& texts() const;

        PngInfo& add_text(std::string key, std::string value);

        PngInfo& add_text(std::string key, const std::string& value,
                          int32_t compression);

        [[nodiscard]]
        const std::optional<std::pair<uint32_t, uint32_t>>& pixels_per_meter() const;

        PngInfo& pixels_per_meter(uint32_t value);

        PngInfo& pixels_per_meter(uint32_t hor_value, uint32_t ver_value);
    private:
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        int32_t m_bit_depth = 8;
        int32_t m_color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        int32_t m_interlace_type = PNG_INTERLACE_NONE;
        int32_t m_compression_method = PNG_COMPRESSION_TYPE_DEFAULT;
        int32_t m_filter_method = PNG_FILTER_TYPE_DEFAULT;
        std::optional<double> m_gamma;
        std::vector<png_text> m_texts;
        std::list<std::string> m_strings;
        std::optional<std::pair<uint32_t, uint32_t>> m_pixels_per_meter;
    };
}
