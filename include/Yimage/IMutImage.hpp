//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-05-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IImage.hpp"

namespace Yimage
{
    class MutImageView;

    class IMutImage : public IImage
    {
    public:
        [[nodiscard]]
        virtual unsigned char* data() = 0;

        [[nodiscard]]
        virtual unsigned char* pixel_pointer(size_t x, size_t y) = 0;

        [[nodiscard]]
        virtual std::pair<unsigned char*, unsigned char*>
        row(size_t index) = 0;

        [[nodiscard]]
        virtual MutImageView mut_subimage(size_t x, size_t y) = 0;

        [[nodiscard]]
        virtual MutImageView
        mut_subimage(size_t x, size_t y, size_t width, size_t height) = 0;
    };

    void paste(const IImage& img, IMutImage& mut_img,
               ptrdiff_t x, ptrdiff_t y);

    void set_rgba8(IMutImage& image, size_t x, size_t y, Rgba8 rgba);

    void fill_rgba8(IMutImage& image, Rgba8 rgba);

    void fill_rgba8(IMutImage& image, const Rgba8* rgba, size_t num_rgba);
}
