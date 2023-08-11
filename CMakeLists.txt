# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-12-08.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(Yimage VERSION 0.1.5)

set(CMAKE_CXX_STANDARD 20)

# Set YIMAGE_MASTER_PROJECT to ON if Yimage is the root level project.
if (NOT DEFINED YIMAGE_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(YIMAGE_MASTER_PROJECT ON)
    else ()
        set(YIMAGE_MASTER_PROJECT OFF)
    endif ()
endif ()

option(YIMAGE_BUILD_EXTRAS "Build apps in the extras dir" ${YIMAGE_MASTER_PROJECT})

option(YIMAGE_BUILD_TESTS "Build unit tests" ${YIMAGE_MASTER_PROJECT})

option(YIMAGE_INSTALL "Generate the install target" ${YIMAGE_MASTER_PROJECT})

if (EMSCRIPTEN)
    list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten)
endif ()

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

add_library(Yimage
    include/Yimage/Image.hpp
    include/Yimage/ImageView.hpp
    include/Yimage/MutableImageView.hpp
    include/Yimage/PixelType.hpp
    include/Yimage/PngInfo.hpp
    include/Yimage/PngTransform.hpp
    include/Yimage/PngWriter.hpp
    include/Yimage/ReadImage.hpp
    include/Yimage/ReadJpeg.hpp
    include/Yimage/ReadPng.hpp
    include/Yimage/WritePng.hpp
    include/Yimage/Yimage.hpp
    include/Yimage/YimageException.hpp
    src/Yimage/Image.cpp
    src/Yimage/ImageView.cpp
    src/Yimage/MutableImageView.cpp
    src/Yimage/PixelType.cpp
    src/Yimage/PngInfo.cpp
    src/Yimage/PngTransform.cpp
    src/Yimage/PngWriter.cpp
    src/Yimage/ReadImage.cpp
    src/Yimage/ReadJpeg.cpp
    src/Yimage/ReadPng.cpp
    src/Yimage/WritePng.cpp
    src/Yimage/Rgba8.cpp include/Yimage/Rgba8.hpp src/Yimage/ImageUtilities.hpp)

include(GNUInstallDirs)

target_include_directories(Yimage
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_link_libraries(Yimage
    PUBLIC
        JPEG::JPEG
        PNG::PNG
    )

add_library(Yimage::Yimage ALIAS Yimage)

if (YIMAGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/YimageTest)
endif ()

if (YIMAGE_BUILD_EXTRAS)
    add_subdirectory(extras/jpg2png)
    add_subdirectory(extras/transparent)
endif()

if (YIMAGE_MASTER_PROJECT AND DEFINED ENV{LOCAL_SOURCE_PACKAGE_DIR})
    set(LOCAL_SOURCE_PACKAGE_DIR $ENV{LOCAL_SOURCE_PACKAGE_DIR})
    set(TARBALL_NAME ${LOCAL_SOURCE_PACKAGE_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.tar.gz)
    add_custom_target(${PROJECT_NAME}_BUILD_SRC_TARBALL
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND git archive -v --format=tar.gz --prefix=${PROJECT_NAME}/ HEAD > ${TARBALL_NAME}
        )
endif ()
