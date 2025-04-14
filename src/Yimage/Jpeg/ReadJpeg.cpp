//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-11-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Jpeg/ReadJpeg.hpp"

#include <algorithm>
#include <jpeglib.h>
#include "Yimage/YimageException.hpp"
#include "../FileUtilities.hpp"

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

        Image read_image(JpegData& data)
        {
            jpeg_read_header(&data.info, TRUE);
            jpeg_calc_output_dimensions(&data.info);
            auto row_size = data.info.output_width
                            * data.info.output_components;
            auto* buffer = (*data.info.mem->alloc_sarray)
                (j_common_ptr(&data.info), JPOOL_IMAGE, row_size, 1);
            jpeg_start_decompress(&data.info);

            Image image(data.info.output_components == 3
                                  ? PixelType::RGB_8
                                  : PixelType::MONO_8,
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

    Image read_jpeg(FILE* file)
    {
        JpegData data = {};
        try
        {
            create_decompress(data);
            jpeg_stdio_src(&data.info, file);
            return read_image(data);
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
        FILE* my_fopen(const std::filesystem::path& path, const char* mode)
        {
            return _wfopen(path.c_str(), mode);
        }
#else
        FILE* my_fopen(const std::filesystem::path& path, const char* mode)
        {
            return fopen(path.c_str(), mode);
        }
#endif
    }

    Image read_jpeg(const std::filesystem::path& path)
    {
        UniqueFile file(my_fopen(path.c_str(), "rb"));
        if (!file)
            YIMAGE_THROW("Could not open file: " + path.string());
        auto img = read_jpeg(file.get());
        if (auto metadata = img.metadata())
            metadata->path = path;
        return img;
    }

    Image read_jpeg(const void* buffer, size_t size)
    {
        JpegData data = {};
        try
        {
            create_decompress(data);
            const auto* uc_buffer = static_cast<const unsigned char*>(buffer);
            jpeg_mem_src(&data.info, uc_buffer, size);
            return read_image(data);
        }
        catch (std::exception&)
        {
            jpeg_destroy_decompress(&data.info);
            throw;
        }
    }
}
