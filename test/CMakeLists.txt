cmake_minimum_required(VERSION 3.21)

project(pccTests LANGUAGES C)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcc REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

set(ASSETS ${CMAKE_SOURCE_DIR}/assets)

add_test(NAME pcc_encode COMMAND encode ${ASSETS}/longdress0000.ply 2 longdress0000.bin)
add_test(NAME pcc_decode COMMAND decode ${ASSETS}/longdress0000.bin longdress0000_rec.ply 1)

# ---- End-of-file commands ----

add_folders(Test)
