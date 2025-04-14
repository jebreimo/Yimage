# ===========================================================================
# Copyright © 2024 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2024-12-25.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(float_image_to_png)

include(FetchContent)

FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG "v1.6")

FetchContent_MakeAvailable(argos)

add_executable(float_image_to_png
    float_image_to_png.cpp
)

target_link_libraries(float_image_to_png
    Argos::Argos
    Yimage::Yimage
)
