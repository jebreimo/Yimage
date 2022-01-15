//****************************************************************************
// Copyright © 2022 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2022-01-08.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>
#include <Yimage/ReadImage.hpp>
#include <Yimage/WritePng.hpp>

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser(argv[0])
        .add(Argument("FILE").help("A png image."))
        .add(Argument("OUTPUT FILE")
            .help("The name of the resulting image."))
        .parse(argc, argv);
}

void print_image_specs(std::ostream& stream, const yimage::Image& img)
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
        auto image = yimage::read_image(args.value("FILE").as_string());
        print_image_specs(std::cout, image);
        yimage::Image output(image.width(), image.height(), yimage::PixelType::MONO_ALPHA_8);
        yimage::ImageView src = image;
        yimage::MutableImageView dst = output;
        for (unsigned y = 0; y < image.height(); ++y)
        {
            for (unsigned x = 0; x < image.width(); ++x)
            {
                auto rgba = yimage::get_rgba8(src, x, y);
                auto a = 255 - std::max(rgba.r, std::max(rgba.g, rgba.b));
                yimage::set_rgba8(dst, x, y, yimage::Rgba8{.a = uint8_t(a)});
            }
        }
        yimage::write_png(args.value("OUTPUT FILE").as_string(), output);
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
    }
    return 0;
}
