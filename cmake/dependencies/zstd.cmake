cmake_minimum_required(VERSION 3.21)

include(FetchContent)

if(NOT BUILD_SHARED_LIBS)
    set(ZSTD_BUILD_STATIC ON)
    set(ZSTD_BUILD_SHARED OFF)
    set(zstd_LIBS libzstd_static)
else()
    set(ZSTD_BUILD_STATIC OFF)
    set(ZSTD_BUILD_SHARED ON)
    set(zstd_LIBS libzstd)
endif()

set(ZSTD_MULTITHREAD_SUPPORT FALSE)
set(ZSTD_BUILD_PROGRAMS OFF)

FetchContent_Declare(
    zstd
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL "https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz"
    SOURCE_SUBDIR build/cmake
)

FetchContent_MakeAvailable(zstd)
