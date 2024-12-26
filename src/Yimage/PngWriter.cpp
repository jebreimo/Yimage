//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PngWriter.hpp"

#include <ostream>
#include <utility>
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    namespace
    {
        size_t get_pixel_components(int colorType)
        {
            switch (colorType)
            {
            default:
                return 1;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                return 2;
            case PNG_COLOR_TYPE_RGB:
                return 3;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                return 4;
            }
        }

        size_t get_pixel_size(const PngMetadata& info,
                              const PngTransform& transform)
        {
            if (info.bit_depth() == 0)
                YIMAGE_THROW("Bit depth can not be 0.");
            size_t component_count = get_pixel_components(info.color_type());
            size_t component_size = info.bit_depth();
            if (component_size < 8 && component_count > 1)
                YIMAGE_THROW("Invalid combination of color type and bit depth.");

            if (component_size < 8)
                return transform.pixel_packing() ? 8 : component_size;

            if (transform.pixel_filler() && component_count % 2 == 1)
                component_count++;
            return component_count * component_size;
        }

        size_t get_row_size(const PngMetadata& info, const PngTransform& transform)
        {
            return (info.width() * get_pixel_size(info, transform) + 7) / 8;
        }

        extern "C" {

        void user_write_data(png_structp png_ptr,
                             png_bytep data,
                             png_size_t length)
        {
            auto stream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
            stream->write(reinterpret_cast<const char*>(data),
                          std::streamsize(length));
        }

        void user_flush_data(png_structp png_ptr)
        {
            auto stream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
            stream->flush();
        }

        }
    }

    PngWriter::PngWriter() = default;

    PngWriter::PngWriter(std::ostream& stream, PngMetadata  info, PngTransform transform)
        : metadata_(std::move(info)),
          transform_(transform)
    {
        png_ptr_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr_)
            YIMAGE_THROW("Can not create PNG struct.");
        info_ptr_ = png_create_info_struct(png_ptr_);
        if (!info_ptr_)
            YIMAGE_THROW("Can not create PNG info struct.");
        png_set_write_fn(png_ptr_, &stream, user_write_data, user_flush_data);
    }

    PngWriter::PngWriter(PngWriter&& obj) noexcept
        : metadata_(std::move(obj.metadata_)),
          transform_(obj.transform_),
          png_ptr_(nullptr),
          info_ptr_(nullptr)
    {
        std::swap(png_ptr_, obj.png_ptr_);
        std::swap(info_ptr_, obj.info_ptr_);
    }

    PngWriter::~PngWriter()
    {
        if (png_ptr_)
        {
            png_write_flush(png_ptr_);
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
        }
    }

    PngWriter& PngWriter::operator=(PngWriter&& obj) noexcept
    {
        if (this == &obj)
            return *this;
        if (png_ptr_)
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
        metadata_ = std::move(obj.metadata_);
        transform_ = obj.transform_;
        png_ptr_ = obj.png_ptr_;
        obj.png_ptr_ = nullptr;
        info_ptr_ = obj.info_ptr_;
        obj.info_ptr_ = nullptr;
        return *this;
    }

    PngWriter::operator bool() const
    {
        return png_ptr_ && info_ptr_;
    }

    void PngWriter::write_info()
    {
        assert_is_valid();
        if (setjmp(png_jmpbuf(png_ptr_)))
        {
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
            YIMAGE_THROW("Error while setting PNG info values.");
        }

        png_set_IHDR(png_ptr_, info_ptr_, metadata_.width(), metadata_.height(),
                     metadata_.bit_depth(), metadata_.color_type(),
                     metadata_.interlace_type(), metadata_.compression_method(),
                     metadata_.filter_method());
        if (metadata_.gamma())
            png_set_gAMA(png_ptr_, info_ptr_, *metadata_.gamma());

        if (!metadata_.texts().empty())
        {
            png_set_text(png_ptr_, info_ptr_,
                         metadata_.texts().data(), int(metadata_.texts().size()));
        }

        if (transform_.invert_alpha())
            png_set_invert_alpha(png_ptr_);

        if (setjmp(png_jmpbuf(png_ptr_)))
        {
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
            YIMAGE_THROW("Error while writing PNG info.");
        }

        png_write_info(png_ptr_, info_ptr_);
    }

    void PngWriter::write(const void* image, size_t size)
    {
        auto rowSize = get_row_size(metadata_, transform_);
        if (size != metadata_.height() * rowSize)
            YIMAGE_THROW("Incorrect image size.");
        std::vector<unsigned char*> rows;
        rows.reserve(metadata_.height());

        auto ucImage = static_cast<unsigned char*>(const_cast<void*>(image));
        for (size_t i = 0; i < metadata_.height(); ++i)
            rows.push_back(ucImage + i * rowSize);

        assert_is_valid();
        if (setjmp(png_jmpbuf(png_ptr_)))
        {
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
            YIMAGE_THROW("Error while writing PNG image data.");
        }
        png_write_image(png_ptr_, rows.data());
    }

    void PngWriter::write_rows(const void* rows[], uint32_t count,
                               size_t row_size)
    {
        if (row_size != get_row_size(metadata_, transform_))
            YIMAGE_THROW("Incorrect row size.");

        assert_is_valid();
        if (setjmp(png_jmpbuf(png_ptr_)))
        {
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
            YIMAGE_THROW("Error while writing PNG rows.");
        }
        png_write_rows(
            png_ptr_,
            reinterpret_cast<unsigned char**>(const_cast<void**>(rows)),
            count);
    }

    void PngWriter::write_row(const void* row, size_t size)
    {
        if (size != get_row_size(metadata_, transform_))
            YIMAGE_THROW("Incorrect row size.");

        assert_is_valid();
        if (setjmp(png_jmpbuf(png_ptr_)))
        {
            png_destroy_write_struct(&png_ptr_, &info_ptr_);
            YIMAGE_THROW("Error while writing PNG row.");
        }
        png_write_row(
            png_ptr_,
            reinterpret_cast<unsigned char*>(const_cast<void*>(row)));
    }

    void PngWriter::write_end()
    {
        assert_is_valid();
        png_write_end(png_ptr_, nullptr);
    }

    void PngWriter::assert_is_valid() const
    {
        if (!info_ptr_ || !png_ptr_)
            YIMAGE_THROW("");
    }
}
