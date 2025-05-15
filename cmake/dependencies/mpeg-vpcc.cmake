cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

find_package(Git REQUIRED)  # ensure GIT_EXECUTABLE is available

set(TBB_BUILD_SHARED ${BUILD_SHARED_LIBS} CACHE BOOL "TBB shared" FORCE)

if(BUILD_SHARED_LIBS)
    set(tbb_static_lib OFF)
else()
    set(tbb_static_lib ON)
endif()
set(TBB_BUILD_STATIC ${tbb_static_lib} CACHE BOOL "TBB static" FORCE)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.28")
FetchContent_Declare(
  tmc2
  GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
  GIT_TAG        release-v25.0
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/patches/tmc2-install.patch
  &&             ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/scripts/replace.sh <SOURCE_DIR>
  EXCLUDE_FROM_ALL
  )

FetchContent_MakeAvailable(tmc2)
else()
FetchContent_Declare(
  tmc2
  GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
  GIT_TAG        release-v25.0
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/patches/tmc2-install.patch
  &&             ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/scripts/replace.sh <SOURCE_DIR>
  )

  FetchContent_GetProperties(tmc2)
  if(NOT tmc2_POPULATED)
    FetchContent_Populate(tmc2)
    add_subdirectory(${tmc2_SOURCE_DIR} ${tmc2_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

