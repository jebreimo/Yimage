//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Rgba8.hpp"

#include <cstdio>
#include <ostream>
#include <string>

namespace Yimage
{
    bool operator==(const Rgba8& a, const Rgba8& b)
    {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    }

    bool operator!=(const Rgba8& a, const Rgba8& b)
    {
        return !(a == b);
    }

    std::string to_string(const Rgba8& rgba)
    {
        std::string result(10, '\0');
        snprintf(result.data(), 10, "#%02X%02X%02X%02X",
                 rgba.r, rgba.g, rgba.b, rgba.a);
        result.resize(9);
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const Rgba8& rgba)
    {
        return os << to_string(rgba);
    }
}
