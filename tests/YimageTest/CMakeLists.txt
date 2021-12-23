# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-12-19.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(YimageTest)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v2.13.7"
    )
FetchContent_MakeAvailable(catch)

add_executable(YimageTest
    YimageTestMain.cpp test_MutableImageView.cpp)

target_link_libraries(YimageTest
    Catch2::Catch2
    Yimage::Yimage)

add_test(NAME Yimage COMMAND ${CMAKE_CURRENT_BINARY_DIR}/YimageTest)
