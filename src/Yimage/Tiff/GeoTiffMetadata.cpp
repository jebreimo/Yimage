//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Tiff/GeoTiffMetadata.hpp"

namespace Yimage
{
    GeoTiffMetadata* GeoTiffMetadata::clone() const
    {
        return new GeoTiffMetadata(*this);
    }
}
