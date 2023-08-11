//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/ReadPng.hpp"

#include <png.h>
#include <fstream>
#include <span>
#include <vector>
#include "Yimage/YimageException.hpp"

namespace Yimage
{
    namespace
    {
        class MemoryReader
        {
        public:
            MemoryReader(const void* buffer, size_t size)
                : span_(static_cast<const unsigned char*>(buffer), size)
            {}

            bool read(unsigned char* dest, size_t count)
            {
                if (current_ + count > span_.size())
                    return false;
                auto first = span_.data() + current_;
                current_ += count;
                auto last = span_.data() + current_;
                std::copy(first, last, dest);
                return true;
            }
        private:
            std::span<const unsigned char> span_;
            size_t current_ = 0;
        };

        extern "C" {

        void user_read_istream_data(png_structp png_ptr,
                                    png_bytep data,
                                    png_size_t length)
        {
            auto stream = static_cast<std::istream*>(png_get_io_ptr(png_ptr));
            stream->read(reinterpret_cast<char*>(data),
                          std::streamsize(length));
            if (size_t(stream->gcount()) != length)
                png_error(png_ptr, "Could not read the requested number of bytes.");
        }

        void user_read_buffer_data(png_structp png_ptr,
                                   png_bytep data,
                                   png_size_t length)
        {
            auto reader = static_cast<MemoryReader*>(png_get_io_ptr(png_ptr));
            if (!reader->read(data, length))
                png_error(png_ptr, "Could not read the requested number of bytes.");
        }
        }
    }

    struct PngHandle
    {
        PngHandle() = default;

        PngHandle(png_structp png_ptr, png_infop info_ptr)
            : png_ptr(png_ptr),
              info_ptr(info_ptr)
        {}

        PngHandle(PngHandle&& obj) noexcept
        {
            std::swap(png_ptr, obj.png_ptr);
            std::swap(info_ptr, obj.info_ptr);
        }

        ~PngHandle()
        {
            if (png_ptr)
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        }

        PngHandle& operator=(PngHandle&& obj) noexcept
        {
            if (this == &obj)
                return *this;

            if (png_ptr)
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            std::swap(png_ptr, obj.png_ptr);
            std::swap(info_ptr, obj.info_ptr);
            return *this;
        }

        png_structp png_ptr = nullptr;
        png_infop info_ptr = nullptr;
    };

    PngHandle create_png_handle()
    {
        auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr)
            YIMAGE_THROW("Can not create PNG struct.");
        auto info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
            YIMAGE_THROW("Can not create PNG info struct.");
        }
        return {png_ptr, info_ptr};
    }

    PixelType get_pixel_type(unsigned char color_type, unsigned char bit_depth)
    {
        switch (color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            switch (bit_depth)
            {
            case 1: return PixelType::MONO_1;
            case 2: return PixelType::MONO_2;
            case 4: return PixelType::MONO_4;
            case 8: return PixelType::MONO_8;
            case 16: return PixelType::MONO_16;
            default: break;
            }
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (bit_depth == 8)
                return PixelType::MONO_ALPHA_8;
            else if (bit_depth == 16)
                return PixelType::MONO_ALPHA_16;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            break;
        case PNG_COLOR_TYPE_RGB:
            if (bit_depth == 8)
                return PixelType::RGB_8;
            else if (bit_depth == 16)
                return PixelType::RGB_16;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (bit_depth == 8)
                return PixelType::RGBA_8;
            else if (bit_depth == 16)
                return PixelType::RGBA_16;
            break;
        }

        YIMAGE_THROW("Unsupported combination of color_type, "
                     + std::to_string(color_type) + ", and bit_depth, "
                     + std::to_string(bit_depth) + ".");
    }

    Image read_png(const PngHandle& png)
    {
        png_read_info(png.png_ptr, png.info_ptr);
        const auto width = png_get_image_width(png.png_ptr, png.info_ptr);
        const auto height = png_get_image_height(png.png_ptr, png.info_ptr);
        const auto bit_depth = png_get_bit_depth(png.png_ptr, png.info_ptr);
        const auto color_type = png_get_color_type(png.png_ptr, png.info_ptr);
        //const auto channels = png_get_channels(png.png_ptr, png.info_ptr);
        //const auto pixel_size = channels * bit_depth;
        //const auto row_size = width * pixel_size;
        //const auto image_size = row_size * height;
        Image image(get_pixel_type(color_type, bit_depth), width, height);
        std::vector<uint8_t*> row_pointers(height);
        for (size_t i = 0; i < height; ++i)
            row_pointers[i] = image.pixel_pointer(0, i);
        png_read_image(png.png_ptr, row_pointers.data());
        return image;
    }

    Image read_png(std::istream& stream)
    {
        auto png = create_png_handle();
        png_set_read_fn(png.png_ptr, &stream, user_read_istream_data);
        return read_png(png);
    }

    Image read_png(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            YIMAGE_THROW("Can not open file: " + path);
        return read_png(file);
    }

    Image read_png(const void* buffer, size_t size)
    {
        auto png = create_png_handle();
        MemoryReader reader(buffer, size);
        png_set_read_fn(png.png_ptr, &reader, user_read_buffer_data);
        return read_png(png);
    }
}
