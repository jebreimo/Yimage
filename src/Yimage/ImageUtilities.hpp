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

namespace Yimage
{
    template <typename ViewType>
    ViewType make_subimage(auto&& view, size_t x, size_t y,
                           size_t width, size_t height)
    {
        x = std::min(x, view.width());
        y = std::min(y, view.height());
        width = std::min(width, view.width() - x);
        height = std::min(height, view.height() - y);
        auto gap_size = view.row_gap_size()
                        + ((view.width() - width) * view.pixel_size()) / 8;
        auto buffer = view.data() + y * view.row_size() + x * view.pixel_size() / 8;
        return ViewType(buffer, view.pixel_type(), width, height, gap_size);
    }
}
