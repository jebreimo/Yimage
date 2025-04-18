//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdio>
#include <filesystem>
#include "../Image.hpp"

namespace Yimage
{
    [[nodiscard]] Image read_jpeg(const std::filesystem::path& path);

    [[nodiscard]] Image read_jpeg(FILE* file);

    [[nodiscard]] Image read_jpeg(const void* buffer, size_t size);
}
