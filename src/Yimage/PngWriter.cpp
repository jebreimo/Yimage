//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/PngWriter.hpp"
#include "Yimage/YimageException.hpp"

#include <ostream>
#include <utility>

namespace yimage
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

        size_t get_pixel_size(const PngInfo& info,
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

        size_t get_row_size(const PngInfo& info, const PngTransform& transform)
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

    PngWriter::PngWriter(std::ostream& stream, PngInfo  info, PngTransform transform)
        : m_info(std::move(info)),
          m_transform(transform)
    {
        m_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!m_png_ptr)
            YIMAGE_THROW("Can not create PNG struct.");
        m_info_ptr = png_create_info_struct(m_png_ptr);
        if (!m_info_ptr)
            YIMAGE_THROW("Can not create PNG info struct.");
        png_set_write_fn(m_png_ptr, &stream, user_write_data, user_flush_data);
    }

    PngWriter::PngWriter(PngWriter&& obj) noexcept
        : m_info(std::move(obj.m_info)),
          m_transform(obj.m_transform),
          m_png_ptr(nullptr),
          m_info_ptr(nullptr)
    {
        std::swap(m_png_ptr, obj.m_png_ptr);
        std::swap(m_info_ptr, obj.m_info_ptr);
    }

    PngWriter::~PngWriter()
    {
        if (m_png_ptr)
        {
            png_write_flush(m_png_ptr);
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
        }
    }

    PngWriter& PngWriter::operator=(PngWriter&& obj) noexcept
    {
        if (m_png_ptr)
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
        m_info = std::move(obj.m_info);
        m_transform = obj.m_transform;
        std::swap(m_png_ptr, obj.m_png_ptr);
        std::swap(m_info_ptr, obj.m_info_ptr);
        return *this;
    }

    PngWriter::operator bool() const
    {
        return m_png_ptr && m_info_ptr;
    }

    void PngWriter::write_info()
    {
        assert_is_valid();
        if (setjmp(png_jmpbuf(m_png_ptr)))
        {
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
            YIMAGE_THROW("Error while setting PNG info values.");
        }

        png_set_IHDR(m_png_ptr, m_info_ptr, m_info.width(), m_info.height(),
                     m_info.bit_depth(), m_info.color_type(),
                     m_info.interlace_type(), m_info.compression_method(),
                     m_info.filter_method());
        if (m_info.gamma())
            png_set_gAMA(m_png_ptr, m_info_ptr, *m_info.gamma());

        if (!m_info.texts().empty())
        {
            png_set_text(m_png_ptr, m_info_ptr,
                         m_info.texts().data(), int(m_info.texts().size()));
        }

        if (m_transform.invert_alpha())
            png_set_invert_alpha(m_png_ptr);

        if (setjmp(png_jmpbuf(m_png_ptr)))
        {
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
            YIMAGE_THROW("Error while writing PNG info.");
        }

        png_write_info(m_png_ptr, m_info_ptr);
    }

    void PngWriter::write(const void* image, size_t size)
    {
        auto rowSize = get_row_size(m_info, m_transform);
        if (size != m_info.height() * rowSize)
            YIMAGE_THROW("Incorrect image size.");
        std::vector<unsigned char*> rows;
        rows.reserve(m_info.height());

        auto ucImage = static_cast<unsigned char*>(const_cast<void*>(image));
        for (size_t i = 0; i < m_info.height(); ++i)
            rows.push_back(ucImage + i * rowSize);

        assert_is_valid();
        if (setjmp(png_jmpbuf(m_png_ptr)))
        {
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
            YIMAGE_THROW("Error while writing PNG image data.");
        }
        png_write_image(m_png_ptr, rows.data());
    }

    void PngWriter::write_rows(const void* rows[], uint32_t count,
                               size_t row_size)
    {
        if (row_size != get_row_size(m_info, m_transform))
            YIMAGE_THROW("Incorrect row size.");

        assert_is_valid();
        if (setjmp(png_jmpbuf(m_png_ptr)))
        {
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
            YIMAGE_THROW("Error while writing PNG rows.");
        }
        png_write_rows(
            m_png_ptr,
            reinterpret_cast<unsigned char**>(const_cast<void**>(rows)),
            count);
    }

    void PngWriter::write_row(const void* row, size_t size)
    {
        if (size != get_row_size(m_info, m_transform))
            YIMAGE_THROW("Incorrect row size.");

        assert_is_valid();
        if (setjmp(png_jmpbuf(m_png_ptr)))
        {
            png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
            YIMAGE_THROW("Error while writing PNG row.");
        }
        png_write_row(
            m_png_ptr,
            reinterpret_cast<unsigned char*>(const_cast<void*>(row)));
    }

    void PngWriter::write_end()
    {
        assert_is_valid();
        png_write_end(m_png_ptr, nullptr);
    }

    void PngWriter::assert_is_valid() const
    {
        if (!m_info_ptr || !m_png_ptr)
            YIMAGE_THROW("");
    }
}
