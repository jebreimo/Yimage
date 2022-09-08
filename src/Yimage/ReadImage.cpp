//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadImage.hpp"

#include <fstream>
#include "Yimage/ReadJpeg.hpp"
#include "Yimage/ReadPng.hpp"
#include "Yimage/YimageException.hpp"

namespace yimage
{
    constexpr char PNG_SIGNATURE[4] = {'\x89', 'P', 'N', 'G'};
    constexpr char JPEG_SIGNATURE[3] = {'\xFF', '\xD8', '\xFF'};
    constexpr char JPEG_JFIF_SIGNATURE[4] = {'J', 'F', 'I', 'F'};
    constexpr char JPEG_EXIF_SIGNATURE[4] = {'E', 'x', 'i', 'f'};

    Image read_image(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        char buffer[16];
        file.read(buffer, 16);
        if (size_t(file.gcount()) != 16)
            YIMAGE_THROW("Unrecognized image format.");

        file.seekg(0, std::ios::beg);
        if (std::equal(buffer, buffer + 4, PNG_SIGNATURE))
        {
            return read_png(file);
        }
        else if (std::equal(buffer, buffer + 3, JPEG_SIGNATURE)
                 && (std::equal(buffer + 6, buffer + 10, JPEG_JFIF_SIGNATURE)
                     || std::equal(buffer + 6, buffer + 10, JPEG_EXIF_SIGNATURE)))
        {
            file.close();
            return read_jpeg(path);
        }

        YIMAGE_THROW("Unrecognized image format.");
    }
}
