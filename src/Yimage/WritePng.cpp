//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-11.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/WritePng.hpp"

#include <fstream>
#include "Yimage/PngWriter.hpp"
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    void write_png(std::ostream& stream,
                   const void* image, size_t image_size,
                   PngInfo options, PngTransform transform)
    {
        PngWriter writer(stream, std::move(options), transform);
        writer.write_info();
        writer.write(image, image_size);
        writer.write_end();
    }

    void write_png(const std::string& fileName,
                   const void* image, size_t image_size,
                   PngInfo options, PngTransform transform)
    {
        std::ofstream stream(fileName);
        if (!stream)
            YIMAGE_THROW("Can not create " + fileName);
        write_png(stream, image, image_size, std::move(options), transform);
    }

    void write_png(std::ostream& stream, const IImage& img)
    {
        auto png_info = PngInfo().width(img.width()).height(img.height());
        PngTransform transform;

        switch (img.pixel_type())
        {
        case PixelType::MONO_1:
            png_info.bit_depth(1).color_type(PNG_COLOR_TYPE_GRAY);
            break;
        case PixelType::MONO_2:
            png_info.bit_depth(2).color_type(PNG_COLOR_TYPE_GRAY);
            break;
        case PixelType::MONO_4:
            png_info.bit_depth(4).color_type(PNG_COLOR_TYPE_GRAY);
            break;
        case PixelType::MONO_8:
            png_info.bit_depth(8).color_type(PNG_COLOR_TYPE_GRAY);
            break;
        case PixelType::ALPHA_MONO_8:
            transform.invert_alpha(true);
            [[fallthrough]];
        case PixelType::MONO_ALPHA_8:
            png_info.bit_depth(8).color_type(PNG_COLOR_TYPE_GRAY_ALPHA);
            break;
        case PixelType::ALPHA_MONO_16:
            transform.invert_alpha(true);
            [[fallthrough]];
        case PixelType::MONO_ALPHA_16:
            png_info.bit_depth(16).color_type(PNG_COLOR_TYPE_GRAY_ALPHA);
            break;
        case PixelType::RGB_8:
            png_info.bit_depth(8).color_type(PNG_COLOR_TYPE_RGB);
            break;
        case PixelType::RGB_16:
            png_info.bit_depth(16).color_type(PNG_COLOR_TYPE_RGB);
            break;
        case PixelType::ARGB_8:
            transform.invert_alpha(true);
            [[fallthrough]];
        case PixelType::RGBA_8:
            png_info.bit_depth(8).color_type(PNG_COLOR_TYPE_RGBA);
            break;
        case PixelType::ARGB_16:
            transform.invert_alpha(true);
            [[fallthrough]];
        case PixelType::RGBA_16:
            png_info.bit_depth(16).color_type(PNG_COLOR_TYPE_RGBA);
            break;
        default:
            YIMAGE_THROW("Unsupported pixel type: "
                         + std::to_string(int(img.pixel_type())));
        }
        write_png(stream, img.data(), img.size(), png_info, transform);
    }

    void write_png(const std::string& fileName, const IImage& img)
    {
        std::ofstream stream(fileName);
        if (!stream)
            YIMAGE_THROW("Can not create " + fileName);
        write_png(stream, img);
    }
}
