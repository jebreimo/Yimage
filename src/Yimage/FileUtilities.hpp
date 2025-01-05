//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdio>
#include <memory>

namespace Yimage
{
    struct FileCloser
    {
        void operator()(FILE* file) const
        {
            fclose(file);
        }
    };

    using UniqueFile = std::unique_ptr<FILE, FileCloser>;

    std::istream& make_buffer_istream(const char* data, size_t size);
}
