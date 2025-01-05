//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "Image.hpp"

namespace Yimage
{
    /**
     * @brief Determines the image format from the file signature.
     * @param buffer The image or start of the image. The buffer should
     *      contain at least the first 16 bytes of the image.
     * @param size The size of buffer in bytes.
     * @return The image format or UNKNOWN if the format is unrecognized
     *      or the buffer is too small to be determined.
     */
    ImageFormat get_image_format(const void* buffer, size_t size);

    Image read_image(const std::string& path);

    Image read_image(const void* buffer, size_t size);
}
