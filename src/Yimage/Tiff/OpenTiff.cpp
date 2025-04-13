//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OpenTiff.hpp"

#include <iostream>
#include <tiffio.h>
#include "ReadGeoTiffMetadata.hpp"

// This file is based on the TIFFOpenStream function from the TIFF library.

namespace
{
    class TiffInputStream
    {
    public:
        explicit TiffInputStream(std::istream& stream)
            : stream_(&stream),
              start_pos_(stream.tellg())
        {
        }

        tmsize_t read(void* buf, tmsize_t size)
        {
            std::streamsize request_size = size;
            if (static_cast<tmsize_t>(request_size) != size)
                return -1;

            stream_->read((char*)buf, request_size);

            return stream_->gcount();
        }

        toff_t seek(toff_t off, int whence)
        {
            switch (whence)
            {
            case SEEK_SET:
            {
                toff_t new_offset = static_cast<toff_t>(start_pos_) + off;

                // Verify that value does not overflow
                auto offset = static_cast<std::ios::off_type>(new_offset);
                if (static_cast<toff_t>(offset) != new_offset)
                    return static_cast<toff_t>(-1);

                stream_->seekg(offset, std::ios::beg);
                break;
            }
            case SEEK_CUR:
            {
                // Verify that value does not overflow
                auto offset = static_cast<std::ios::off_type>(off);
                if (static_cast<toff_t>(offset) != off)
                    return static_cast<toff_t>(-1);

                stream_->seekg(offset, std::ios::cur);
                break;
            }
            case SEEK_END:
            {
                // Verify that value does not overflow
                auto offset = static_cast<std::ios::off_type>(off);
                if (static_cast<toff_t>(offset) != off)
                    return static_cast<toff_t>(-1);

                stream_->seekg(offset, std::ios::end);
                break;
            }
            }

            return static_cast<toff_t>(stream_->tellg());
        }

        [[nodiscard]] toff_t size() const
        {
            std::ios::pos_type pos = stream_->tellg();
            stream_->seekg(0, std::ios::end);
            std::ios::pos_type size = stream_->tellg();
            stream_->seekg(pos);
            return size;
        }

    private:
        std::istream* stream_;
        std::ios::pos_type start_pos_;
    };

    bool has_registered_extra_tags = false;
}

extern "C" {
static tmsize_t tiff_read_proc(thandle_t fd, void* buf, tmsize_t size)
{
    auto* reader = static_cast<TiffInputStream*>(fd);
    return reader->read(buf, size);
}

static tmsize_t tiff_write_proc(thandle_t, void*, tmsize_t)
{
    return 0;
}

static toff_t tiff_seek_proc(thandle_t fd, toff_t off, int whence)
{
    auto* reader = static_cast<TiffInputStream*>(fd);
    return reader->seek(off, whence);
}

static toff_t tiff_size_proc(thandle_t fd)
{
    auto* reader = static_cast<TiffInputStream*>(fd);
    return reader->size();
}

static int tiff_close_proc(thandle_t fd)
{
    delete static_cast<TiffInputStream*>(fd);
    return 0;
}

static int tiff_dummy_map_proc(thandle_t, void** /*base*/, toff_t* /*size*/)
{
    return (0);
}

static void tiff_dummy_unmap_proc(thandle_t, void* /*base*/, toff_t /*size*/)
{
}

static void register_extra_tags(TIFF* tiff)
{
    Yimage::register_geotiff_tags(tiff);
}
}

namespace Yimage
{
    void TiffDeleter::operator()(TIFF* tiff) const
    {
        TIFFClose(tiff);
    }

    std::unique_ptr<TIFF, TiffDeleter>
    open_tiff(std::istream& is, const char* name)
    {
        if (!has_registered_extra_tags)
        {
            TIFFSetTagExtender(register_extra_tags);
            has_registered_extra_tags = true;
        }
        auto* reader = new TiffInputStream(is);
        std::unique_ptr<TIFF, TiffDeleter> tiff(TIFFClientOpen(
            name, "rm", reader, tiff_read_proc,
            tiff_write_proc, tiff_seek_proc, tiff_close_proc,
            tiff_size_proc, tiff_dummy_map_proc, tiff_dummy_unmap_proc));

        if (!tiff)
            delete reader;

        return tiff;
    }
}
