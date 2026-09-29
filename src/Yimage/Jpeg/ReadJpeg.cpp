//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Jpeg/ReadJpeg.hpp"

#include <algorithm>
#include <jpeglib.h>
#include "Yimage/YimageException.hpp"
#include "../FileUtilities.hpp"
#include "../ImageUtilities.hpp"

namespace Yimage
{
    namespace
    {
        void handle_error(j_common_ptr cinfo)
        {
            char msg[JMSG_LENGTH_MAX];
            (*cinfo->err->format_message)(cinfo, msg);
            throw YimageException("Could not read JPEG image: "
                                  + std::string(msg));
        }

        struct JpegData
        {
            jpeg_error_mgr error_mgr = {};
            jpeg_decompress_struct info = {};
        };

        void create_decompress(JpegData& data)
        {
            data.info.err = jpeg_std_error(&data.error_mgr);
            data.error_mgr.error_exit = handle_error;
            jpeg_create_decompress(&data.info);
        }

        Image read_image(JpegData& data,
                         std::span<const PixelType> allowed_pixel_types)
        {
            jpeg_read_header(&data.info, TRUE);
            jpeg_calc_output_dimensions(&data.info);
            const auto pixel_type = data.info.output_components == 3
                                        ? PixelType::RGB_8
                                        : PixelType::MONO_8;
            check_pixel_type(pixel_type, allowed_pixel_types);

            auto row_size = data.info.output_width
                            * data.info.output_components;
            auto* buffer = (*data.info.mem->alloc_sarray)
                (reinterpret_cast<j_common_ptr>(&data.info), JPOOL_IMAGE, row_size, 1);
            jpeg_start_decompress(&data.info);

            Image image(pixel_type,
                        data.info.output_width,
                        data.info.output_height);

            auto* dst = image.data();

            while (data.info.output_scanline < data.info.output_height)
            {
                jpeg_read_scanlines(&data.info, buffer, 1);
                std::copy_n(buffer[0], row_size, dst);
                dst += row_size;
            }

            jpeg_finish_decompress(&data.info);
            jpeg_destroy_decompress(&data.info);

            image.set_metadata(std::make_unique<ImageMetadata>(ImageFormat::JPEG));

            return image;
        }
    }

    Image read_jpeg(FILE* file,
                    std::span<const PixelType> allowed_pixel_types)
    {
        JpegData data = {};
        try
        {
            create_decompress(data);
            jpeg_stdio_src(&data.info, file);
            return read_image(data, allowed_pixel_types);
        }
        catch (std::exception&)
        {
            jpeg_destroy_decompress(&data.info);
            throw;
        }
    }

    namespace
    {
#ifdef _WIN32
        FILE* my_fopen(const std::filesystem::path& path)
        {
            FILE* file = nullptr;
            if (const auto result = _wfopen_s(&file, path.c_str(), L"rb"); result != 0)
                YIMAGE_THROW("Could not open file: " + path.string() + " (error code: " + std::to_string(result) + ")");
            return file;
        }
#else
        FILE* my_fopen(const std::filesystem::path& path)
        {
            return fopen(path.c_str(), "rb");
        }
#endif
    }

    Image read_jpeg(const std::filesystem::path& path,
                    std::span<const PixelType> allowed_pixel_types)
    {
        UniqueFile file(my_fopen(path));
        if (!file)
            YIMAGE_THROW("Could not open file: " + path.string());
        auto img = read_jpeg(file.get(), allowed_pixel_types);
        if (auto metadata = img.metadata())
            metadata->path = path;
        return img;
    }

    Image read_jpeg(const void* buffer, size_t size,
                    std::span<const PixelType> allowed_pixel_types)
    {
        JpegData data = {};
        try
        {
            create_decompress(data);
            const auto* uc_buffer = static_cast<const unsigned char*>(buffer);
            jpeg_mem_src(&data.info, uc_buffer, size);
            return read_image(data, allowed_pixel_types);
        }
        catch (std::exception&)
        {
            jpeg_destroy_decompress(&data.info);
            throw;
        }
    }
}
