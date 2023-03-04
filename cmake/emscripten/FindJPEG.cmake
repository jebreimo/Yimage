#****************************************************************************
# Copyright © 2023 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2023-03-03.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
#****************************************************************************
cmake_minimum_required(VERSION 3.13)

## Defines the target JPEG::JPEG.

if (NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG INTERFACE IMPORTED)
    target_compile_options(JPEG::JPEG
        INTERFACE
            "SHELL:-s USE_LIBJPEG"
        )
    target_link_options(JPEG::JPEG
        INTERFACE
            "SHELL:-s USE_LIBJPEG"
        )
endif ()

set(JPEG_FOUND YES)
