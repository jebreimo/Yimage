# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-12-08.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(jpg2png)

include(FetchContent)

FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG "v1.6")

FetchContent_MakeAvailable(argos)

add_executable(jpg2png
    jpg2png.cpp)

target_link_libraries(jpg2png
    Argos::Argos
    Yimage::Yimage)
