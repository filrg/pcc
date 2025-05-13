cmake_minimum_required(VERSION 3.21)

project(pccTests LANGUAGES C)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcc REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(pcc_test source/pcc_test.c)
target_link_libraries(pcc_test PRIVATE pcc::pcc)
target_compile_features(pcc_test PRIVATE c_std_99)

add_test(NAME pcc_test COMMAND pcc_test)

# ---- End-of-file commands ----

add_folders(Test)
