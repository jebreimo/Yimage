//****************************************************************************
// Copyright © 2026 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2026-09-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/ReadImage.hpp>
#include <Yimage/Png/WritePng.hpp>

namespace
{
    argos::ParsedArguments parse_arguments(int argc, char* argv[])
    {
        using namespace argos;
        return ArgumentParser()
            .add(Argument("FILE").help("An image file."))
            .parse(argc, argv);
    }
}

int main(int argc, char* argv[])
{
    const auto args = parse_arguments(argc, argv);

    try
    {
        Yimage::Image image = Yimage::read_image(args.value("FILE").as_string());
        std::cout << "Image size: " << image.width() << 'x' << image.height() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}