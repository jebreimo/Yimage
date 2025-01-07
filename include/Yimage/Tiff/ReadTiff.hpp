//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include "../Image.hpp"
#include "TiffMetadata.hpp"

namespace Yimage
{
    Image read_tiff(std::istream& stream,
                    const std::string& name = "TiffStream");

    Image read_tiff(const std::string& path);

    Image read_tiff(const void* buffer, size_t size);

    std::unique_ptr<TiffMetadata> read_tiff_metadata(const std::string& path);
}
