//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iomanip>
#include <iostream>
#include <Yimage/ReadTiff.hpp>

float get_float_pixel(const Yimage::Image& img, size_t x, size_t y)
{
    auto ptr = img.pixel_pointer(x, y);
    return *reinterpret_cast<const float*>(ptr);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: tiffreader FILENAME" << std::endl;
        return 1;
    }

    auto image = Yimage::read_tiff(argv[1]);
    if (!image)
    {
        std::cerr << "Failed to read image." << std::endl;
        return 1;
    }

    std::cout << "Image: " << image.width() << "x" << image.height() << std::endl;
    std::cout << std::fixed << std::setprecision(1);

    if (image.pixel_type() == Yimage::PixelType::MONO_FLOAT_32)
    {
        for (size_t y = 0; y < image.height(); ++y)
        {
            for (size_t x = 0; x < image.width(); ++x)
            {
                auto pixel = get_float_pixel(image, x, y);
                std::cout << pixel << " ";
            }
            std::cout << "\n" << std::endl;
        }
    }

    return 0;
}
