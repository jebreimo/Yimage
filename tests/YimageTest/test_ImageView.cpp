//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-04-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Yimage/Image.hpp"

using Yimage::ImageView;
using Yimage::PixelType;
using Yimage::Rgba8;

TEST_CASE("test get_rgba8")
{
    std::vector<uint8_t> buffer{
        0x00, 0x10, 0x20, 0x30,
        0x40, 0x50, 0x60, 0x70,
        0x80, 0x90, 0xA0, 0xB0,
        0xC0, 0xD0, 0xE0, 0xF0,
        0x08, 0x18, 0x28, 0x38,
        0x48, 0x58, 0x68, 0x78,
        0x88, 0x98, 0xA8, 0xB8,
        0xC8, 0xD8, 0xE8, 0xF8
    };

    SECTION("RGBA_8")
    {
        ImageView img(buffer.data(), PixelType::RGBA_8, 4, 2);
        REQUIRE(get_rgba8(img, 2, 1) == Rgba8{0x88, 0x98, 0xA8, 0xB8});
    }
    SECTION("RGBA_8")
    {
        ImageView img(buffer.data(), PixelType::RGB_8, 5, 2, 1);
        REQUIRE(get_rgba8(img, 2, 1) == Rgba8{0x68, 0x78, 0x88, 0xFF});
    }
    SECTION("MONO_4")
    {
        ImageView img(buffer.data(), PixelType::MONO_4, 8, 8);
        REQUIRE(get_rgba8(img, 4, 6) == Rgba8{0xAA, 0xAA, 0xAA, 0xFF});
        REQUIRE(get_rgba8(img, 5, 6) == Rgba8{0x88, 0x88, 0x88, 0xFF});
    }
    SECTION("MONO_2")
    {
        ImageView img(buffer.data(), PixelType::MONO_2, 16, 8);
        REQUIRE(get_rgba8(img, 4, 6) == Rgba8{0xAA, 0xAA, 0xAA, 0xFF});
        REQUIRE(get_rgba8(img, 5, 6) == Rgba8{0x55, 0x55, 0x55, 0xFF});
        REQUIRE(get_rgba8(img, 6, 6) == Rgba8{0xAA, 0xAA, 0xAA, 0xFF});
        REQUIRE(get_rgba8(img, 7, 6) == Rgba8{0x00, 0x00, 0x00, 0xFF});
    }
    SECTION("MONO_1")
    {
        ImageView img(buffer.data(), PixelType::MONO_1, 32, 8);
        REQUIRE(get_rgba8(img, 8, 6) == Rgba8{0xFF, 0xFF, 0xFF, 0xFF});
        REQUIRE(get_rgba8(img, 9, 6) == Rgba8{0x00, 0x00, 0x00, 0xFF});
        REQUIRE(get_rgba8(img, 10, 6) == Rgba8{0x00, 0x00, 0x00, 0xFF});
        REQUIRE(get_rgba8(img, 11, 6) == Rgba8{0xFF, 0xFF, 0xFF, 0xFF});
    }
}
