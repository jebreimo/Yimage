//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "MutableImageView.hpp"

namespace Yimage
{
    void fill_rgba8(const MutableImageView& image, Rgba8 rgba);

    void fill_rgba8(const MutableImageView& image, const Rgba8* rgba, size_t num_rgba);

    void flip_vertically(MutableImageView image);

    void paste(ImageView src,
               MutableImageView dst,
               ptrdiff_t x = 0,
               ptrdiff_t y = 0);
}
