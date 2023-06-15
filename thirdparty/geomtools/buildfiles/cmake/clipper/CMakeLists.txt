cmake_minimum_required(VERSION 3.12)

# A stripped down version of clipper for CUGL
project(polyclipping LANGUAGES CXX VERSION 6.4.1)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Release type")
set(PCFILE "${CMAKE_CURRENT_BINARY_DIR}/polyclipping.pc")
set(BUILD_SHARED_LIBS OFF CACHE BOOL
    "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)")

# Indirection
set(PCKG_DIR  "${PROJECT_SOURCE_DIR}/../../../clipper")

add_library(polyclipping ${PCKG_DIR}/clipper.cpp)
configure_file(${PCKG_DIR}/polyclipping.pc.cmakein "${PCFILE}" @ONLY)
