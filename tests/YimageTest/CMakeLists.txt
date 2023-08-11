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
FetchContent_Declare(cppembed
    GIT_REPOSITORY "https://github.com/jebreimo/cppembed.git"
    GIT_TAG "main"
)
FetchContent_MakeAvailable(catch cppembed)

list(APPEND CMAKE_MODULE_PATH ${cppembed_SOURCE_DIR}/cmake)

include(TargetEmbedCppData)

add_executable(YimageTest
    YimageTestMain.cpp
    test_MutableImageView.cpp
    test_ImageView.cpp
    test_ReadPng.cpp
    Resources.hpp
    Resources.cpp
)

target_include_directories(YimageTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(YimageTest
    Catch2::Catch2
    Yimage::Yimage
)

target_embed_cpp_data(YimageTest
    FILES
        Images.hpp.in
)

add_test(NAME Yimage COMMAND ${CMAKE_CURRENT_BINARY_DIR}/YimageTest)
