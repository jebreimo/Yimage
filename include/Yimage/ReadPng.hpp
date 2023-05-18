//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "Image.hpp"

namespace Yimage
{
    Image read_png(std::istream& stream);

    Image read_png(const std::string& path);
}
