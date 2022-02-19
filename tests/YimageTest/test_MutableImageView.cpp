//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Yimage/Image.hpp"

//yimage::Image make_mono_image(unsigned width, unsigned height,
//                              const std::vector<uint8_t>& pixels)
//{
//    return
//}

TEST_CASE("test MutableImageView::paste")
{
    using namespace yimage;
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
    MutableImageView mut_image1(buffer1.data(), 4, 4, PixelType::MONO_8);
    ImageView image1 = mut_image1;
    ImageView image2(buffer2.data(), 2, 2, PixelType::MONO_8);

    SECTION("in the center")
    {
        paste(image2, 1, 1, mut_image1);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 3,
            4, 20, 21, 7,
            8, 22, 23, 11,
            12, 13, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), 4, 4, PixelType::MONO_8));
    }
    SECTION("partially outside 1")
    {
        paste(image2, 3, -1, mut_image1);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 22,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), 4, 4, PixelType::MONO_8));
    }
    SECTION("partially outside 2")
    {
        paste(image2, 0, 3, mut_image1);
        std::vector<uint8_t> buffer3{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            20, 21, 14, 15
        };
        REQUIRE(image1 == ImageView(buffer3.data(), 4, 4, PixelType::MONO_8));
    }
}
