if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/pcc-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package pcc)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT pcc_Development
)

install(
    TARGETS pcc_pcc
    EXPORT pccTargets
    RUNTIME #
    COMPONENT pcc_Runtime
    LIBRARY #
    COMPONENT pcc_Runtime
    NAMELINK_COMPONENT pcc_Development
    ARCHIVE #
    COMPONENT pcc_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    pcc_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE pcc_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(pcc_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${pcc_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT pcc_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${pcc_INSTALL_CMAKEDIR}"
    COMPONENT pcc_Development
)

install(
    EXPORT pccTargets
    NAMESPACE pcc::
    DESTINATION "${pcc_INSTALL_CMAKEDIR}"
    COMPONENT pcc_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
