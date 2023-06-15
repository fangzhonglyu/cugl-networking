cmake_minimum_required(VERSION 3.0)

project(plog LANGUAGES CXX)
if(NOT MSVC)
    set(CMAKE_C_FLAGS "-fPIC")
    set(CMAKE_CXX_FLAGS "-fPIC")
endif()
set(PLOG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../deps/plog")

# Make sure install paths work on all platforms.
if(NOT CMAKE_INSTALL_INCLUDEDIR)
    include(GNUInstallDirs)
endif()

add_library(plog INTERFACE)
target_include_directories(plog
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
    $<BUILD_INTERFACE:${PLOG_PATH}/include>
)

add_library(plog::plog ALIAS plog)

#making sure we can build standalone under windows
get_filename_component(CURRENT_SOURCE_DIR ${PLOG_PATH}   ABSOLUTE)
get_filename_component(SOURCE_DIR ${CMAKE_SOURCE_DIR} ABSOLUTE)
