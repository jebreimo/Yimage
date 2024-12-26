//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <filesystem>
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/Yimage.hpp>

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser(argc ? argv[0] : "jpg2png")
        .add(Argument("FILE").help("The path to a JPEG file."))
        .parse(argc, argv);
}

int main(int argc, char* argv[])
{
    auto args = parse_arguments(argc, argv);
    auto jpg_path = std::filesystem::path(args.value("FILE").as_string());
    auto png_path = std::filesystem::path(jpg_path).replace_extension(".png");
    if (jpg_path == png_path)
    {
        std::cerr << "Not a JPEG file: " << jpg_path.string() << "\n";
        return 1;
    }
    auto image = Yimage::read_jpeg(jpg_path.string());
    Yimage::write_png(png_path.string(), image);
    return 0;
}
