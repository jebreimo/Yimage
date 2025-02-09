//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-12-26.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Tiff/TiffMetadata.hpp"

namespace Yimage
{
    TiffMetadata::TiffMetadata()
        : ImageMetadata(ImageFormat::TIFF)
    {
    }

    TiffMetadata* TiffMetadata::clone() const
    {
        return new TiffMetadata(*this);
    }
}
