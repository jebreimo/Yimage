//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadImage.hpp"
#include <catch2/catch_test_macros.hpp>
#include "Resources.hpp"

TEST_CASE("read_image: Read PNG RGBA")
{
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE);
    REQUIRE(bool(image));
    REQUIRE(Yimage::get_rgba8(image.view(), 0, 0) == Yimage::Rgba8(0xFFFFFF00));
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99132F1));
}

TEST_CASE("read_image: Read PNG indexed")
{
    auto image = Yimage::read_image(THUMB_UP_PALETTE_PNG, THUMB_UP_PALETTE_PNG_SIZE);
    REQUIRE(bool(image));
    REQUIRE(Yimage::get_rgba8(image.view(), 0, 0) == Yimage::Rgba8(0x47704C00));
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99031F1));
}

TEST_CASE("read_image: Read JPEG")
{
    auto image = Yimage::read_image(CITY_JPG, CITY_JPG_SIZE);
    REQUIRE(bool(image));
    REQUIRE(Yimage::get_rgba8(image.view(), 0, 0) == Yimage::Rgba8(0x363636FF));
    REQUIRE(Yimage::get_rgba8(image.view(), 64, 55) == Yimage::Rgba8(0x484848FF));
}

TEST_CASE("read_image: Read TIFF")
{
    auto image = Yimage::read_image(GEOID_TIF, GEOID_TIF_SIZE);
    REQUIRE(bool(image));
    REQUIRE(image.pixel_type() == Yimage::PixelType::MONO_FLOAT_32);
}
