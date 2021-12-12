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

namespace yimage
{
    class PngInfo
    {
    public:
        [[nodiscard]]
        unsigned width() const;

        PngInfo& width(unsigned value);

        [[nodiscard]]
        unsigned height() const;

        PngInfo& height(unsigned value);

        [[nodiscard]]
        int bit_depth() const;

        PngInfo& bit_depth(int value);

        [[nodiscard]]
        int color_type() const;

        PngInfo& color_type(int value);

        [[nodiscard]]
        int interlace_type() const;

        PngInfo& interlace_type(int value);

        [[nodiscard]]
        int compression_method() const;

        PngInfo& compression_method(int value);

        [[nodiscard]]
        int filter_method() const;

        PngInfo& filter_method(int value);

        [[nodiscard]]
        const std::optional<double>& gamma() const;

        PngInfo& gamma(std::optional<double> gamma);

        using Text = std::tuple<std::string, std::string, int>;

        [[nodiscard]]
        const std::vector<png_text>& texts() const;

        PngInfo& add_text(std::string key, std::string value);

        PngInfo& add_text(std::string key, std::string value, int compression);

        [[nodiscard]]
        const std::optional<std::pair<unsigned, unsigned>>& pixels_per_meter() const;

        PngInfo& pixels_per_meter(unsigned value);

        PngInfo& pixels_per_meter(unsigned hor_value, unsigned ver_value);
    private:
        unsigned m_width = 0;
        unsigned m_height = 0;
        int m_bit_depth = 8;
        int m_color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        int m_interlace_type = PNG_INTERLACE_NONE;
        int m_compression_method = PNG_COMPRESSION_TYPE_DEFAULT;
        int m_filter_method = PNG_FILTER_TYPE_DEFAULT;
        std::optional<double> m_gamma;
        std::vector<png_text> m_texts;
        std::list<std::string> m_strings;
        std::optional<std::pair<unsigned, unsigned>> m_pixels_per_meter;
    };
}
