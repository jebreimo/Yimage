//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-12-27.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yimage/Png/ReadPng.hpp"

#include <png.h>
#include <algorithm>
#include <fstream>
#include <optional>
#include <span>
#include <vector>

#include "Yimage/Png/PngMetadata.hpp"
#include "Yimage/YimageException.hpp"
#include "../ImageUtilities.hpp"

namespace Yimage
{
    namespace
    {
        class MemoryReader
        {
        public:
            MemoryReader(const void* buffer, size_t size)
                : span_(static_cast<const unsigned char*>(buffer), size)
            {
            }

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

        struct PngHandle
        {
            PngHandle() = default;

            PngHandle(png_structp png_ptr, png_infop info_ptr)
                : png_ptr(png_ptr),
                  info_ptr(info_ptr)
            {
            }

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

        PixelType get_pixel_type(uint8_t color_type, uint8_t bit_depth)
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
                if (bit_depth == 16)
                    return PixelType::MONO_ALPHA_16;
                break;
            case PNG_COLOR_TYPE_PALETTE:
                switch (bit_depth)
                {
                case 1: return PixelType::INDEX_1;
                case 2: return PixelType::INDEX_2;
                case 4: return PixelType::INDEX_4;
                case 8: return PixelType::INDEX_8;
                default: break;
                }
                break;
            case PNG_COLOR_TYPE_RGB:
                if (bit_depth == 8)
                    return PixelType::RGB_8;
                if (bit_depth == 16)
                    return PixelType::RGB_16;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                if (bit_depth == 8)
                    return PixelType::RGBA_8;
                if (bit_depth == 16)
                    return PixelType::RGBA_16;
                break;
            default:
                break;
            }

            YIMAGE_THROW("Unsupported combination of color_type ("
                + std::to_string(color_type) + ") and bit_depth ("
                + std::to_string(bit_depth) + ").");
        }

#ifdef PNG_tRNS_SUPPORTED
        void update_palette_alpha(const PngHandle& png, std::vector<Rgba8>& palette)
        {
            png_bytep trans;
            int num_trans;
            png_color_16p trans_values;
            if (png_get_tRNS(png.png_ptr, png.info_ptr, &trans, &num_trans, &trans_values))
            {
                for (int i = 0; i < num_trans; ++i)
                    palette[i].a = trans[i];
            }
        }
#else
        void update_palette_alpha(const PngHandle&, std::vector<Rgba8>&)
        {
        }
#endif

        std::vector<Rgba8> get_palette(const PngHandle& png)
        {
            png_colorp palette;
            int num_palette;
            png_get_PLTE(png.png_ptr, png.info_ptr, &palette, &num_palette);
            std::vector<Rgba8> result(num_palette);
            for (int i = 0; i < num_palette; ++i)
            {
                result[i] = {
                    palette[i].red,
                    palette[i].green,
                    palette[i].blue,
                    0xFF
                };
            }
            update_palette_alpha(png, result);
            return result;
        }

        /**
         * @brief The properties of a pixel type that matter when choosing
         *  which pixel type a PNG image should be converted to.
         */
        struct PixelFormat
        {
            bool indexed = false;
            bool color = false;
            bool alpha = false;
            bool alpha_first = false;
            /// Bits per channel, or per index for indexed pixels.
            unsigned bit_depth = 0;
        };

        std::optional<PixelFormat> get_pixel_format(PixelType pixel_type)
        {
            switch (pixel_type)
            {
            case PixelType::MONO_1: return PixelFormat{.bit_depth = 1};
            case PixelType::MONO_2: return PixelFormat{.bit_depth = 2};
            case PixelType::MONO_4: return PixelFormat{.bit_depth = 4};
            case PixelType::MONO_8: return PixelFormat{.bit_depth = 8};
            case PixelType::MONO_16: return PixelFormat{.bit_depth = 16};
            case PixelType::MONO_ALPHA_8:
                return PixelFormat{.alpha = true, .bit_depth = 8};
            case PixelType::MONO_ALPHA_16:
                return PixelFormat{.alpha = true, .bit_depth = 16};
            case PixelType::ALPHA_MONO_8:
                return PixelFormat{.alpha = true, .alpha_first = true, .bit_depth = 8};
            case PixelType::ALPHA_MONO_16:
                return PixelFormat{.alpha = true, .alpha_first = true, .bit_depth = 16};
            case PixelType::RGB_8:
                return PixelFormat{.color = true, .bit_depth = 8};
            case PixelType::RGB_16:
                return PixelFormat{.color = true, .bit_depth = 16};
            case PixelType::RGBA_8:
                return PixelFormat{.color = true, .alpha = true, .bit_depth = 8};
            case PixelType::RGBA_16:
                return PixelFormat{.color = true, .alpha = true, .bit_depth = 16};
            case PixelType::ARGB_8:
                return PixelFormat{.color = true, .alpha = true, .alpha_first = true,
                                   .bit_depth = 8};
            case PixelType::ARGB_16:
                return PixelFormat{.color = true, .alpha = true, .alpha_first = true,
                                   .bit_depth = 16};
            case PixelType::INDEX_1:
                return PixelFormat{.indexed = true, .color = true, .bit_depth = 1};
            case PixelType::INDEX_2:
                return PixelFormat{.indexed = true, .color = true, .bit_depth = 2};
            case PixelType::INDEX_4:
                return PixelFormat{.indexed = true, .color = true, .bit_depth = 4};
            case PixelType::INDEX_8:
                return PixelFormat{.indexed = true, .color = true, .bit_depth = 8};
            default:
                return {};
            }
        }

        /**
         * @brief Returns true if libpng's transformations can convert
         *  pixels of format @a src to pixels of format @a dst.
         */
        bool can_convert(const PixelFormat& src, const PixelFormat& dst)
        {
            // libpng can unpack indexes to 8 bits, but it can't convert
            // anything else to indexed pixels.
            if (dst.indexed)
                return src.indexed && dst.bit_depth == 8;
            // libpng can only produce gray and color pixels with
            // 8 or 16 bits per channel.
            return dst.bit_depth == 8 || dst.bit_depth == 16;
        }

        /**
         * @brief Returns a measure of the distance between pixel formats
         *  @a src and @a dst. Lower values are better.
         *
         * Conversions that lose information are always worse than those
         * that don't. Loss of color is worse than loss of transparency,
         * which is worse than loss of precision. Among conversions that
         * lose the same kind of information, the one resulting in the
         * smallest pixels wins.
         */
        unsigned get_conversion_cost(const PixelFormat& src,
                                     const PixelFormat& dst,
                                     PixelType dst_type)
        {
            auto cost = unsigned(get_pixel_size(dst_type));
            if (src.color && !dst.color)
                cost += 4000;
            if (src.alpha && !dst.alpha)
                cost += 2000;
            // Palette entries have 8 bits per channel.
            auto src_depth = src.indexed ? 8u : src.bit_depth;
            if (src_depth > dst.bit_depth)
                cost += 1000;
            return cost;
        }

        /**
         * @brief Returns the pixel type in @a allowed_pixel_types that is
         *  closest to @a src and can be produced by libpng.
         *
         * If several pixel types are equally close, the first one in
         * @a allowed_pixel_types is returned.
         */
        std::optional<PixelType>
        find_closest_pixel_type(const PixelFormat& src,
                                std::span<const PixelType> allowed_pixel_types)
        {
            std::optional<PixelType> best_type;
            unsigned best_cost = 0;
            for (auto type : allowed_pixel_types)
            {
                auto dst = get_pixel_format(type);
                if (!dst || !can_convert(src, *dst))
                    continue;

                auto cost = get_conversion_cost(src, *dst, type);
                if (!best_type || cost < best_cost)
                {
                    best_type = type;
                    best_cost = cost;
                }
            }
            return best_type;
        }

        void set_transformations(const PngHandle& png,
                                 const PixelFormat& src,
                                 const PixelFormat& dst)
        {
            auto color_type = png_get_color_type(png.png_ptr, png.info_ptr);
            bool has_trns = png_get_valid(png.png_ptr, png.info_ptr,
                                          PNG_INFO_tRNS) != 0;
            bool has_alpha_channel = (color_type & PNG_COLOR_MASK_ALPHA) != 0;

            if (dst.indexed)
            {
                // The only conversion between indexed types that libpng
                // supports is unpacking 1, 2 and 4 bit indexes to 8 bits.
                png_set_packing(png.png_ptr);
                return;
            }

            if (src.indexed)
                png_set_palette_to_rgb(png.png_ptr);
            else if (src.bit_depth < 8)
                png_set_expand_gray_1_2_4_to_8(png.png_ptr);

            if (dst.alpha)
            {
                if (has_trns)
                    png_set_tRNS_to_alpha(png.png_ptr);

                if (!has_alpha_channel && !has_trns)
                {
                    // libpng only uses the lower 8 bits of the filler
                    // value when the bit depth is 8.
                    png_set_add_alpha(png.png_ptr, 0xFFFF,
                                      dst.alpha_first
                                          ? PNG_FILLER_BEFORE
                                          : PNG_FILLER_AFTER);
                }
                else if (dst.alpha_first)
                {
                    png_set_swap_alpha(png.png_ptr);
                }
            }
            else if (has_alpha_channel || has_trns)
            {
                // Also prevents palette_to_rgb and expand_16 from turning
                // tRNS into an alpha channel.
                png_set_strip_alpha(png.png_ptr);
            }

            if (dst.color && !src.color)
                png_set_gray_to_rgb(png.png_ptr);
            else if (!dst.color && src.color)
                png_set_rgb_to_gray_fixed(png.png_ptr, PNG_ERROR_ACTION_NONE, -1, -1);

            // Palette entries have 8 bits per channel, and gray values with
            // less than 8 bits have already been expanded to 8 bits.
            auto src_depth = src.indexed ? 8u : std::max(src.bit_depth, 8u);
            if (src_depth < dst.bit_depth)
                png_set_expand_16(png.png_ptr);
            else if (src_depth > dst.bit_depth)
                png_set_scale_16(png.png_ptr);
        }

        PixelType to_alpha_first(PixelType pixel_type)
        {
            switch (pixel_type)
            {
            case PixelType::MONO_ALPHA_8: return PixelType::ALPHA_MONO_8;
            case PixelType::MONO_ALPHA_16: return PixelType::ALPHA_MONO_16;
            case PixelType::RGBA_8: return PixelType::ARGB_8;
            case PixelType::RGBA_16: return PixelType::ARGB_16;
            default: return pixel_type;
            }
        }

        /**
         * @brief Sets up the transformations required to convert the image
         *  to the closest pixel type in @a allowed_pixel_types, unless its
         *  pixel type already is in @a allowed_pixel_types.
         *
         * @return The pixel type of the image after the transformations.
         * @throw YimageException if the image can't be converted to any of
         *  the pixel types in @a allowed_pixel_types.
         */
        PixelType set_transformations(const PngHandle& png,
                                      std::span<const PixelType> allowed_pixel_types)
        {
            auto color_type = png_get_color_type(png.png_ptr, png.info_ptr);
            auto bit_depth = png_get_bit_depth(png.png_ptr, png.info_ptr);
            auto pixel_type = get_pixel_type(color_type, bit_depth);

            if (allowed_pixel_types.empty()
                || has_pixel_type(allowed_pixel_types, pixel_type))
            {
                return pixel_type;
            }

            auto src = *get_pixel_format(pixel_type);
            // A tRNS chunk adds transparency to an image without an alpha
            // channel, but doesn't change its pixel type.
            if (png_get_valid(png.png_ptr, png.info_ptr, PNG_INFO_tRNS))
                src.alpha = true;

            auto dst_type = find_closest_pixel_type(src, allowed_pixel_types);
            if (!dst_type)
            {
                // Throws an exception listing the allowed types.
                check_pixel_type(pixel_type, allowed_pixel_types);
            }

            auto dst = *get_pixel_format(*dst_type);
            set_transformations(png, src, dst);
            png_set_interlace_handling(png.png_ptr);
            png_read_update_info(png.png_ptr, png.info_ptr);

            auto new_type = get_pixel_type(
                png_get_color_type(png.png_ptr, png.info_ptr),
                png_get_bit_depth(png.png_ptr, png.info_ptr));
            if (dst.alpha_first)
                new_type = to_alpha_first(new_type);

            if (new_type != *dst_type)
            {
                YIMAGE_THROW("Failed to convert PNG image from "
                    + to_string(pixel_type) + " to " + to_string(*dst_type)
                    + ". The result was " + to_string(new_type) + ".");
            }

            return new_type;
        }

        Image read_png(const PngHandle& png, std::span<const PixelType> allowed_pixel_types)
        {
            png_read_info(png.png_ptr, png.info_ptr);

            auto metadata = std::make_unique<PngMetadata>();
            metadata->width = png_get_image_width(png.png_ptr, png.info_ptr);
            metadata->height = png_get_image_height(png.png_ptr, png.info_ptr);
            metadata->bit_depth = png_get_bit_depth(png.png_ptr, png.info_ptr);
            metadata->color_type = png_get_color_type(png.png_ptr, png.info_ptr);
            //const auto channels = png_get_channels(png.png_ptr, png.info_ptr);

            auto pixel_type = set_transformations(png, allowed_pixel_types);
            Image image(pixel_type, metadata->width, metadata->height);

            std::vector<uint8_t*> row_pointers(metadata->height);
            for (size_t i = 0; i < metadata->height; ++i)
                row_pointers[i] = image.pixel_pointer(0, i);
            png_read_image(png.png_ptr, row_pointers.data());

            if (png_get_color_type(png.png_ptr, png.info_ptr) == PNG_COLOR_TYPE_PALETTE)
                image.palette() = get_palette(png);

            image.set_metadata(std::move(metadata));
            return image;
        }
    }

    Image read_png(std::istream& stream,
                   std::span<const PixelType> allowed_pixel_types)
    {
        auto png = create_png_handle();
        png_set_read_fn(png.png_ptr, &stream, user_read_istream_data);
        return read_png(png, allowed_pixel_types);
    }

    Image read_png(const std::filesystem::path& path,
                   std::span<const PixelType> allowed_pixel_types)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            YIMAGE_THROW("Can not open file: " + path.string());
        auto image = read_png(file, allowed_pixel_types);
        image.metadata()->path = path;
        return image;
    }

    Image read_png(const void* buffer, size_t size,
                   std::span<const PixelType> allowed_pixel_types)
    {
        auto png = create_png_handle();
        MemoryReader reader(buffer, size);
        png_set_read_fn(png.png_ptr, &reader, user_read_buffer_data);
        return read_png(png, allowed_pixel_types);
    }
}
