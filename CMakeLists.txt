# CMake build script for SimpleJPEGLibrary

cmake_minimum_required(VERSION 3.0)
project(simplejpeg)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g -ltcmalloc")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# Add sources
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp"
                  "${PROJECT_SOURCE_DIR}/JPEG/*.cpp"
                  "${PROJECT_SOURCE_DIR}/Encoder/*.cpp")
#                  "${PROJECT_SOURCE_DIR}/Decoder/*.cpp")

# Specify include directory
include_directories("${PROJECT_SOURCE_DIR}/JPEG/"
                    "${PROJECT_SOURCE_DIR}/Encoder/")
#include_directories("${PROJECT_SOURCE_DIR}/Encoder/")
#include_directories("${PROJECT_SOURCE_DIR}/Decoder/")

# Compile and generate the executable
add_executable(simplejpeg ${SOURCES})

set_property(TARGET simplejpeg PROPERTY CXX_STANDARD 14)
set_property(TARGET simplejpeg PROPERTY CXX_STANDARD_REQUIRED ON)
