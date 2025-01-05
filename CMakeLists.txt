# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-12-08.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)
project(Yimage VERSION 0.2.0)

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

option(YIMAGE_USE_LIBTIFF "Use libtiff" OFF)

if (EMSCRIPTEN)
    list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten)
endif ()

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if (YIMAGE_USE_LIBTIFF)
    find_package(TIFF REQUIRED COMPONENTS CXX)
endif ()

configure_file(src/Yimage/YimageVersion.hpp.in YimageVersion.hpp @ONLY)

add_library(Yimage
    include/Yimage/Image.hpp
    include/Yimage/ImageAlgorithms.hpp
    include/Yimage/ImageMetadata.hpp
    include/Yimage/ImageView.hpp
    include/Yimage/Jpeg/ReadJpeg.hpp
    include/Yimage/MutableImageView.hpp
    include/Yimage/PixelType.hpp
    include/Yimage/Png/PngMetadata.hpp
    include/Yimage/Png/PngTransform.hpp
    include/Yimage/Png/PngWriter.hpp
    include/Yimage/Png/ReadPng.hpp
    include/Yimage/Png/WritePng.hpp
    include/Yimage/ReadImage.hpp
    include/Yimage/Rgba8.hpp
    include/Yimage/Yimage.hpp
    include/Yimage/YimageException.hpp
    src/Yimage/ColorBytes.cpp
    src/Yimage/ColorBytes.hpp
    src/Yimage/Image.cpp
    src/Yimage/ImageAlgorithms.cpp
    src/Yimage/ImageMetadata.cpp
    src/Yimage/ImageUtilities.hpp
    src/Yimage/ImageView.cpp
    src/Yimage/Jpeg/ReadJpeg.cpp
    src/Yimage/MutableImageView.cpp
    src/Yimage/PixelType.cpp
    src/Yimage/Png/PngMetadata.cpp
    src/Yimage/Png/PngTransform.cpp
    src/Yimage/Png/PngWriter.cpp
    src/Yimage/Png/ReadPng.cpp
    src/Yimage/Png/WritePng.cpp
    src/Yimage/ReadImage.cpp
    src/Yimage/Rgba8.cpp
    src/Yimage/FileUtilities.hpp
    src/Yimage/ReadOnlyStreamBuffer.hpp
)

include(GNUInstallDirs)

target_include_directories(Yimage
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(Yimage
    PUBLIC
        JPEG::JPEG
        PNG::PNG
)

if (YIMAGE_USE_LIBTIFF)
    target_sources(Yimage PUBLIC
        include/Yimage/Tiff/GeoTiffMetadata.hpp
        include/Yimage/Tiff/ReadTiff.hpp
        include/Yimage/Tiff/TiffMetadata.hpp
    )

    target_sources(Yimage PRIVATE
        src/Yimage/Tiff/GeoTiffMetadata.cpp
        src/Yimage/Tiff/ReadGeoTiffMetadata.cpp
        src/Yimage/Tiff/ReadGeoTiffMetadata.hpp
        src/Yimage/Tiff/ReadTiff.cpp
        src/Yimage/Tiff/TiffMetadata.cpp
    )

    target_link_libraries(Yimage PUBLIC
        TIFF::CXX
    )
endif ()

add_library(Yimage::Yimage ALIAS Yimage)

if (YIMAGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/YimageTest)
endif ()

if (YIMAGE_BUILD_EXTRAS)
    add_subdirectory(extras)
endif()
