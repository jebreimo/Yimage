//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Tiff/ReadTiff.hpp"

#include <fstream>
#include "Yimage/ImageAlgorithms.hpp"
#include "Yimage/Tiff/TiffMetadata.hpp"
#include "Yimage/YimageException.hpp"
#include "../FileUtilities.hpp"
#include "../ReadOnlyStreamBuffer.hpp"
#include "OpenTiff.hpp"
#include "ReadGeoTiffMetadata.hpp"

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

        bool get_field(TIFF* tiff, int tag, std::array<uint16_t, 2>& value)
        {
            if (TIFFGetField(tiff, tag, &value[0], &value[1]) == 0)
                return false;
            return true;
        }

        std::optional<TileInfo> get_tile_info(TIFF* tiff, uint32_t width, uint32_t height)
        {
            uint64_t* byte_counts = nullptr;
            uint64_t* offsets = nullptr;

            TileInfo result;
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

        std::optional<StripInfo> get_strip_info(TIFF* tiff, uint32_t height)
        {
            StripInfo strips;
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

        Image read_float32_tiles(TIFF* tiff, const TiffMetadata& metadata)
        {
            Image image(PixelType::MONO_FLOAT_32, metadata.width, metadata.height);
            Image tile_image(PixelType::MONO_FLOAT_32, metadata.tiles->width,
                             metadata.tiles->height);
            for (auto& tile : metadata.tiles->tiles)
            {
                if (TIFFReadTile(tiff,
                                 tile_image.data(),
                                 tile.x * metadata.tiles->width,
                                 tile.y * metadata.tiles->height,
                                 0, 0) != -1)
                {
                    paste(tile_image.view(),
                          image.mutable_subimage(tile.x * metadata.tiles->width,
                                                 tile.y * metadata.tiles->height,
                                                 metadata.tiles->width,
                                                 metadata.tiles->height));
                }
            }
            return image;
        }

        std::unique_ptr<TiffMetadata> get_metadata(TIFF* tiff)
        {
            auto geotiff = read_geotiff_metadata(tiff);
            auto metadata = geotiff
                                ? std::move(geotiff)
                                : std::make_unique<TiffMetadata>();
            get_field(tiff, TIFFTAG_IMAGEWIDTH, metadata->width);
            get_field(tiff, TIFFTAG_IMAGELENGTH, metadata->height);
            get_field(tiff, TIFFTAG_BITSPERSAMPLE, metadata->bits_per_sample);
            get_field(tiff, TIFFTAG_SAMPLESPERPIXEL, metadata->samples_per_pixel);
            get_field(tiff, TIFFTAG_SAMPLEFORMAT, metadata->sample_format);
            get_field(tiff, TIFFTAG_MINSAMPLEVALUE, metadata->min_sample_value);
            get_field(tiff, TIFFTAG_MAXSAMPLEVALUE, metadata->max_sample_value);
            get_field(tiff, TIFFTAG_XPOSITION, metadata->x_position);
            get_field(tiff, TIFFTAG_YPOSITION, metadata->y_position);
            get_field(tiff, TIFFTAG_PAGENUMBER, metadata->page_number);
            get_field(tiff, TIFFTAG_COMPRESSION, metadata->compression);
            // get_field(tiff, TIFFTAG_PHOTOMETRIC, metadata->photometric_interpretation);
            get_field(tiff, TIFFTAG_FILLORDER, metadata->fill_order);
            get_field(tiff, TIFFTAG_ORIENTATION, metadata->orientation);
            get_field(tiff, TIFFTAG_PLANARCONFIG, metadata->planar_configuration);
            get_field(tiff, TIFFTAG_XRESOLUTION, metadata->x_resolution);
            get_field(tiff, TIFFTAG_YRESOLUTION, metadata->y_resolution);
            get_field(tiff, TIFFTAG_RESOLUTIONUNIT, metadata->resolution_unit);
            get_field(tiff, TIFFTAG_SOFTWARE, metadata->software);
            get_field(tiff, TIFFTAG_DATETIME, metadata->date_time);
            get_field(tiff, TIFFTAG_ARTIST, metadata->artist);
            get_field(tiff, TIFFTAG_HOSTCOMPUTER, metadata->host_computer);
            get_field(tiff, TIFFTAG_PREDICTOR, metadata->predictor);

            metadata->tiles = get_tile_info(tiff, metadata->width, metadata->height);
            metadata->strips = get_strip_info(tiff, metadata->height);

            return metadata;
        }
    }

    Image read_tiff(std::istream& stream, const std::string& name)
    {
        auto tiff = open_tiff(stream, name.c_str());
        if (!tiff)
            return {};

        auto metadata = get_metadata(tiff.get());

        Image image;
        if (metadata->bits_per_sample <= 16)
        {
            image = Image(PixelType::RGBA_8, metadata->width, metadata->height);
            if (!image)
                return {};

            if (!TIFFReadRGBAImage(tiff.get(),
                                   metadata->width, metadata->height,
                                   reinterpret_cast<uint32_t*>(image.data()),
                                   0))
            {
                return {};
            }
        }
        else if (metadata->bits_per_sample == 32
                 && metadata->samples_per_pixel == 1
                 && metadata->sample_format == SAMPLEFORMAT_IEEEFP
                 && metadata->planar_configuration == PLANARCONFIG_CONTIG)
        {
            if (metadata->tiles)
            {
                image = read_float32_tiles(tiff.get(), *metadata);
            }
        }

        metadata->path = name;

        image.set_metadata(std::move(metadata));
        return image;
    }

    Image read_tiff(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            YIMAGE_THROW("Could not open file: " + path.string());
        auto img = read_tiff(file, path);
        if (auto metadata = img.metadata())
            metadata->path = path;
        return img;
    }

    Image read_tiff(const void* buffer, size_t size)
    {
        ReadOnlyStreamBuffer stream_buffer(static_cast<const char*>(buffer), size);
        std::istream stream(&stream_buffer);
        return read_tiff(stream);
    }

    std::unique_ptr<TiffMetadata> read_tiff_metadata(const std::filesystem::path& path)
    {
        auto tiff = open_tiff(path);
        if (!tiff)
            return {};

        auto metadata = get_metadata(tiff.get());
        metadata->path = path;
        return metadata;
    }
}
