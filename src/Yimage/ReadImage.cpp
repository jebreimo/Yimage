//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-12.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadImage.hpp"

#include <algorithm>
#include <fstream>
#include "Yimage/YimageException.hpp"
#include "YimageVersion.hpp"

#ifdef YIMAGE_JPEG
    #include "Yimage/Jpeg/ReadJpeg.hpp"
#endif
#ifdef YIMAGE_PNG
    #include "Yimage/Png/ReadPng.hpp"
#endif
#ifdef YIMAGE_TIFF
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

    ImageFormat get_image_format(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        char buffer[16];
        file.read(buffer, 16);
        return get_image_format(buffer, size_t(file.gcount()));
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

    Image read_image(const std::filesystem::path& path)
    {
        std::ifstream stream(path, std::ios::binary);
        char buffer[16];
        stream.read(buffer, 16);

        switch (get_image_format(buffer, size_t(stream.gcount())))
        {
#ifdef YIMAGE_JPEG
        case ImageFormat::JPEG:
            stream.close();
            return read_jpeg(path);
#endif
#ifdef YIMAGE_PNG
        case ImageFormat::PNG:
            stream.seekg(0, std::ios::beg);
            return read_png(stream);
#endif
#ifdef YIMAGE_TIFF
        case ImageFormat::TIFF:
            stream.seekg(0, std::ios::beg);
            return read_tiff(stream, path);
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
#ifdef YIMAGE_JPEG
        case ImageFormat::JPEG:
            return read_jpeg(buffer, size);
#endif
#ifdef YIMAGE_PNG
        case ImageFormat::PNG:
            return read_png(buffer, size);
#endif
#ifdef YIMAGE_TIFF
        case ImageFormat::TIFF:
            return read_tiff(buffer, size);
#endif
        case ImageFormat::UNKNOWN:
        default:
            YIMAGE_THROW("Unrecognized image format.");
        }
    }
}
