//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ReadGeoTiffMetadata.hpp"

#include <algorithm>
#include <array>
#include <vector>
#include <tiffio.h>

#include "Yimage/YimageException.hpp"

namespace Yimage
{
    namespace
    {
        constexpr int GEOTIFF_TAG_GEO_KEY_DIRECTORY = 34735;
        constexpr int GEOTIFF_TAG_GEO_DOUBLE_PARAMS = 34736;
        constexpr int GEOTIFF_TAG_GEO_ASCII_PARAMS = 34737;
        constexpr int GEOTIFF_TAG_MODEL_PIXEL_SCALE = 33550;
        constexpr int GEOTIFF_TAG_MODEL_TIE_POINT = 33922;
        constexpr int GEOTIFF_TAG_MODEL_TRANSFORMATION = 34264;
        constexpr int GEOTIFF_TAG_GDAL_METADATA = 42112;
        constexpr int GEOTIFF_TAG_GDAL_NODATA = 42113;

        constexpr int GEOTIFF_KEY_MODEL_TYPE = 1024;
        constexpr int GEOTIFF_KEY_RASTER_TYPE = 1025;
        constexpr int GEOTIFF_KEY_CITATION = 1026;
        constexpr int GEOTIFF_KEY_GEODETIC_CRS = 2048;
        constexpr int GEOTIFF_KEY_GEOG_CITATION = 2049;
        constexpr int GEOTIFF_KEY_GEODETIC_DATUM = 2050;
        constexpr int GEOTIFF_KEY_PRIME_MERIDIAN = 2051;
        // constexpr int GEOTIFF_KEY_GEOG_LINEAR_UNITS = 2052;
        // constexpr int GEOTIFF_KEY_GEOG_LINEAR_UNIT_SIZE = 2053;
        constexpr int GEOTIFF_KEY_GEOG_ANGULAR_UNITS = 2054;
        constexpr int GEOTIFF_KEY_GEOG_ANGULAR_UNIT_SIZE = 2055;
        constexpr int GEOTIFF_KEY_ELLIPSOID = 2056;
        constexpr int GEOTIFF_KEY_GEOG_SEMI_MAJOR_AXIS = 2057;
        // constexpr int GEOTIFF_KEY_GEOG_SEMI_MINOR_AXIS = 2058;
        constexpr int GEOTIFF_KEY_GEOG_INV_FLATTENING = 2059;
        // constexpr int GEOTIFF_KEY_GEOG_AZIMUTH_UNITS = 2060;
        constexpr int GEOTIFF_KEY_GEOG_PRIME_MERIDIAN_LONGITUDE = 2061;
        constexpr int GEOTIFF_KEY_PROJECTED_CRS = 3072;
        constexpr int GEOTIFF_KEY_PROJECTED_CITATION = 3073;
        // constexpr int GEOTIFF_KEY_PROJECTED_PROJECTION = 3074;
        // constexpr int GEOTIFF_KEY_PROJECTED_ZONE = 3075;
        constexpr int GEOTIFF_KEY_PROJECTED_LINEAR_UNITS = 3076;
        // constexpr int GEOTIFF_KEY_PROJECTED_LINEAR_UNIT_SIZE = 3077;
        // constexpr int GEOTIFF_KEY_PROJECTED_STD_PARALLEL1 = 3078;
        // constexpr int GEOTIFF_KEY_PROJECTED_STD_PARALLEL2 = 3079;
        // constexpr int GEOTIFF_KEY_PROJECTED_NAT_ORIGIN_LONG = 3080;
        // constexpr int GEOTIFF_KEY_PROJECTED_NAT_ORIGIN_LAT = 3081;
        // constexpr int GEOTIFF_KEY_PROJECTED_FALSE_EASTING = 3082;
        // constexpr int GEOTIFF_KEY_PROJECTED_FALSE_NORTHING = 3083;
        constexpr int GEOTIFF_KEY_VERTICAL_CRS = 4096;
        // constexpr int GEOTIFF_KEY_VERTICAL_CITATION = 4097;
        // constexpr int GEOTIFF_KEY_VERTICAL_DATUM = 4098;
        // constexpr int GEOTIFF_KEY_VERTICAL_UNITS = 4099;
        // constexpr int GEOTIFF_KEY_VERTICAL_UNIT_SIZE = 4100;
        // constexpr int GEOTIFF_KEY_VERTICAL_AUTHORITY_CODE = 4101;
        // constexpr int GEOTIFF_KEY_VERTICAL_AUTHORITY_NAME = 4102;

        constexpr unsigned char TIFF_TRUE = 1;
        constexpr unsigned char TIFF_FALSE = 0;

        const TIFFFieldInfo GEOTIFF_FIELDS[] = {
            /* XXX Insert Your tags here */
            {
                GEOTIFF_TAG_MODEL_PIXEL_SCALE, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_TRUE, const_cast<char*>("GeoPixelScale")
            },
            {
                GEOTIFF_TAG_MODEL_TRANSFORMATION, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_TRUE, const_cast<char*>("GeoTransformationMatrix")
            },
            {
                GEOTIFF_TAG_MODEL_TIE_POINT, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_TRUE, const_cast<char*>("GeoTiePoints")
            },
            {
                GEOTIFF_TAG_GEO_KEY_DIRECTORY, -1, -1, TIFF_SHORT, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_TRUE, const_cast<char*>("GeoKeyDirectory")
            },
            {
                GEOTIFF_TAG_GEO_DOUBLE_PARAMS, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_TRUE, const_cast<char*>("GeoDoubleParams")
            },
            {
                GEOTIFF_TAG_GEO_ASCII_PARAMS, -1, -1, TIFF_ASCII, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_FALSE, const_cast<char*>("GeoASCIIParams")
            },
            {
                GEOTIFF_TAG_GDAL_METADATA, -1, -1, TIFF_ASCII, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_FALSE, const_cast<char*>("GDALMetadata")
            },
            {
                GEOTIFF_TAG_GDAL_NODATA, -1, -1, TIFF_ASCII, FIELD_CUSTOM,
                TIFF_TRUE, TIFF_FALSE, const_cast<char*>("GDALNoData")
            }
        };

        template <typename T>
        bool get_field(TIFF* tiff, int tag, std::vector<T>& value)
        {
            uint32_t count = 0;
            T* values = {};
            if (TIFFGetField(tiff, tag, &count, &values) == 0)
                return false;

            value.resize(count);
            std::copy_n(values, count, value.begin());
            return true;
        }

        template <typename T, size_t N>
        bool get_field(TIFF* tiff, int tag, std::array<T, N>& value)
        {
            uint32_t count = 0;
            T* values = {};
            if (TIFFGetField(tiff, tag, &count, &values) == 0)
                return false;

            if (count != N)
                return false;

            std::copy_n(values, count, value.data());
            return true;
        }

        bool get_field(TIFF* tiff, int tag, std::string& value)
        {
            const char* str = nullptr;
            if (TIFFGetField(tiff, tag, &str) == 0)
                return false;

            value = str;
            return true;
        }

        void process_geo_key_directory_entry(const int16_t geo_key_directory[4],
                                             const std::string& geo_ascii_params,
                                             const std::vector<double>& geo_double_params,
                                             GeoTiffMetadata* metadata)
        {
            const int16_t key_id = geo_key_directory[0];
            const int tiff_tag = 65536 + geo_key_directory[1];
            const int16_t count = geo_key_directory[2];
            const int16_t value = geo_key_directory[3];
            switch (key_id)
            {
            case GEOTIFF_KEY_MODEL_TYPE:
                metadata->model_type = value;
                break;
            case GEOTIFF_KEY_RASTER_TYPE:
                metadata->raster_type = value;
                break;
            case GEOTIFF_KEY_CITATION:
                if (tiff_tag == GEOTIFF_TAG_GEO_ASCII_PARAMS)
                    metadata->citation = geo_ascii_params.substr(value, count);
                break;
            case GEOTIFF_KEY_GEODETIC_CRS:
                metadata->geodetic_crs = value;
                break;
            case GEOTIFF_KEY_GEOG_CITATION:
                if (tiff_tag == GEOTIFF_TAG_GEO_ASCII_PARAMS)
                    metadata->geog_citation = geo_ascii_params.substr(value, count);
                break;
            case GEOTIFF_KEY_GEODETIC_DATUM:
                metadata->geodetic_datum = value;
                break;
            case GEOTIFF_KEY_PRIME_MERIDIAN:
                metadata->prime_meridian = value;
                break;
            case GEOTIFF_KEY_GEOG_ANGULAR_UNITS:
                metadata->angular_units = value;
                break;
            case GEOTIFF_KEY_GEOG_ANGULAR_UNIT_SIZE:
                if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    metadata->angular_unit_size = geo_double_params[value];
                break;
            case GEOTIFF_KEY_ELLIPSOID:
                metadata->ellipsoid = value;
                break;
            case GEOTIFF_KEY_GEOG_SEMI_MAJOR_AXIS:
                if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    metadata->geog_semi_major_axis = geo_double_params[value];
                break;
            case GEOTIFF_KEY_GEOG_INV_FLATTENING:
                if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    metadata->geog_inv_flattening = geo_double_params[value];
                break;
            case GEOTIFF_KEY_GEOG_PRIME_MERIDIAN_LONGITUDE:
                if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    metadata->geog_prime_meridian_longitude = geo_double_params[value];
                break;
            case GEOTIFF_KEY_PROJECTED_CRS:
                metadata->projected_crs = value;
                break;
            case GEOTIFF_KEY_VERTICAL_CRS:
                metadata->vertical_crs = value;
                break;
            case GEOTIFF_KEY_PROJECTED_CITATION:
                if (tiff_tag == GEOTIFF_TAG_GEO_ASCII_PARAMS)
                    metadata->projected_citation = geo_ascii_params.substr(value, count);
                break;
            case GEOTIFF_KEY_PROJECTED_LINEAR_UNITS:
                metadata->projected_linear_units = value;
                break;
            default:
                YIMAGE_THROW("Unknown GeoKey ID: " + std::to_string(key_id));
            }
        }

        void process_geo_key_directory_entries(const std::vector<int16_t>& geo_key_directory,
                                               const std::string& geo_ascii_params,
                                               const std::vector<double>& geo_double_params,
                                               GeoTiffMetadata* metadata)
        {
            metadata->key_directory_version = geo_key_directory[0];
            metadata->key_revision = geo_key_directory[1];
            metadata->minor_revision = geo_key_directory[2];

            for (size_t i = 4; i < geo_key_directory.size(); i += 4)
            {
                process_geo_key_directory_entry(geo_key_directory.data() + i,
                                                geo_ascii_params,
                                                geo_double_params,
                                                metadata);
            }
        }
    }

    void register_geotiff_tags(TIFF* tiff)
    {
        int error = TIFFMergeFieldInfo(tiff, GEOTIFF_FIELDS,
                                       std::size(GEOTIFF_FIELDS));
        if (error != 0)
            YIMAGE_THROW("Failed to register custom tags: " + std::to_string(error));
    }

    std::unique_ptr<GeoTiffMetadata> read_geotiff_metadata(TIFF* tiff)
    {
        std::vector<int16_t> geo_key_directory;
        if (!get_field(tiff, GEOTIFF_TAG_GEO_KEY_DIRECTORY, geo_key_directory))
            return {};

        std::string geo_ascii_params;
        get_field(tiff, GEOTIFF_TAG_GEO_ASCII_PARAMS, geo_ascii_params);

        std::vector<double> geo_double_params;
        get_field(tiff, GEOTIFF_TAG_GEO_DOUBLE_PARAMS, geo_double_params);

        auto metadata = std::make_unique<GeoTiffMetadata>();
        process_geo_key_directory_entries(geo_key_directory,
                                          geo_ascii_params,
                                          geo_double_params,
                                          metadata.get());

        get_field(tiff, GEOTIFF_TAG_MODEL_PIXEL_SCALE, metadata->model_pixel_scale);
        get_field(tiff, GEOTIFF_TAG_MODEL_TIE_POINT, metadata->model_tie_point);
        get_field(tiff, GEOTIFF_TAG_MODEL_TRANSFORMATION,
                  metadata->model_transformation);
        get_field(tiff, GEOTIFF_TAG_GDAL_METADATA, metadata->gdal_metadata);
        get_field(tiff, GEOTIFF_TAG_GDAL_NODATA, metadata->gdal_no_data);

        return metadata;
    }
}
