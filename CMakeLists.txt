cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
    pcc
    VERSION 0.1.0
    DESCRIPTION "A point cloud compression tool"
    HOMEPAGE_URL "https://filrg.github.io/pcc"
    LANGUAGES C
)

include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    pcc_pcc
    source/pcc.c
)
add_library(pcc::pcc ALIAS pcc_pcc)

include(GenerateExportHeader)
generate_export_header(
    pcc_pcc
    BASE_NAME pcc
    EXPORT_FILE_NAME export/pcc/pcc_export.h
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(pcc_pcc PUBLIC PCC_STATIC_DEFINE)
endif()

set_target_properties(
    pcc_pcc PROPERTIES
    C_VISIBILITY_PRESET hidden
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME pcc
    OUTPUT_NAME pcc
)

target_include_directories(
    pcc_pcc ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    pcc_pcc SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(pcc_pcc PUBLIC c_std_99)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${pcc_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT pcc_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of pcc"
  )
endif()

include(cmake/dev-mode.cmake)
