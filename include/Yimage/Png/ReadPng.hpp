//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include "../Image.hpp"

namespace Yimage
{
    [[nodiscard]] Image read_png(std::istream& stream);

    [[nodiscard]] Image read_png(const std::filesystem::path& path);

    [[nodiscard]] Image read_png(const void* buffer, size_t size);
}
