cmake_minimum_required(VERSION 3.12)

# A stripped down version of poly2tri for CUGL
project(poly2tri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

option(P2T_BUILD_TESTS "Build tests" OFF)
option(P2T_BUILD_TESTBED "Build the testbed application" OFF)

# Indirection
set(PCKG_DIR  "${PROJECT_SOURCE_DIR}/../../../poly2tri")

# Source Code
file(GLOB SOURCES 
	${PCKG_DIR}/poly2tri/common/*.cc 
	${PCKG_DIR}/poly2tri/sweep/*.cc
	)
	
# Headers Code
file(GLOB HEADERS 
	${PCKG_DIR}/poly2tri/*.h 
	${PCKG_DIR}/poly2tri/common/*.h 
	${PCKG_DIR}/poly2tri/sweep/*.h)

add_library(poly2tri ${SOURCES} ${HEADERS})

target_include_directories(poly2tri INTERFACE ${PCKG_DIR}/include)
target_compile_definitions(poly2tri PUBLIC P2T_STATIC_EXPORTS)
