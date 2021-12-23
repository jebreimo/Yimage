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

namespace yimage
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

    void write_png(std::ostream& stream, const ImageView& img)
    {
        auto png_info = PngInfo().width(img.width()).height(img.height());

        switch (img.pixel_type())
        {
        case PixelType::MONO8:
            png_info.bit_depth(8).bit_depth(PNG_COLOR_TYPE_GRAY);
            break;
        case PixelType::ALPHA_MONO8:
            YIMAGE_THROW("PNG doesn't support ALPHA_MONO8."
                         " Convert the image to MONO_ALPHA8 first.");
        case PixelType::MONO_ALPHA8:
            png_info.bit_depth(8).bit_depth(PNG_COLOR_TYPE_GRAY_ALPHA);
            break;
        case PixelType::RGB24:
            png_info.bit_depth(8).bit_depth(PNG_COLOR_TYPE_RGB);
            break;
        case PixelType::ARGB32:
            YIMAGE_THROW("PNG doesn't support ARGB32."
                         " Convert the image to RGBA32 first.");
        case PixelType::RGBA32:
            png_info.bit_depth(8).bit_depth(PNG_COLOR_TYPE_RGBA);
            break;
        case PixelType::NONE:
            break;
        }
        write_png(stream, img.data(), img.size(), png_info, PngTransform());
    }

    void write_png(const std::string& fileName, const ImageView& img)
    {
        std::ofstream stream(fileName);
        if (!stream)
            YIMAGE_THROW("Can not create " + fileName);
        write_png(stream, img);
    }
}
