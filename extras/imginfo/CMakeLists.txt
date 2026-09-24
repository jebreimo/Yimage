# ===========================================================================
# Copyright © 2026 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2026-09-24.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(imginfo)

include(FetchContent)

FetchContent_Declare(argos
        GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
        GIT_TAG "v1.7.2")

FetchContent_MakeAvailable(argos)

add_executable(imginfo
        imginfo.cpp)

target_link_libraries(imginfo
        Argos::Argos
        Yimage::Yimage)
