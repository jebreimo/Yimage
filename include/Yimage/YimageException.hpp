//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>

namespace Yimage
{
    class YimageException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#define YIMAGE_THROW_3_(file, line, msg) \
    throw ::Yimage::YimageException(file ":" #line ": " msg)

#define YIMAGE_THROW_2_(file, line, msg) \
    YIMAGE_THROW_3_(file, line, msg)

#define YIMAGE_THROW(msg) \
    YIMAGE_THROW_2_(__FILE__, __LINE__, msg)
