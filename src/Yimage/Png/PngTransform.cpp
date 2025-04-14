//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Png/PngTransform.hpp"

namespace Yimage
{
    bool PngTransform::invert_alpha() const
    {
        return invert_alpha_;
    }

    PngTransform& PngTransform::invert_alpha(bool value)
    {
        invert_alpha_ = value;
        return *this;
    }

    const std::optional<uint32_t>& PngTransform::pixel_filler() const
    {
        return pixel_filler_;
    }

    PngTransform& PngTransform::pixel_filler(std::optional<uint32_t> filler)
    {
        pixel_filler_ = filler;
        return *this;
    }

    bool PngTransform::pixel_packing() const
    {
        return pixel_packing_;
    }

    PngTransform& PngTransform::pixel_packing(bool value)
    {
        pixel_packing_ = value;
        return *this;
    }
}
