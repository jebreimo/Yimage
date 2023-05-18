//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <algorithm>
#include <cstddef>
#include "Yimage/MutImageView.hpp"
namespace Yimage
{
    [[nodiscard]]
    inline ImageView make_subimage(const IImage& img, size_t x, size_t y,
                                   size_t width, size_t height)
    {
        x = std::min(x, img.width());
        y = std::min(y, img.height());
        width = std::min(width, img.width() - x);
        height = std::min(height, img.height() - y);
        auto gap_size = img.row_gap_size()
                        + ((img.width() - width) * img.pixel_size()) / 8;
        auto buffer = img.data() + y * img.row_size()
                      + x * img.pixel_size() / 8;
        return {buffer, img.pixel_type(), width, height, gap_size};
    }

    [[nodiscard]]
    inline MutImageView
    make_mut_subimage(const IMutImage& img, size_t x, size_t y,
                      size_t width, size_t height)
    {
        x = std::min(x, img.width());
        y = std::min(y, img.height());
        width = std::min(width, img.width() - x);
        height = std::min(height, img.height() - y);
        auto gap_size = img.row_gap_size()
                        + ((img.width() - width) * img.pixel_size()) / 8;
        auto buffer = img.mut_data() + y * img.row_size()
                      + x * img.pixel_size() / 8;
        return {buffer, img.pixel_type(), width, height, gap_size};
    }
}
