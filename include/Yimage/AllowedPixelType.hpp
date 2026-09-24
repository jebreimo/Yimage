//****************************************************************************
// Copyright © 2026 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2026-09-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Yimage
{
    enum class AllowedPixelType : unsigned
    {
        ALL = 0,
        RGBA_8 = 0x01,
        MONO_8 = 0x02,
        MONO_ALPHA_8 = 0x04,
        INDEXED_8 = 0x08,
        RGB_8 = 0x10,
        FLOAT_32 = 0x20,
    };
}