//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>

namespace Yimage
{
    enum class ImageFormat
    {
        UNKNOWN,
        JPEG,
        PNG,
    };

    class ImageMetadata
    {
    public:
        explicit ImageMetadata(ImageFormat format = ImageFormat::UNKNOWN);

        [[nodiscard]] std::string path() const;

        void set_path(const std::string& path);

        [[nodiscard]] ImageFormat format() const;

        void set_format(ImageFormat format);

    private:
        std::string path_;
        ImageFormat format_ = ImageFormat::UNKNOWN;
    };
}
