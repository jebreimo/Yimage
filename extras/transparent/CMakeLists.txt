# ===========================================================================
# Copyright © 2022 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2022-01-08.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(transparent)

include(FetchContent)

FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG "v1.1.271")

FetchContent_MakeAvailable(argos)

add_executable(transparent
    transparent.cpp)

target_link_libraries(transparent
    Argos::Argos
    Yimage::Yimage
    )
