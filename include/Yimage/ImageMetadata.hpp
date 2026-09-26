//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>

namespace Yimage
{
    enum class ImageFormat
    {
        UNKNOWN,
        JPEG,
        PNG,
        TIFF
    };

    std::string to_string(ImageFormat format);

    class ImageMetadata
    {
    public:
        explicit ImageMetadata(ImageFormat format = ImageFormat::UNKNOWN);

        virtual ~ImageMetadata();

        [[nodiscard]] virtual ImageMetadata* clone() const;

        std::filesystem::path path;
        ImageFormat format = ImageFormat::UNKNOWN;
    };
}
