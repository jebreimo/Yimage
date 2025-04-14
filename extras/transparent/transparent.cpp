//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/ReadImage.hpp>
#include <Yimage/Png/WritePng.hpp>

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser()
        .add(Argument("FILE").help("A png image."))
        .add(Argument("OUTPUT FILE")
            .help("The name of the resulting image."))
        .parse(argc, argv);
}

void print_image_specs(std::ostream& stream, const Yimage::Image& img)
{
    stream << "width: " << img.width()
           << "\nheight: " << img.height()
           << "\npixel type: " << int(img.pixel_type())
           << "\n";
}

int main(int argc, char* argv[])
{
    try
    {
        const auto args = parse_arguments(argc, argv);
        auto src = Yimage::read_image(args.value("FILE").as_string());
        print_image_specs(std::cout, src);
        Yimage::Image dst(Yimage::PixelType::MONO_ALPHA_8, src.width(), src.height());
        for (unsigned y = 0; y < src.height(); ++y)
        {
            for (unsigned x = 0; x < src.width(); ++x)
            {
                auto rgba = Yimage::get_rgba8(src.view(), x, y);
                auto a = 255 - std::max(rgba.r, std::max(rgba.g, rgba.b));
                Yimage::set_rgba8(dst.mutable_view(), x, y, Yimage::Rgba8{0, 0, 0, uint8_t(a)});
            }
        }
        Yimage::write_png(args.value("OUTPUT FILE").as_string(), dst.view());
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
    }
    return 0;
}
