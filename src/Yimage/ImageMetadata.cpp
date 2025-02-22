//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-25.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ImageMetadata.hpp"

namespace Yimage
{
    ImageMetadata::ImageMetadata(ImageFormat format)
        : format(format)
    {
    }

    ImageMetadata::~ImageMetadata()
    {
    }

    ImageMetadata* ImageMetadata::clone() const
    {
        return new ImageMetadata(*this);
    }
}
