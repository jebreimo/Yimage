//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Yimage/Image.hpp"

TEST_CASE("test paste")
{
    using namespace Yimage;
    std::vector<uint8_t> buffer1{
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    };
    std::vector<uint8_t> buffer2{
        20, 21,
        22, 23
    };
    MutableImageView mut_image1(buffer1.data(), PixelType::MONO_8, 4, 4);
    ImageView image1(mut_image1);
    ImageView image2(buffer2.data(), PixelType::MONO_8, 2, 2);

    SECTION("in the center")
    {
        paste(image2, mut_image1, 1, 1);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 3,
            4, 20, 21, 7,
            8, 22, 23, 11,
            12, 13, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), PixelType::MONO_8, 4, 4));
    }
    SECTION("partially outside 1")
    {
        paste(image2, mut_image1, 3, -1);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 22,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), PixelType::MONO_8, 4, 4));
    }
    SECTION("partially outside 2")
    {
        paste(image2, mut_image1, 0, 3);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            20, 21, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), PixelType::MONO_8, 4, 4));
    }
    SECTION("completely outside")
    {
        paste(image2, mut_image1, 2, 4);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), PixelType::MONO_8, 4, 4));
    }
}

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

TEST_CASE("test fill_rgba8")
{
    using namespace Yimage;
    std::vector<uint8_t> buffer(8*8);
    SECTION("mono8 chess board")
    {
        constexpr Rgba8 B{0, 0, 0, 0xFF};
        constexpr Rgba8 W{0xFF, 0xFF, 0xFF, 0xFF};
        std::vector<Rgba8> colors{W, B, W, B, W, B, W, B,
                                  B, W, B, W, B, W, B, W};
        MutableImageView img(buffer.data(), PixelType::MONO_8, 8, 8);
        fill_rgba8(img, colors.data(), colors.size());
        constexpr uint8_t b = 0;
        constexpr uint8_t w = 255;
        std::vector<uint8_t> expected{
            w, b, w, b, w, b, w, b,
            b, w, b, w, b, w, b, w,
            w, b, w, b, w, b, w, b,
            b, w, b, w, b, w, b, w,
            w, b, w, b, w, b, w, b,
            b, w, b, w, b, w, b, w,
            w, b, w, b, w, b, w, b,
            b, w, b, w, b, w, b, w
        };
        REQUIRE(buffer == expected);
    }
    SECTION("unaligned rgb8")
    {
        MutableImageView img(buffer.data(), PixelType::RGB_8, 5, 4);
        constexpr Rgba8 R{0xFF, 0, 0, 0xFF};
        constexpr Rgba8 G{0, 0xFF, 0, 0xFF};
        constexpr Rgba8 B{0, 0, 0xFF, 0xFF};

        std::vector<Rgba8> colors{R, G, B};
        fill_rgba8(img, colors.data(), colors.size());
        constexpr uint8_t w = 255;
        std::vector<uint8_t> expected{
            w, 0, 0, 0, w, 0, 0, 0, w, w, 0, 0, 0, w, 0,
            0, 0, w, w, 0, 0, 0, w, 0, 0, 0, w, w, 0, 0,
            0, w, 0, 0, 0, w, w, 0, 0, 0, w, 0, 0, 0, w,
            w, 0, 0, 0, w, 0, 0, 0, w, w, 0, 0, 0, w, 0
        };
        buffer.resize(5 * 4 * 3);
        REQUIRE(buffer == expected);
    }
}
