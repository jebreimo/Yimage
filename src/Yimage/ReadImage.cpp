//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadImage.hpp"

#include <algorithm>
#include <fstream>
#include "Yimage/Jpeg/ReadJpeg.hpp"
#include "Yimage/Png/ReadPng.hpp"
#include "Yimage/YimageException.hpp"
#include "YimageVersion.hpp"

#ifdef YIMAGE_USE_LIBTIFF
    #include "Yimage/Tiff/ReadTiff.hpp"
#endif

namespace Yimage
{
    namespace
    {
        constexpr char PNG_SIGNATURE[4] = {'\x89', 'P', 'N', 'G'};
        constexpr char JPEG_SIGNATURE[3] = {'\xFF', '\xD8', '\xFF'};
        constexpr char JPEG_JFIF_SIGNATURE[4] = {'J', 'F', 'I', 'F'};
        constexpr char JPEG_EXIF_SIGNATURE[4] = {'E', 'x', 'i', 'f'};
        constexpr char TIFF_SIGNATURE_LE[4] = {'I', 'I', 0x2A, 0x00};
        constexpr char TIFF_SIGNATURE_BE[4] = {'M', 'M', 0x00, 0x2A};
    }

    ImageFormat get_image_format(const void* buffer, size_t size)
    {
        auto bytes = static_cast<const char*>(buffer);
        if (size >= 4 && std::equal(bytes, bytes + 4, PNG_SIGNATURE))
            return ImageFormat::PNG;

        if (size >= 10
            && std::equal(bytes, bytes + 3, JPEG_SIGNATURE)
            && (std::equal(bytes + 6, bytes + 10, JPEG_JFIF_SIGNATURE)
                || std::equal(bytes + 6, bytes + 10, JPEG_EXIF_SIGNATURE)))
        {
            return ImageFormat::JPEG;
        }

        if (size >= 4
            && (std::equal(bytes, bytes + 4, TIFF_SIGNATURE_LE)
                || std::equal(bytes, bytes + 4, TIFF_SIGNATURE_BE)))
        {
            return ImageFormat::TIFF;
        }

        return ImageFormat::UNKNOWN;
    }

    Image read_image(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        char buffer[16];
        file.read(buffer, 16);

        switch (get_image_format(buffer, size_t(file.gcount())))
        {
        case ImageFormat::JPEG:
            file.close();
            return read_jpeg(path);
        case ImageFormat::PNG:
            file.seekg(0, std::ios::beg);
            return read_png(file);
        case ImageFormat::TIFF:
#ifdef YIMAGE_USE_LIBTIFF
            file.seekg(0, std::ios::beg);
            return read_tiff(file, path);
#endif
        case ImageFormat::UNKNOWN:
        default:
            YIMAGE_THROW("Unrecognized image format.");
        }
    }

    Image read_image(const void* buffer, size_t size)
    {
        switch (get_image_format(buffer, size))
        {
        case ImageFormat::JPEG:
            return read_jpeg(buffer, size);
        case ImageFormat::PNG:
            return read_png(buffer, size);
        case ImageFormat::TIFF:
#ifdef YIMAGE_USE_LIBTIFF
            return read_tiff(buffer, size);
#endif
        case ImageFormat::UNKNOWN:
        default:
            YIMAGE_THROW("Unrecognized image format.");
        }
    }
}
