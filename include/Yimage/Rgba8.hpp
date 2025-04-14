//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <iosfwd>
#include <string>

namespace Yimage
{
    struct Rgba8
    {
        constexpr Rgba8() = default;

        explicit constexpr Rgba8(uint32_t rgba)
            : r(uint8_t(rgba >> 24)),
              g(uint8_t((rgba >> 16) & 0xFF)),
              b(uint8_t((rgba >> 8) & 0xFF)),
              a(uint8_t(rgba & 0xFF))
        {}

        constexpr Rgba8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
            : r(r),
              g(g),
              b(b),
              a(a)
        {}

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0xFF;
    };

    bool operator==(const Rgba8& a, const Rgba8& b);

    bool operator!=(const Rgba8& a, const Rgba8& b);

    std::string to_string(const Rgba8& rgba);

    std::ostream& operator<<(std::ostream& os, const Rgba8& rgba);

    namespace Color
    {
        constexpr Rgba8 Black;
        constexpr Rgba8 White{0xFF, 0xFF, 0xFF};
        constexpr Rgba8 Red{0xFF, 0, 0};
        constexpr Rgba8 Green{0, 0xFF, 0};
        constexpr Rgba8 Blue{0, 0, 0xFF};
        constexpr Rgba8 Yellow{0xFF, 0xFF, 0};
        constexpr Rgba8 Cyan{0, 0xFF, 0xFF};
        constexpr Rgba8 Magenta{0xFF, 0, 0xFF};
        constexpr Rgba8 Transparent{0, 0, 0, 0};
    }
}
