//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include <iosfwd>
#include <memory>
#include <tiffio.h>

namespace Yimage
{
    struct TiffDeleter
    {
        void operator()(TIFF* tiff) const;
    };

    std::unique_ptr<TIFF, TiffDeleter>
    open_tiff(std::istream& is, const char* stream_name);

    std::unique_ptr<TIFF, TiffDeleter>
    open_tiff(const std::filesystem::path& path);
}
