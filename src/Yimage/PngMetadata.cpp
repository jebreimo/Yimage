//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PngMetadata.hpp"

namespace Yimage
{
    PngMetadata::PngMetadata()
        : ImageMetadata(ImageFormat::PNG)
    {}

    unsigned PngMetadata::width() const
    {
        return width_;
    }

    void PngMetadata::set_width(unsigned int value)
    {
        width_ = value;
    }

    unsigned PngMetadata::height() const
    {
        return height_;
    }

    void PngMetadata::set_height(unsigned int value)
    {
        height_ = value;
    }

    int PngMetadata::bit_depth() const
    {
        return bit_depth_;
    }

    void PngMetadata::set_bit_depth(int value)
    {
        bit_depth_ = value;
    }

    int PngMetadata::color_type() const
    {
        return color_type_;
    }

    void PngMetadata::set_color_type(int value)
    {
        color_type_ = value;
    }

    int PngMetadata::interlace_type() const
    {
        return interlace_type_;
    }

    void PngMetadata::set_interlace_type(int value)
    {
        interlace_type_ = value;
    }

    int PngMetadata::compression_method() const
    {
        return compression_method_;
    }

    void PngMetadata::set_compression_method(int value)
    {
        compression_method_ = value;
    }

    int PngMetadata::filter_method() const
    {
        return filter_method_;
    }

    void PngMetadata::set_filter_method(int value)
    {
        filter_method_ = value;
    }

    const std::optional<double>& PngMetadata::gamma() const
    {
        return gamma_;
    }

    void PngMetadata::set_gamma(std::optional<double> gamma)
    {
        gamma_ = gamma;
    }

    const std::vector<png_text>& PngMetadata::texts() const
    {
        return texts_;
    }

    void PngMetadata::add_text(std::string key, std::string value)
    {
        return add_text(std::move(key), std::move(value),
                        PNG_TEXT_COMPRESSION_NONE);
    }

    void PngMetadata::add_text(std::string key, std::string value,
                                       int compression)
    {
        strings_.push_back(std::move(key));
        auto key_ptr = &strings_.back();
        const std::string* value_ptr = nullptr;
        if (!value.empty())
        {
            strings_.push_back(std::move(value));
            value_ptr = &strings_.back();
        }
        texts_.push_back({
            compression,
            const_cast<char*>(key_ptr->c_str()),
            const_cast<char*>(value_ptr ? value_ptr->c_str() : nullptr),
            value_ptr ? value_ptr->size() : 0,
            0, nullptr, nullptr
        });
    }

    const std::optional<std::pair<unsigned, unsigned>>& PngMetadata::pixels_per_meter() const
    {
        return pixels_per_meter_;
    }

    void PngMetadata::set_pixels_per_meter(unsigned int value)
    {
        return set_pixels_per_meter(value, value);
    }

    void PngMetadata::set_pixels_per_meter(unsigned int hor_value, unsigned int ver_value)
    {
        pixels_per_meter_ = {hor_value, ver_value};
    }
}
