//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include <iosfwd>
#include "PngMetadata.hpp"
#include "PngTransform.hpp"

namespace Yimage
{
    class PngWriter
    {
    public:
        PngWriter();

        PngWriter(std::ostream& stream, PngMetadata info, PngTransform transform);

        PngWriter(PngWriter&& obj) noexcept;

        ~PngWriter();

        PngWriter& operator=(PngWriter&& obj) noexcept;

        explicit operator bool() const;

        void write_info();

        void write(const void* image, size_t size);

        void write_rows(const void* rows[], uint32_t count, size_t row_size);

        void write_row(const void* row, size_t size);

        void write_end();
    private:
        void assert_is_valid() const;

        PngMetadata metadata_;
        PngTransform transform_;
        png_structp png_ptr_ = nullptr;
        png_infop info_ptr_ = nullptr;
    };
}
