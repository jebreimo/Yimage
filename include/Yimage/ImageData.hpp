//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "ImageView.hpp"

namespace yimage
{
    //struct ImageData
    //{
    //    unsigned width = 0;
    //    unsigned height = 0;
    //    PixelType pixel_type = PixelType::NONE;
    //    std::unique_ptr<unsigned char> buffer;
    //
    //    ImageData() = default;
    //
    //    ImageData(unsigned width, unsigned height, PixelType pixel_type)
    //        : width(width),
    //          height(height),
    //          pixel_type(pixel_type),
    //          buffer(new unsigned char[width * height * get_pixel_size(pixel_type)])
    //    {}
    //
    //    ImageData(ImageData&& rhs) noexcept
    //        : width(rhs.width),
    //          height(rhs.height),
    //          pixel_type(pixel_type),
    //          buffer(move(rhs.buffer))
    //    {
    //        rhs.width =
    //    }
    //    operator ImageView() const
    //    {
    //        return {buffer.get(), width, height, pixel_type};
    //    }
    //
    //    operator MutableImageView()
    //    {
    //        return {buffer.get(), width, height, pixel_type};
    //    }
    //};
}
