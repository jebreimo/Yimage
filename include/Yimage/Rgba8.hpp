//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <iosfwd>

namespace Yimage
{
    struct Rgba8
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    bool operator==(const Rgba8& a, const Rgba8& b);

    bool operator!=(const Rgba8& a, const Rgba8& b);

    std::string to_string(const Rgba8& rgba);

    std::ostream& operator<<(std::ostream& os, const Rgba8& rgba);
}
