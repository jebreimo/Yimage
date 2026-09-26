//****************************************************************************
// Copyright © 2026 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2026-09-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/ReadImage.hpp>
#include <Yimage/Png/WritePng.hpp>
#include <Yson/JsonWriter.hpp>

#include "Yimage/Tiff/GeoTiffMetadata.hpp"
#include "Yimage/Tiff/TiffMetadata.hpp"

namespace
{
    argos::ParsedArguments parse_arguments(int argc, char* argv[])
    {
        using namespace argos;
        return ArgumentParser()
            .add(Argument("FILE").help("An image file."))
            .parse(argc, argv);
    }

    template <typename T, size_t N>
    void write(Yson::Writer& writer, const std::array<T, N>& values, size_t values_per_line = 0)
    {
        writer.beginArray(Yson::JsonParameters(values_per_line));
        for (const auto& value : values)
            writer.value(value);
        writer.endArray();
    }

    void write_properties(Yson::Writer& writer, const Yimage::ImageMetadata& metadata)
    {
        writer.key("path").value(metadata.path.string());
        writer.key("format").value(to_string(metadata.format));
    }

    void write_properties(Yson::Writer& writer, const Yimage::PngMetadata& metadata)
    {
        writer.key("width").value(metadata.width);
        writer.key("height").value(metadata.height);
        writer.key("bit_depth").value(metadata.bit_depth);
        writer.key("color_type").value(metadata.color_type);
        writer.key("interlace_type").value(metadata.interlace_type);
        writer.key("compression_method").value(metadata.compression_method);
        writer.key("filter_method").value(metadata.filter_method);
        if (metadata.gamma)
            writer.key("gamma").value(*metadata.gamma);
        if (metadata.pixels_per_meter)
            writer.key("pixels_per_meter")
                .beginArray()
                .value(metadata.pixels_per_meter->first)
                .value(metadata.pixels_per_meter->second)
                .endArray();
    }

    void write_properties(Yson::Writer& writer, const Yimage::TiffMetadata& metadata)
    {
        writer.key("width").value(metadata.width);
        writer.key("height").value(metadata.height);
        writer.key("bits_per_sample").value(metadata.bits_per_sample);
        writer.key("samples_per_pixel").value(metadata.samples_per_pixel);
        writer.key("compression").value(metadata.compression);
        writer.key("planar_configuration").value(metadata.planar_configuration);
        writer.key("orientation").value(metadata.orientation);
        writer.key("fill_order").value(metadata.fill_order);
        writer.key("resolution_unit").value(metadata.resolution_unit);
        writer.key("predictor").value(metadata.predictor);
        writer.key("sample_format").value(metadata.sample_format);
        write(writer, metadata.page_number, 10);
        writer.key("x_resolution").value(metadata.x_resolution);
        writer.key("y_resolution").value(metadata.y_resolution);
        writer.key("x_position").value(metadata.x_position);
        writer.key("y_position").value(metadata.y_position);
        writer.key("software").value(metadata.software);
        writer.key("date_time").value(metadata.date_time);
        writer.key("artist").value(metadata.artist);
        writer.key("host_computer").value(metadata.host_computer);

        if (metadata.strips)
        {
            writer.key("strips").beginObject();
            writer.key("rows_per_strip").value(metadata.strips->rows_per_strip);
            writer.key("strips").beginArray();
            for (const auto& [byte_count, offset] : metadata.strips->strips)
            {
                writer.beginObject();
                writer.key("byte_count").value(byte_count);
                writer.key("offset").value(offset);
                writer.endObject();
            }
            writer.endArray();
            writer.endObject();
        }

        if (const auto& tiles = metadata.tiles)
        {
            writer.key("tiles").beginObject();
            writer.key("width").value(tiles->width);
            writer.key("height").value(tiles->height);
            writer.key("tiles").beginArray();
            for (const auto& [x, y, byte_count, offset] : tiles->tiles)
            {
                writer.beginObject();
                writer.key("x").value(x);
                writer.key("y").value(y);
                writer.key("byte_count").value(byte_count);
                writer.key("offset").value(offset);
                writer.endObject();
            }
            writer.endArray();
            writer.endObject();
        }

        writer.key("min_sample_value").value(metadata.min_sample_value);
        writer.key("max_sample_value").value(metadata.max_sample_value);
    }

    void write_properties(Yson::Writer& writer, const Yimage::GeoTiffMetadata& metadata)
    {
        writer.key("model_pixel_scale");
        write(writer, metadata.model_pixel_scale, 3);
        writer.key("model_tie_point");
        write(writer, metadata.model_tie_point, 3);
        writer.key("model_transformation");
        write(writer, metadata.model_transformation, 4);

        writer.key("citation").value(metadata.citation);
        writer.key("geog_citation").value(metadata.geog_citation);
        writer.key("projected_citation").value(metadata.projected_citation);
        writer.key("gdal_metadata").value(metadata.gdal_metadata);
        writer.key("gdal_no_data").value(metadata.gdal_no_data);

        writer.key("angular_unit_size").value(metadata.angular_unit_size);
        writer.key("geog_semi_major_axis").value(metadata.geog_semi_major_axis);
        writer.key("geog_inv_flattening").value(metadata.geog_inv_flattening);
        writer.key("geog_prime_meridian_longitude").value(metadata.geog_prime_meridian_longitude);

        writer.key("key_directory_version").value(metadata.key_directory_version);
        writer.key("key_revision").value(metadata.key_revision);
        writer.key("minor_revision").value(metadata.minor_revision);
        writer.key("model_type").value(metadata.model_type);
        writer.key("raster_type").value(metadata.raster_type);
        writer.key("geodetic_crs").value(metadata.geodetic_crs);
        writer.key("geodetic_datum").value(metadata.geodetic_datum);
        writer.key("prime_meridian").value(metadata.prime_meridian);
        writer.key("projected_crs").value(metadata.projected_crs);
        writer.key("vertical_crs").value(metadata.vertical_crs);
        writer.key("geog_azimuth_units").value(metadata.geog_azimuth_units);
        writer.key("projection_zone").value(metadata.projection_zone);
        writer.key("projected_linear_units").value(metadata.projected_linear_units);
        writer.key("ellipsoid").value(metadata.ellipsoid);
        writer.key("linear_units").value(metadata.linear_units);
        writer.key("vertical_units").value(metadata.vertical_units);
        writer.key("angular_units").value(metadata.angular_units);
        writer.key("coordinate_system").value(metadata.coordinate_system);
    }

    void write(Yson::Writer& writer, const Yimage::ImageMetadata& metadata)
    {
        writer.beginObject();
        write_properties(writer, metadata);
        if (const auto& png = dynamic_cast<const Yimage::PngMetadata*>(&metadata))
            write_properties(writer, *png);
        if (const auto& tiff = dynamic_cast<const Yimage::TiffMetadata*>(&metadata))
            write_properties(writer, *tiff);
        if (const auto& geo_tiff = dynamic_cast<const Yimage::GeoTiffMetadata*>(&metadata))
            write_properties(writer, *geo_tiff);
        writer.endObject();
    }

    void write(Yson::Writer& writer, const std::vector<Yimage::Rgba8>& colors)
    {
        writer.beginArray(Yson::JsonParameters(4));
        for (const auto& color : colors)
        {
            writer.value(to_string(color));
        }
        writer.endArray();
    }

    void write(Yson::Writer& writer, const Yimage::Image& image)
    {
        writer.beginObject();
        writer.key("width").value(image.width());
        writer.key("height").value(image.height());
        writer.key("row_gap_size").value(image.row_gap_size());
        writer.key("pixel_type").value(to_string(image.pixel_type()));
        if (!image.palette().empty())
        {
            writer.key("palette");
            write(writer, image.palette());
        }
        if (image.metadata())
        {
            writer.key("metadata");
            write(writer, *image.metadata());
        }
        writer.endObject();
    }
}
int main(int argc, char* argv[])
{
    const auto args = parse_arguments(argc, argv);

    try
    {
        Yimage::Image image = Yimage::read_image(args.value("FILE").as_string());
        Yson::JsonWriter writer(std::cout);
        write(writer, image);
        std::cout << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
