#****************************************************************************
# Copyright © 2023 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2023-03-04.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
#****************************************************************************
cmake_minimum_required(VERSION 3.13)

## Defines the target PNG::PNG.

if (NOT TARGET PNG::PNG)
    add_library(PNG::PNG INTERFACE IMPORTED)
    target_compile_options(PNG::PNG
        INTERFACE
        "SHELL:-s USE_LIBPNG"
        )
    target_link_options(PNG::PNG
        INTERFACE
        "SHELL:-s USE_LIBPNG"
        )
endif ()

set(PNG_FOUND YES)
