//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-11.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ImageView.hpp"
#include "PngInfo.hpp"
#include "PngTransform.hpp"

namespace Yimage
{
    void write_png(std::ostream& stream,
                   const void* image, size_t image_size,
                   PngInfo options, PngTransform transform);

    void write_png(const std::string& fileName,
                   const void* image, size_t image_size,
                   PngInfo info, PngTransform transform);

    void write_png(std::ostream& stream,
                   const ImageView& img);

    void write_png(const std::string& fileName,
                   const ImageView& img);
}
