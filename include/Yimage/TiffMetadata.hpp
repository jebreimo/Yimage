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
#include <vector>

#include "ImageMetadata.hpp"

namespace Yimage
{
    struct GeoTiffMetadata;

    class TiffMetadata : public ImageMetadata
    {
    public:
        TiffMetadata();

        uint16_t bits_per_sample = 0;
        uint16_t compression = 0;
        uint16_t fill_order = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t rows_per_strip = 0;
        // uint64_t strip_byte_counts = 0;
        float x_resolution = 0;
        float y_resolution = 0;
        uint16_t resolution_unit = 0;
        uint16_t planar_configuration = 0;
        uint16_t orientation = 0;
        uint16_t samples_per_pixel = 0;
        double min_sample_value = 0;
        double max_sample_value = 0;
        float x_position = 0;
        float y_position = 0;
        std::array<uint16_t, 2> page_number = {};
        std::string software;
        std::string date_time;
        std::string artist;
        std::string host_computer;
        uint16_t predictor = 0;
        // uint16_t color_map = 0;
        // uint16_t extra_samples = 0;
        uint16_t sample_format = 0;

        std::unique_ptr<GeoTiffMetadata> geo_tiff;
    };

    struct GeoTiffMetadata
    {
        std::array<double, 3> model_pixel_scale = {};
        std::array<double, 6> model_tie_point = {};
        std::array<double, 16> model_transformation = {};
        int16_t key_directory_version = 0;
        int16_t key_revision = 0;
        int16_t minor_revision = 0;
        int16_t model_type = 0;
        int16_t raster_type = 0;
        int16_t geodetic_crs = 0;

        std::string citation;
        std::string geog_citation;
        std::string projected_citation;
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
        // int16_t citation_date = 0;
        int16_t coordinate_system = 0;
        double angular_unit_size = 0;
        double geog_semi_major_axis = 0;
        double geog_inv_flattening = 0;
        double geog_prime_meridian_longitude = 0;
        // std::string vertical_citation;
        // std::string vertical_citation_date;
        // std::string vertical_coordinate_system;
        // std::string vertical_datum_shift;
        // std::string vertical_datum_shift_date;
        // std::string vertical_datum_shift_units;
        // std::string vertical_datum_shift_model;
        // std::string vertical_datum_shift_model_date;
        // std::string vertical_datum_shift_model_units;
        // std::string vertical_datum_shift_model_description;
        // std::string vertical_datum_shift_model_citation;
        // std::string vertical_datum_shift_model_citation_date;
        // std::string vertical_datum_shift_model_citation_author;
        // std::string vertical_datum_shift_model_citation_publisher;
        // std::string vertical_datum_shift_model_citation_publication_date;
        // std::string vertical_datum_shift_model_citation_title;
        // std::string vertical_datum_shift_model_citation_edition;
        // std::string vertical_datum_shift_model_citation_series;
        // std::string vertical_datum_shift_model_citation_series_number;
        // std::string vertical_datum_shift_model_citation_institution;
        // std::string vertical_datum_shift_model_citation_institution_id;
        // std::string vertical_datum_shift_model_citation_institution_address;
        // std::string vertical_datum_shift_model_citation_institution_email;
        // std::string vertical_datum_shift_model_citation_institution_telephone;
        // std::string vertical_datum_shift_model_citation_institution_fax;
        // std::string vertical_datum_shift_model_citation_institution_url;
        // std::string vertical_datum_shift_model_citation_institution_logo;
    };
}
