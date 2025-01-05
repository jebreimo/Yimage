//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "TiffMetadata.hpp"

namespace Yimage
{
    class GeoTiffMetadata : public TiffMetadata
    {
    public:
        [[nodiscard]] GeoTiffMetadata* clone() const override;

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
