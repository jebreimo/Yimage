//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Image.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("test set_rgba8")
{
    using namespace Yimage;
    std::vector<uint8_t> buffer1{
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    };
    SECTION("rgba8 image")
    {
        MutableImageView img(buffer1.data(), PixelType::RGBA_8, 2, 2);
        set_rgba8(img, 1, 0, {0x33, 0x77, 0xCC, 0xFF});
        set_rgba8(img, 0, 1, {0x44, 0x88, 0xDD, 0xEE});
        REQUIRE(buffer1[4] == 0x33);
        REQUIRE(buffer1[7] == 0xFF);
        REQUIRE(buffer1[9] == 0x88);
        REQUIRE(buffer1[11] == 0xEE);
    }
    SECTION("rgb8 image")
    {
        MutableImageView img(buffer1.data(), PixelType::RGB_8, 2, 2, 2);
        set_rgba8(img, 1, 0, {0x33, 0x77, 0xCC, 0xFF});
        set_rgba8(img, 0, 1, {0x44, 0x88, 0xDD, 0xEE});
        REQUIRE(buffer1[3] == 0x33);
        REQUIRE(buffer1[5] == 0xCC);
        REQUIRE(buffer1[6] == 6);
        REQUIRE(buffer1[7] == 7);
        REQUIRE(buffer1[9] == 0x88);
        REQUIRE(buffer1[10] == 0xDD);
        REQUIRE(buffer1[11] == 11);
    }
}
