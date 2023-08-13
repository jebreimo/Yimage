//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Yimage/ReadImage.hpp"
#include "Resources.hpp"

TEST_CASE("Open PNG")
{
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE);
    REQUIRE(bool(image));
    REQUIRE(Yimage::get_rgba8(image, 0, 0) == Yimage::Rgba8(0xFFFFFF00));
    REQUIRE(Yimage::get_rgba8(image, 7, 27) == Yimage::Rgba8(0xC99132F1));
}

TEST_CASE("Open JPEG")
{
    auto image = Yimage::read_image(CITY_JPG, CITY_JPG_SIZE);
    REQUIRE(bool(image));
    REQUIRE(Yimage::get_rgba8(image, 0, 0) == Yimage::Rgba8(0x363636FF));
    REQUIRE(Yimage::get_rgba8(image, 64, 55) == Yimage::Rgba8(0x484848FF));
}
