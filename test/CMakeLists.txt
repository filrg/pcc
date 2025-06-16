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
add_test(NAME pcc_multiple_encode COMMAND multiple_encode 9 longdress.bin ${ASSETS}/longdress0000.ply ${ASSETS}/longdress0001.ply ${ASSETS}/longdress0002.ply)
add_test(NAME pcc_multiple_decode COMMAND multiple_decode ${ASSETS}/longdress.bin rec%04d.ply)
# ---- End-of-file commands ----

add_folders(Test)
