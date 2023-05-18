//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PngTransform.hpp"

namespace Yimage
{
    bool PngTransform::invert_alpha() const
    {
        return m_invert_alpha;
    }

    PngTransform& PngTransform::invert_alpha(bool value)
    {
        m_invert_alpha = value;
        return *this;
    }

    const std::optional<uint32_t>& PngTransform::pixel_filler() const
    {
        return m_pixel_filler;
    }

    PngTransform& PngTransform::pixel_filler(std::optional<uint32_t> filler)
    {
        m_pixel_filler = filler;
        return *this;
    }

    bool PngTransform::pixel_packing() const
    {
        return m_pixel_packing;
    }

    PngTransform& PngTransform::pixel_packing(bool value)
    {
        m_pixel_packing = value;
        return *this;
    }
}
