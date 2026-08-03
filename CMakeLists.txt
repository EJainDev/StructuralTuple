cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS_EQUAL 4.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.4)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
endif()
set(CMAKE_CXX_MODULE_STD 1)

project(
  StructuralTuple
  VERSION 1.0.0.0
  LANGUAGES CXX)

option(BUILD_TESTS "Enable building of tests" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
endif()

# Library

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -freflection")

add_library(structural_tuple STATIC)
target_sources(
  structural_tuple
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}/include"
         FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/include/structural_tuple.cxx")
target_include_directories(
  structural_tuple PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:include>)
target_compile_features(structural_tuple PUBLIC cxx_std_26)

# Installation configuration

install(
  TARGETS structural_tuple
  EXPORT structural_tuple_targets
  FILE_SET modules
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/structural_tuple
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(StructuralTuple_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/StructuralTuple")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/StructuralTupleConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/StructuralTupleConfig.cmake"
  INSTALL_DESTINATION "${StructuralTuple_INSTALL_CMAKEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/StructuralTupleConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT structural_tuple_targets
  NAMESPACE structural_tuple::
  DESTINATION "${StructuralTuple_INSTALL_CMAKEDIR}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/StructuralTupleConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/StructuralTupleConfigVersion.cmake"
        DESTINATION "${StructuralTuple_INSTALL_CMAKEDIR}")

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
