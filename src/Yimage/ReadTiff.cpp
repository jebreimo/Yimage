//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadTiff.hpp"

#include <tiffio.h>
#include "Yimage/TiffMetadata.hpp"
#include "Yimage/ImageAlgorithms.hpp"

namespace Yimage
{
    namespace
    {
        template <typename T>
        bool get_field(TIFF* tiff, int tag, T& value)
        {
            return TIFFGetField(tiff, tag, &value) == 1;
        }

        bool get_field(TIFF* tiff, int tag, std::string& value)
        {
            const char* str = nullptr;
            if (TIFFGetField(tiff, tag, &str) == 0)
                return false;

            value = str;
            return true;
        }

        bool get_field(TIFF* tiff, int tag, std::array<int16_t, 2>& value)
        {
            if (TIFFGetField(tiff, tag, &value[0], &value[1]) == 0)
                return false;
            return true;
        }

        constexpr int GEOTIFF_TAG_GEO_KEY_DIRECTORY = 34735;
        constexpr int GEOTIFF_TAG_GEO_DOUBLE_PARAMS = 34736;
        constexpr int GEOTIFF_TAG_GEO_ASCII_PARAMS = 34737;
        constexpr int GEOTIFF_TAG_MODEL_PIXEL_SCALE = 33550;
        constexpr int GEOTIFF_TAG_MODEL_TIE_POINT = 33922;
        constexpr int GEOTIFF_TAG_MODEL_TRANSFORMATION = 34264;
        constexpr int GEOTIFF_TAG_GDAL_METADATA = 42112;

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
        // constexpr int GEOTIFF_KEY_VERTICAL_CRS = 4096;
        // constexpr int GEOTIFF_KEY_VERTICAL_CITATION = 4097;
        // constexpr int GEOTIFF_KEY_VERTICAL_DATUM = 4098;
        // constexpr int GEOTIFF_KEY_VERTICAL_UNITS = 4099;
        // constexpr int GEOTIFF_KEY_VERTICAL_UNIT_SIZE = 4100;
        // constexpr int GEOTIFF_KEY_VERTICAL_AUTHORITY_CODE = 4101;
        // constexpr int GEOTIFF_KEY_VERTICAL_AUTHORITY_NAME = 4102;

        template <typename T>
        bool get_custom_field(TIFF* tiff, int tag, std::vector<T>& value)
        {
            uint32_t count = 0;
            T* values = {};
            if (TIFFGetField(tiff, tag, &count, &values) == 0)
                return false;

            value.resize(count);
            std::copy_n(values, count, value.begin());
            return true;
        }

        bool get_custom_field(TIFF* tiff, int tag, std::string& value)
        {
            uint32_t count = 0;
            char* values = {};
            if (TIFFGetField(tiff, tag, &count, &values) == 0)
                return false;

            value.assign(values, count);
            return true;
        }

        template <typename T, size_t N>
        bool get_custom_field(TIFF* tiff, int tag, std::array<T, N>& value)
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

        std::unique_ptr<GeoTiffMetadata> read_geotiff_metadata(TIFF* tiff)
        {
            std::vector<int16_t> geo_key_directory;
            if (!get_custom_field(tiff, GEOTIFF_TAG_GEO_KEY_DIRECTORY, geo_key_directory))
                return {};

            std::vector<char> geo_ascii_params;
            get_custom_field(tiff, GEOTIFF_TAG_GEO_ASCII_PARAMS, geo_ascii_params);

            std::vector<double> geo_double_params;
            get_custom_field(tiff, GEOTIFF_TAG_GEO_DOUBLE_PARAMS, geo_double_params);

            auto metadata = std::make_unique<GeoTiffMetadata>();
            metadata->key_directory_version = geo_key_directory[0];
            metadata->key_revision = geo_key_directory[1];
            metadata->minor_revision = geo_key_directory[2];
            for (size_t i = 4; i < geo_key_directory.size(); i += 4)
            {
                int16_t key_id = geo_key_directory[i];
                int tiff_tag = 65536 + geo_key_directory[i + 1];
                int16_t count = geo_key_directory[i + 2];
                int16_t value = geo_key_directory[i + 3];
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
                    {
                        metadata->citation = std::string(&geo_ascii_params[value], count);
                    }
                    break;
                case GEOTIFF_KEY_GEODETIC_CRS:
                    metadata->geodetic_crs = value;
                    break;
                case GEOTIFF_KEY_GEOG_CITATION:
                    if (tiff_tag == GEOTIFF_TAG_GEO_ASCII_PARAMS)
                    {
                        metadata->geog_citation = std::string(&geo_ascii_params[value], count);
                    }
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
                    {
                        metadata->angular_unit_size = geo_double_params[value];
                    }
                    break;
                case GEOTIFF_KEY_ELLIPSOID:
                    metadata->ellipsoid = value;
                    break;
                case GEOTIFF_KEY_GEOG_SEMI_MAJOR_AXIS:
                    if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    {
                        metadata->geog_semi_major_axis = geo_double_params[value];
                    }
                    break;
                case GEOTIFF_KEY_GEOG_INV_FLATTENING:
                    if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    {
                        metadata->geog_inv_flattening = geo_double_params[value];
                    }
                    break;
                case GEOTIFF_KEY_GEOG_PRIME_MERIDIAN_LONGITUDE:
                    if (tiff_tag == GEOTIFF_TAG_GEO_DOUBLE_PARAMS)
                    {
                        metadata->geog_prime_meridian_longitude = geo_double_params[value];
                    }
                    break;
                case GEOTIFF_KEY_PROJECTED_CRS:
                    metadata->projected_crs = value;
                    break;
                case GEOTIFF_KEY_PROJECTED_CITATION:
                    if (tiff_tag == GEOTIFF_TAG_GEO_ASCII_PARAMS)
                    {
                        metadata->projected_citation = std::string(&geo_ascii_params[value], count);
                    }
                    break;
                case GEOTIFF_KEY_PROJECTED_LINEAR_UNITS:
                    metadata->projected_linear_units = value;
                    break;
                default:
                    break;
                }
            }

            get_custom_field(tiff, GEOTIFF_TAG_MODEL_PIXEL_SCALE, metadata->model_pixel_scale);
            get_custom_field(tiff, GEOTIFF_TAG_MODEL_TIE_POINT, metadata->model_tie_point);
            get_custom_field(tiff, GEOTIFF_TAG_MODEL_TRANSFORMATION,
                             metadata->model_transformation);
            get_custom_field(tiff, GEOTIFF_TAG_GDAL_METADATA, metadata->gdal_metadata);

            return metadata;
        }
    }

    struct TiffDeleter
    {
        void operator()(TIFF* tiff) const
        {
            TIFFClose(tiff);
        }
    };

    std::optional<Tiles> get_tiles(TIFF* tiff, uint32_t width, uint32_t height)
    {
        uint64_t* byte_counts = nullptr;
        uint64_t* offsets = nullptr;

        Tiles result;
        if (!get_field(tiff, TIFFTAG_TILEWIDTH, result.width)
            || !get_field(tiff, TIFFTAG_TILELENGTH, result.height)
            || !get_field(tiff, TIFFTAG_TILEBYTECOUNTS, byte_counts)
            || !get_field(tiff, TIFFTAG_TILEOFFSETS, offsets))
        {
            return {};
        }

        uint32_t rows = (height + result.height - 1) / result.height;
        uint32_t cols = (width + result.width - 1) / result.width;
        for (uint32_t y = 0; y < rows; ++y)
        {
            for (uint32_t x = 0; x < cols; ++x)
            {
                result.tiles.push_back({
                    x,
                    y,
                    byte_counts[y * cols + x],
                    offsets[y * cols + x]
                });
            }
        }

        return result;
    }

    std::optional<Strips> get_strips(TIFF* tiff, uint32_t height)
    {
        Strips strips;
        uint64_t* byte_counts = nullptr;
        uint64_t* offsets = nullptr;
        if (!get_field(tiff, TIFFTAG_ROWSPERSTRIP, strips.rows_per_strip)
            || !get_field(tiff, TIFFTAG_STRIPBYTECOUNTS, byte_counts)
            || !get_field(tiff, TIFFTAG_STRIPOFFSETS, offsets))
        {
            return {};
        }

        uint32_t rows = (height + strips.rows_per_strip - 1) / strips.rows_per_strip;
        for (uint32_t i = 0; i < rows; ++i)
            strips.strips.push_back({byte_counts[i], offsets[i]});

        return strips;
    }

    Image read_tiff(const std::string& path)
    {
        auto tiff = std::unique_ptr<TIFF, TiffDeleter>(TIFFOpen(path.c_str(), "r"));
        if (!tiff)
            return {};

        auto metadata = std::make_unique<TiffMetadata>();
        get_field(tiff.get(), TIFFTAG_IMAGEWIDTH, metadata->width);
        get_field(tiff.get(), TIFFTAG_IMAGELENGTH, metadata->height);
        get_field(tiff.get(), TIFFTAG_BITSPERSAMPLE, metadata->bits_per_sample);
        get_field(tiff.get(), TIFFTAG_SAMPLESPERPIXEL, metadata->samples_per_pixel);
        get_field(tiff.get(), TIFFTAG_SAMPLEFORMAT, metadata->sample_format);
        get_field(tiff.get(), TIFFTAG_MINSAMPLEVALUE, metadata->min_sample_value);
        get_field(tiff.get(), TIFFTAG_MAXSAMPLEVALUE, metadata->max_sample_value);
        get_field(tiff.get(), TIFFTAG_XPOSITION, metadata->x_position);
        get_field(tiff.get(), TIFFTAG_YPOSITION, metadata->y_position);
        get_field(tiff.get(), TIFFTAG_PAGENUMBER, metadata->page_number);
        get_field(tiff.get(), TIFFTAG_COMPRESSION, metadata->compression);
        // get_field(tiff.get(), TIFFTAG_PHOTOMETRIC, metadata->photometric_interpretation);
        get_field(tiff.get(), TIFFTAG_FILLORDER, metadata->fill_order);
        get_field(tiff.get(), TIFFTAG_ORIENTATION, metadata->orientation);
        get_field(tiff.get(), TIFFTAG_PLANARCONFIG, metadata->planar_configuration);
        get_field(tiff.get(), TIFFTAG_XRESOLUTION, metadata->x_resolution);
        get_field(tiff.get(), TIFFTAG_YRESOLUTION, metadata->y_resolution);
        get_field(tiff.get(), TIFFTAG_RESOLUTIONUNIT, metadata->resolution_unit);
        get_field(tiff.get(), TIFFTAG_SOFTWARE, metadata->software);
        get_field(tiff.get(), TIFFTAG_DATETIME, metadata->date_time);
        get_field(tiff.get(), TIFFTAG_ARTIST, metadata->artist);
        get_field(tiff.get(), TIFFTAG_HOSTCOMPUTER, metadata->host_computer);
        get_field(tiff.get(), TIFFTAG_PREDICTOR, metadata->predictor);

        metadata->tiles = get_tiles(tiff.get(), metadata->width, metadata->height);

        get_strips(tiff.get(), metadata->height);

        metadata->geo_tiff = read_geotiff_metadata(tiff.get());

        Image image;
        if (metadata->bits_per_sample <= 16)
        {
            image = Image(PixelType::RGBA_8, metadata->width, metadata->height);
            if (!image)
                return {};

            if (TIFFReadRGBAImage(tiff.get(),
                                  metadata->width, metadata->height,
                                  reinterpret_cast<uint32_t*>(image.data()),
                                  0) == 0)
            {
                return {};
            }
        }
        else if (metadata->bits_per_sample == 32
                 && metadata->samples_per_pixel == 1
                 && metadata->sample_format == SAMPLEFORMAT_IEEEFP
                 && metadata->planar_configuration == PLANARCONFIG_CONTIG)
        {
            image = Image(PixelType::MONO_FLOAT_32, metadata->width, metadata->height);
            if (metadata->tiles)
            {
                Image tile_image(PixelType::MONO_FLOAT_32, metadata->tiles->width,
                                 metadata->tiles->height);
                for (auto& tile : metadata->tiles->tiles)
                {
                    if (TIFFReadTile(tiff.get(),
                                     tile_image.data(),
                                     tile.x * metadata->tiles->width,
                                     tile.y * metadata->tiles->height,
                                     0, 0) != -1)
                    {
                        paste(tile_image.view(),
                              image.mutable_subimage(tile.x * metadata->tiles->width,
                                                     tile.y * metadata->tiles->height,
                                                     metadata->tiles->width,
                                                     metadata->tiles->height));
                    }
                }
            }
        }

        metadata->path = path;
        metadata->format = ImageFormat::TIFF;
        image.set_metadata(std::move(metadata));
        return image;
    }
}
