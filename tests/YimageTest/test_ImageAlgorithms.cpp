//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageAlgorithms.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test flip RGB image vertically")
{
    using namespace Yimage;
    std::vector<uint8_t> buffer{
        0, 1, 2, 3, 255,
        4, 5, 6, 7, 254,
        8, 9, 10, 11, 253
    };

    MutableImageView image(buffer.data(), PixelType::MONO_8, 4, 3, 1);
    flip_vertically(image);
    REQUIRE(buffer[0] == 8);
    REQUIRE(buffer[3] == 11);
    REQUIRE(buffer[4] == 255);
    REQUIRE(buffer[5] == 4);
    REQUIRE(buffer[8] == 7);
    REQUIRE(buffer[9] == 254);
    REQUIRE(buffer[10] == 0);
    REQUIRE(buffer[13] == 3);
    REQUIRE(buffer[14] == 253);
}

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

TEST_CASE("test fill_rgba8")
{
    using namespace Yimage;
    std::vector<uint8_t> buffer(8 * 8);
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
