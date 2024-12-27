# ===========================================================================
# Copyright © 2024 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2024-12-25.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(tiffreader)

add_executable(tiffreader
    tiffreader.cpp
)

target_link_libraries(tiffreader
    Yimage::Yimage
)
