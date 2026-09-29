//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadImage.hpp"
#include <sstream>
#include "Yimage/Png/WritePng.hpp"
#include "Yimage/YimageException.hpp"
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

TEST_CASE("read_image: PNG with its own pixel type allowed")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::RGB_8,
                                   Yimage::PixelType::RGBA_8};
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::RGBA_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99132F1));
}

TEST_CASE("read_image: PNG RGBA to ARGB")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::ARGB_8};
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::ARGB_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 0, 0) == Yimage::Rgba8(0xFFFFFF00));
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99132F1));
}

TEST_CASE("read_image: PNG RGBA prefers losing alpha over losing color")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::MONO_ALPHA_8,
                                   Yimage::PixelType::RGB_8};
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::RGB_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99132FF));
}

TEST_CASE("read_image: PNG RGBA prefers lossless conversion")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::RGB_8,
                                   Yimage::PixelType::RGBA_16};
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::RGBA_16);
    auto* p = image.pixel_pointer(7, 27);
    REQUIRE(p[0] == 0xC9);
    REQUIRE(p[2] == 0x91);
    REQUIRE(p[4] == 0x32);
    REQUIRE(p[6] == 0xF1);
}

TEST_CASE("read_image: PNG RGBA to gray")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::MONO_8};
    auto image = Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::MONO_8);
}

TEST_CASE("read_image: PNG indexed to RGBA")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::RGB_8,
                                   Yimage::PixelType::RGBA_8};
    auto image = Yimage::read_image(THUMB_UP_PALETTE_PNG,
                                    THUMB_UP_PALETTE_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::RGBA_8);
    REQUIRE(image.palette().empty());
    REQUIRE(Yimage::get_rgba8(image.view(), 0, 0) == Yimage::Rgba8(0x47704C00));
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99031F1));
}

TEST_CASE("read_image: PNG indexed to RGB")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::RGB_8};
    auto image = Yimage::read_image(THUMB_UP_PALETTE_PNG,
                                    THUMB_UP_PALETTE_PNG_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::RGB_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 7, 27) == Yimage::Rgba8(0xC99031FF));
}

TEST_CASE("read_image: PNG with no reachable pixel type")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::INDEX_4,
                                   Yimage::PixelType::MONO_FLOAT_32};
    REQUIRE_THROWS_AS(Yimage::read_image(THUMB_UP_PNG, THUMB_UP_PNG_SIZE, allowed),
                      Yimage::YimageException);
}

TEST_CASE("read_image: JPEG with allowed pixel types")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::MONO_8,
                                   Yimage::PixelType::RGB_8};
    auto image = Yimage::read_image(CITY_JPG, CITY_JPG_SIZE, allowed);
    REQUIRE(bool(image));

    Yimage::PixelType disallowed[] = {Yimage::PixelType::RGBA_8};
    REQUIRE_THROWS_AS(Yimage::read_image(CITY_JPG, CITY_JPG_SIZE, disallowed),
                      Yimage::YimageException);
}

TEST_CASE("read_image: TIFF with allowed pixel types")
{
    Yimage::PixelType allowed[] = {Yimage::PixelType::MONO_FLOAT_32};
    auto image = Yimage::read_image(GEOID_TIF, GEOID_TIF_SIZE, allowed);
    REQUIRE(image.pixel_type() == Yimage::PixelType::MONO_FLOAT_32);

    Yimage::PixelType disallowed[] = {Yimage::PixelType::RGBA_8};
    REQUIRE_THROWS_AS(Yimage::read_image(GEOID_TIF, GEOID_TIF_SIZE, disallowed),
                      Yimage::YimageException);
}

namespace
{
    std::string make_png(Yimage::PixelType pixel_type,
                         std::initializer_list<unsigned char> pixel)
    {
        Yimage::Image image(pixel_type, 2, 1);
        std::ranges::copy(pixel, image.pixel_pointer(0, 0));
        std::ranges::copy(pixel, image.pixel_pointer(1, 0));
        std::ostringstream stream;
        Yimage::write_png(stream, image.view());
        return stream.str();
    }

    Yimage::Image read_png(const std::string& png,
                           std::initializer_list<Yimage::PixelType> allowed)
    {
        std::vector<Yimage::PixelType> types(allowed);
        return Yimage::read_image(png.data(), png.size(), types);
    }
}

TEST_CASE("read_image: PNG gray conversions")
{
    using Yimage::PixelType;
    auto png = make_png(PixelType::MONO_8, {0x40});

    auto image = read_png(png, {PixelType::RGBA_8});
    REQUIRE(image.pixel_type() == PixelType::RGBA_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 1, 0) == Yimage::Rgba8(0x404040FF));

    image = read_png(png, {PixelType::ARGB_8});
    REQUIRE(image.pixel_type() == PixelType::ARGB_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 1, 0) == Yimage::Rgba8(0x404040FF));

    image = read_png(png, {PixelType::ALPHA_MONO_8});
    REQUIRE(image.pixel_type() == PixelType::ALPHA_MONO_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 1, 0) == Yimage::Rgba8(0x404040FF));

    image = read_png(png, {PixelType::RGB_16});
    REQUIRE(image.pixel_type() == PixelType::RGB_16);
    auto* p = image.pixel_pointer(1, 0);
    REQUIRE(p[0] == 0x40);
    REQUIRE(p[1] == 0x40);
    REQUIRE(p[4] == 0x40);
}

TEST_CASE("read_image: PNG gray+alpha conversions")
{
    using Yimage::PixelType;
    auto png = make_png(PixelType::MONO_ALPHA_8, {0x40, 0x80});

    auto image = read_png(png, {PixelType::ARGB_8, PixelType::MONO_8});
    REQUIRE(image.pixel_type() == PixelType::ARGB_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 1, 0) == Yimage::Rgba8(0x40404080));

    image = read_png(png, {PixelType::ALPHA_MONO_8, PixelType::RGBA_8});
    REQUIRE(image.pixel_type() == PixelType::ALPHA_MONO_8);
    REQUIRE(Yimage::get_rgba8(image.view(), 1, 0) == Yimage::Rgba8(0x40404080));

    image = read_png(png, {PixelType::MONO_8});
    REQUIRE(image.pixel_type() == PixelType::MONO_8);
}
