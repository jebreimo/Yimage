//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <optional>

namespace Yimage
{
    class PngTransform
    {
    public:
        [[nodiscard]]
        bool invert_alpha() const;

        PngTransform& invert_alpha(bool value);

        [[nodiscard]]
        const std::optional<uint32_t>& pixel_filler() const;

        PngTransform& pixel_filler(std::optional<uint32_t> filler);

        [[nodiscard]]
        bool pixel_packing() const;

        PngTransform& pixel_packing(bool value);
    private:
        std::optional<uint32_t> m_pixel_filler;
        bool m_invert_alpha = false;
        bool m_pixel_packing = false;
    };
}
