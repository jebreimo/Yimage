//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-26.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include "../ImageMetadata.hpp"

namespace Yimage
{
    struct Strip
    {
        uint64_t byte_count = 0;
        uint64_t offset = 0;
    };

    struct StripInfo
    {
        uint32_t rows_per_strip = 0;
        std::vector<Strip> strips;
    };

    struct Tile
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint64_t byte_count = 0;
        uint64_t offset = 0;
    };

    struct TileInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<Tile> tiles;
    };

    class TiffMetadata : public ImageMetadata
    {
    public:
        TiffMetadata();

        [[nodiscard]] TiffMetadata* clone() const override;

        uint32_t width = 0;
        uint32_t height = 0;

        uint16_t bits_per_sample = 0;
        uint16_t compression = 0;
        uint16_t fill_order = 0;
        uint16_t resolution_unit = 0;
        uint16_t planar_configuration = 0;
        uint16_t orientation = 0;
        uint16_t samples_per_pixel = 0;
        uint16_t predictor = 0;
        uint16_t sample_format = 0;
        // uint16_t color_map = 0;
        // uint16_t extra_samples = 0;

        std::array<uint16_t, 2> page_number = {};

        float x_resolution = 0;
        float y_resolution = 0;
        float x_position = 0;
        float y_position = 0;

        std::string software;
        std::string date_time;
        std::string artist;
        std::string host_computer;

        std::optional<StripInfo> strips;

        std::optional<TileInfo> tiles;

        double min_sample_value = 0;
        double max_sample_value = 0;
    };
}
