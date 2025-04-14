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
    /**
     * @brief Reads a TIFF image from a stream.
     *
     * @param stream The stream to read the image from.
     * @param name The name of the stream. Used for error messages.
     * @return The image read from the stream.
     */
    [[nodiscard]] Image
    read_tiff(std::istream& stream, const std::string& name = "TIFF stream");

    [[nodiscard]] Image read_tiff(const std::filesystem::path& path);

    [[nodiscard]] Image read_tiff(const void* buffer, size_t size);

    [[nodiscard]] std::unique_ptr<TiffMetadata>
    read_tiff_metadata(const std::filesystem::path& path);
}
