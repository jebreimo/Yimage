//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdio>
#include <string>
#include "Image.hpp"

namespace Yimage
{
    Image read_jpeg(const std::string& path);

    Image read_jpeg(FILE* file);

    Image read_jpeg(const void* buffer, size_t size);
}
