//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include "Yimage/PixelType.hpp"
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    template <typename ViewType, typename ImageType>
    [[nodiscard]]
    ViewType make_subimage(ImageType&& img, size_t x, size_t y,
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
        return {buffer, img.pixel_type(), width, height, gap_size, img.metadata()};
    }

    [[nodiscard]]
    inline bool has_pixel_type(std::span<const PixelType> pixel_types,
                               PixelType pixel_type)
    {
        return std::ranges::find(pixel_types, pixel_type) != pixel_types.end();
    }

    /**
     * @brief Throws YimageException if @a pixel_type isn't in
     *  @a allowed_pixel_types.
     *
     * An empty @a allowed_pixel_types accepts all pixel types.
     */
    inline void check_pixel_type(PixelType pixel_type,
                                 std::span<const PixelType> allowed_pixel_types)
    {
        if (allowed_pixel_types.empty()
            || has_pixel_type(allowed_pixel_types, pixel_type))
        {
            return;
        }

        std::string allowed;
        for (auto type : allowed_pixel_types)
        {
            if (!allowed.empty())
                allowed += ", ";
            allowed += to_string(type);
        }
        throw YimageException("The image's pixel type (" + to_string(pixel_type)
                              + ") is not among the allowed pixel types: "
                              + allowed + ".");
    }
}
