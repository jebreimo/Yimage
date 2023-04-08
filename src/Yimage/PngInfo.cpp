//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PngInfo.hpp"
#include "Yimage/YimageException.hpp"

namespace yimage
{
    unsigned PngInfo::width() const
    {
        return m_width;
    }

    PngInfo& PngInfo::width(unsigned int value)
    {
        m_width = value;
        return *this;
    }

    unsigned PngInfo::height() const
    {
        return m_height;
    }

    PngInfo& PngInfo::height(unsigned int value)
    {
        m_height = value;
        return *this;
    }

    int PngInfo::bit_depth() const
    {
        return m_bit_depth;
    }

    PngInfo& PngInfo::bit_depth(int value)
    {
        m_bit_depth = value;
        return *this;
    }

    int PngInfo::color_type() const
    {
        return m_color_type;
    }

    PngInfo& PngInfo::color_type(int value)
    {
        m_color_type = value;
        return *this;
    }

    int PngInfo::interlace_type() const
    {
        return m_interlace_type;
    }

    PngInfo& PngInfo::interlace_type(int value)
    {
        m_interlace_type = value;
        return *this;
    }

    int PngInfo::compression_method() const
    {
        return m_compression_method;
    }

    PngInfo& PngInfo::compression_method(int value)
    {
        m_compression_method = value;
        return *this;
    }

    int PngInfo::filter_method() const
    {
        return m_filter_method;
    }

    PngInfo& PngInfo::filter_method(int value)
    {
        m_filter_method = value;
        return *this;
    }

    const std::optional<double>& PngInfo::gamma() const
    {
        return m_gamma;
    }

    PngInfo& PngInfo::gamma(std::optional<double> gamma)
    {
        m_gamma = gamma;
        return *this;
    }

    const std::vector<png_text>& PngInfo::texts() const
    {
        return m_texts;
    }

    PngInfo& PngInfo::add_text(std::string key, std::string value)
    {
        return add_text(std::move(key), std::move(value),
                        PNG_TEXT_COMPRESSION_NONE);
    }

    PngInfo& PngInfo::add_text(std::string key, std::string value, int compression)
    {
        m_strings.push_back(std::move(key));
        auto key_ptr = &m_strings.back();
        const std::string* value_ptr = nullptr;
        if (!value.empty())
        {
            m_strings.push_back(value);
            value_ptr = &m_strings.back();
        }
        m_texts.push_back({compression,
                           const_cast<char*>(key_ptr->c_str()),
                           const_cast<char*>(value_ptr ? value_ptr->c_str() : nullptr),
                           value_ptr ? value_ptr->size() : 0,
                           0, nullptr, nullptr});
        return *this;
    }

    const std::optional<std::pair<unsigned, unsigned>>& PngInfo::pixels_per_meter() const
    {
        return m_pixels_per_meter;
    }

    PngInfo& PngInfo::pixels_per_meter(unsigned int value)
    {
        return pixels_per_meter(value, value);
    }

    PngInfo& PngInfo::pixels_per_meter(unsigned int hor_value, unsigned int ver_value)
    {
        m_pixels_per_meter = {hor_value, ver_value};
        return *this;
    }
}
