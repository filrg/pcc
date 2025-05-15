cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

find_package(Git REQUIRED)  # ensure GIT_EXECUTABLE is available

FetchContent_Declare(
  draco
  GIT_REPOSITORY https://github.com/google/draco.git
  GIT_TAG        1.5.7
  )

FetchContent_MakeAvailable(draco)
