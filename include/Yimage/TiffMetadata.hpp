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
#include <memory>
#include <optional>
#include <vector>

#include "ImageMetadata.hpp"

namespace Yimage
{
    struct GeoTiffMetadata;

    struct Strip
    {
        uint64_t byte_count = 0;
        uint64_t offset = 0;
    };

    struct Strips
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

    struct Tiles
    {
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<Tile> tiles;
    };

    class TiffMetadata : public ImageMetadata
    {
    public:
        TiffMetadata();

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

        std::optional<Strips> strips;

        std::optional<Tiles> tiles;

        double min_sample_value = 0;
        double max_sample_value = 0;

        std::unique_ptr<GeoTiffMetadata> geo_tiff;
    };

    struct GeoTiffMetadata
    {
        std::array<double, 3> model_pixel_scale = {};
        std::array<double, 6> model_tie_point = {};
        std::array<double, 16> model_transformation = {};

        std::string citation;
        std::string geog_citation;
        std::string projected_citation;
        std::string gdal_metadata;

        double angular_unit_size = 0;
        double geog_semi_major_axis = 0;
        double geog_inv_flattening = 0;
        double geog_prime_meridian_longitude = 0;

        int16_t key_directory_version = 0;
        int16_t key_revision = 0;
        int16_t minor_revision = 0;
        int16_t model_type = 0;
        int16_t raster_type = 0;
        int16_t geodetic_crs = 0;
        int16_t geodetic_datum = 0;
        int16_t prime_meridian = 0;
        int16_t projected_crs = 0;
        int16_t geog_azimuth_units = 0;
        int16_t projection_zone = 0;
        int16_t projected_linear_units = 0;
        int16_t ellipsoid = 0;
        int16_t linear_units = 0;
        int16_t vertical_units = 0;
        int16_t angular_units = 0;
        int16_t coordinate_system = 0;
    };
}
