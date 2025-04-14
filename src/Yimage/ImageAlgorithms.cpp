//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-08-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageAlgorithms.hpp"

#include <algorithm>
#include <vector>
#include "ColorBytes.hpp"
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    void fill_rgba8(const MutableImageView& image, Rgba8 rgba)
    {
        fill_rgba8(image, &rgba, 1);
    }

    void fill_rgba8(const MutableImageView& image, const Rgba8* rgba, size_t num_rgba)
    {
        if (!image)
            return;

        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < num_rgba; ++i)
        {
            auto cb = get_color_bytes(rgba[i], image.pixel_type());
            bytes.insert(bytes.end(), cb.bytes, cb.bytes + cb.size);
        }

        auto src_it = bytes.begin();
        for (size_t y = 0; y < image.height(); ++y)
        {
            auto [beg, end] = image.row(y);
            while (std::distance(src_it, bytes.end())
                   <= std::distance(beg, end))
            {
                beg = std::copy(src_it, bytes.end(), beg);
                src_it = bytes.begin();
            }
            if (beg != end)
            {
                std::copy(src_it, src_it + (end - beg), beg);
                std::advance(src_it, end - beg);
            }
        }
    }

    void flip_vertically(MutableImageView image)
    {
        if (image.height() <= 1)
            return;

        std::vector<unsigned char> buffer(image.row_size());
        auto buf_beg = buffer.data();
        for (size_t i = 0, n = image.height(); i < n / 2; ++i)
        {
            auto [src_beg, src_end] = image.row(i);
            auto buf_end = std::copy(src_beg, src_end, buf_beg);
            auto [btm_beg, btm_end] = image.row(n - i - 1);
            std::copy(btm_beg, btm_end, src_beg);
            std::copy(buf_beg, buf_end, btm_beg);
        }
    }

    void paste(ImageView src, MutableImageView dst, ptrdiff_t x, ptrdiff_t y)
    {
        if (src.pixel_type() != dst.pixel_type())
            YIMAGE_THROW("Source and destination images can't have different pixel types.");
        if (src.pixel_size() < 8)
            YIMAGE_THROW("Pixel sizes less than 8 bits are not supported.");

        if (x < 0)
            src = src.subimage(size_t(-x), 0);
        else
            dst = dst.subimage(size_t(x), 0);

        if (y < 0)
            src = src.subimage(0, size_t(-y));
        else
            dst = dst.subimage(0, size_t(y));

        auto width = std::min(src.width(), dst.width());
        auto height = std::min(src.height(), dst.height());
        src = src.subimage(0, 0, width, height);
        dst = dst.subimage(0, 0, width, height);

        if (src.is_contiguous() && dst.is_contiguous())
        {
            std::copy(src.data(), src.data() + src.size(), dst.data());
            return;
        }

        for (size_t i = 0; i < src.height(); ++i)
        {
            auto [i_b, i_e] = src.row(i);
            auto [m_b, m_e] = dst.row(i);
            std::copy(i_b, i_e, m_b);
        }
    }
}
