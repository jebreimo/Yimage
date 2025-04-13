//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <filesystem>
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/ReadImage.hpp>
#include <Yimage/Png/WritePng.hpp>

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser()
        .add(Argument("FILE").
            help("The path to a TIFF file with floating point pixels."))
        .parse(argc, argv);
}

template <typename T>
std::pair<const T*, const T*> get_image_row(const Yimage::Image& image, size_t y)
{
    auto [b, e] = image.row(y);
    if ((e - b) % sizeof(T) != 0)
        throw std::runtime_error("The row size is not a multiple of the pixel size.");
    return {reinterpret_cast<T*>(b), reinterpret_cast<T*>(e)};
}

template <typename T>
std::pair<T*, T*> get_image_row(Yimage::Image& image, size_t y)
{
    auto [b, e] = image.row(y);
    if ((e - b) % sizeof(T) != 0)
        throw std::runtime_error("The row size is not a multiple of the pixel size.");
    return {reinterpret_cast<T*>(b), reinterpret_cast<T*>(e)};
}

std::pair<float, float> get_float_min_max(const Yimage::Image& img)
{
    auto min = std::numeric_limits<float>::max();
    auto max = std::numeric_limits<float>::lowest();
    for (size_t y = 0; y < img.height(); ++y)
    {
        auto [b, e] = img.row(y);
        for (auto p = b; p != e; p += sizeof(float))
        {
            auto pixel = *reinterpret_cast<const float*>(p);
            min = std::min(min, pixel);
            max = std::max(max, pixel);
        }
    }
    return {min, max};
}

float get_float_pixel(const Yimage::Image& img, size_t x, size_t y)
{
    auto ptr = img.pixel_pointer(x, y);
    return *reinterpret_cast<const float*>(ptr);
}

int convert_image(const std::filesystem::path& input_path)
{
    auto png_path = std::filesystem::path(input_path).replace_extension(".png");

    auto src_image = Yimage::read_image(input_path.string());
    if (!src_image)
    {
        std::cerr << "Failed to read image." << std::endl;
        return 1;
    }

    if (src_image.pixel_type() != Yimage::PixelType::MONO_FLOAT_32)
    {
        std::cerr << "The image is not a 32-bit floating point image." << std::endl;
        return 1;
    }

    auto [min, max] = get_float_min_max(src_image);

    Yimage::Image dst_image(Yimage::PixelType::MONO_8, src_image.width(), src_image.height());

    std::vector<uint8_t> row(src_image.width());

    for (size_t y = 0; y < src_image.height(); ++y)
    {
        auto [b, e] = get_image_row<float>(src_image, y);
        auto [out_b, out_e] = get_image_row<uint8_t>(dst_image, y);
        auto out_p = out_b;
        for (auto p = b; p != e; ++p, ++out_p)
        {
            if (*p == 0)
                *out_p = 0;
            else
                *out_p = uint8_t(64 + 191 * (*p - min) / (max - min));
        }
    }

    Yimage::write_png(png_path.string(), dst_image.view());
    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        auto args = parse_arguments(argc, argv);
        return convert_image(args.value("FILE").as_string());
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
