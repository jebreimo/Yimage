//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Yimage/ReadTiff.hpp>

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

    return 0;
}
