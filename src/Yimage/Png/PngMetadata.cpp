//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Png/PngMetadata.hpp"

namespace Yimage
{
    PngMetadata::PngMetadata()
        : ImageMetadata(ImageFormat::PNG)
    {}

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
}
