//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <list>
#include <optional>
#include <string>
#include <vector>
#include <png.h>
#include "../ImageMetadata.hpp"

namespace Yimage
{
    class PngMetadata : public ImageMetadata
    {
    public:
        PngMetadata();

        using Text = std::tuple<std::string, std::string, int>;

        [[nodiscard]]
        const std::vector<png_text>& texts() const;

        void add_text(std::string key, std::string value);

        void add_text(std::string key, std::string value,
                      int32_t compression);

        uint32_t width = 0;
        uint32_t height = 0;
        int32_t bit_depth = 8;
        int32_t color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        int32_t interlace_type = PNG_INTERLACE_NONE;
        int32_t compression_method = PNG_COMPRESSION_TYPE_DEFAULT;
        int32_t filter_method = PNG_FILTER_TYPE_DEFAULT;
        std::optional<double> gamma;
        std::optional<std::pair<uint32_t, uint32_t>> pixels_per_meter;

    private:
        std::vector<png_text> texts_;
        std::list<std::string> strings_;
    };
}
