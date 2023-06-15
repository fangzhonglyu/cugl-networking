# Copyright 2019 Joe Drago. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.5)

# Specify search path for CMake modules to be loaded by include()
# and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

project(libavif LANGUAGES C VERSION 0.10.1)

# Set C99 as the default
set(CMAKE_C_STANDARD 99)

# SOVERSION scheme: MAJOR.MINOR.PATCH
#   If there was an incompatible interface change:
#     Increment MAJOR. Set MINOR and PATCH to 0
#   If there was a compatible interface change:
#     Increment MINOR. Set PATCH to 0
#   If the source code was changed, but there were no interface changes:
#     Increment PATCH.
set(LIBRARY_VERSION_MAJOR 14)
set(LIBRARY_VERSION_MINOR 0)
set(LIBRARY_VERSION_PATCH 1)
set(LIBRARY_VERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}.${LIBRARY_VERSION_PATCH}")
set(LIBRARY_SOVERSION ${LIBRARY_VERSION_MAJOR})

# Inherited from above
#option(BUILD_SHARED_LIBS "Build shared avif library" OFF)

option(AVIF_ENABLE_WERROR "Treat all compiler warnings as errors" ON)
option(AVIF_LOCAL_DAV1D "Build the dav1d codec by providing your own copy of the repo in ../dav1d (see Local Builds in README)" OFF)

if(AVIF_LOCAL_LIBGAV1)
    enable_language(CXX)
endif()

if(APPLE)
    set(XCRUN xcrun)
else()
    set(XCRUN)
endif()

# ---------------------------------------------------------------------------------------

# Enable all warnings
include(CheckCCompilerFlag)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    MESSAGE(STATUS "libavif: Enabling warnings for Clang")
    add_definitions(
        -Weverything
        -Wno-bad-function-cast
        -Wno-cast-align
        -Wno-conversion
        -Wno-covered-switch-default
        -Wno-disabled-macro-expansion
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-double-promotion
        -Wno-float-equal
        -Wno-missing-noreturn
        -Wno-padded
        -Wno-sign-conversion
        -Wno-error=c11-extensions
    )
    # Some headers such as glib-2.0 use identifier names that start with '_' followed by a capital letter.
    # The C standard reserves names beginning with an underscore and various other combinations.
    # ISO/IEC 9899:1999 (aka C99 standard) 7.1.3 Reserved identifiers
    check_c_compiler_flag(-Wreserved-identifier HAVE_RESERVED_IDENTIFIER_WARNING)
    if(HAVE_RESERVED_IDENTIFIER_WARNING)
        add_definitions(-Wno-reserved-identifier)
    endif()
    # Clang >= 14 warns that mixing declarations and code is incompatible with standards before C99, even if
    # you compile with -std=c99 or -std=gnu99.
    check_c_compiler_flag(-Wdeclaration-after-statement HAVE_DECLARATION_AFTER_STATEMENT_WARNING)
    if(HAVE_DECLARATION_AFTER_STATEMENT_WARNING)
        add_definitions(-Wno-declaration-after-statement)
    endif()
    # The detection of cross compilation by -Wpoison-system-directories has false positives on macOS because
    # --sysroot is implicitly added. Turn the warning off.
    if(NOT DEFINED HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
        check_c_compiler_flag(-Wpoison-system-directories HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
    endif()
    if(HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
        add_definitions(-Wno-poison-system-directories)
    endif()
    # MINGW declares printf with __attribute__ ((__unused__)) in stdio.h.
    # It is out of our control so we just ignore it.
    # See https://sourceforge.net/p/mingw-w64/bugs/868/
    if(MINGW)
        add_definitions(-Wno-used-but-marked-unused)
    endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
    MESSAGE(STATUS "libavif: Enabling warnings for GCC")
    add_definitions(-Wall -Wextra)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    MESSAGE(STATUS "libavif: Enabling warnings for MSVC")
    add_definitions(
        /Wall   # All warnings
        /wd4255 # Disable: no function prototype given
        /wd4324 # Disable: structure was padded due to alignment specifier
        /wd4668 # Disable: is not defined as a preprocessor macro, replacing with '0'
        /wd4710 # Disable: function not inlined
        /wd4711 # Disable: function selected for inline expansion
        /wd4738 # Disable: storing 32-bit float result in memory, possible loss of performance
        /wd4820 # Disable: bytes padding added after data member
        /wd4996 # Disable: potentially unsafe stdlib methods
        /wd5045 # Disable: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

        # This tells MSVC to read source code as UTF-8 and assume console can only use ASCII (minimal safe).
        # libavif uses ANSI API to print to console, which is not portable between systems using different
        # languages and results in mojibake unless we only use codes shared by every code page: ASCII.
        # A C4556 warning will be generated on violation.
        # Commonly used /utf-8 flag assumes UTF-8 for both source and console, which is usually not the case.
        # Warnings can be suppressed but there will still be random characters printed to the console.
        /source-charset:utf-8 /execution-charset:us-ascii
    )
else()
    MESSAGE(FATAL_ERROR "libavif: Unknown compiler, bailing out")
endif()

if(AVIF_ENABLE_WERROR)
    # Warnings as errors
    if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
        add_definitions(-Werror)
    elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        add_definitions(/WX)
    else()
        MESSAGE(FATAL_ERROR "libavif: Unknown compiler, bailing out")
    endif()
endif()

if(AVIF_ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
        MESSAGE(STATUS "libavif: Enabling coverage for Clang")
        add_definitions(-fprofile-instr-generate -fcoverage-mapping -O0)
        set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-fprofile-instr-generate -fcoverage-mapping")
    else()
        # TODO: Add support for other compilers
        message(WARNING "libavif: Ignoring request for coverage (AVIF_ENABLE_COVERAGE); only clang is currently supported.")
    endif()
endif()

set(AVIF_SRCS
    src/alpha.c
    src/avif.c
    src/colr.c
    src/diag.c
    src/io.c
    src/mem.c
    src/obu.c
    src/rawdata.c
    src/read.c
    src/reformat.c
    src/reformat_libyuv.c
    src/scale.c
    src/stream.c
    src/utils.c
    src/write.c
)

# Add the DAV1D includes
list(APPEND DAV1D_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../dav1d")
list(APPEND DAV1D_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../dav1d/include")
list(APPEND DAV1D_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../dav1d/arch")
list(APPEND DAV1D_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../dav1d/arch/${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(AVIF_PLATFORM_DEFINITIONS)
set(AVIF_PLATFORM_INCLUDES)
set(AVIF_PLATFORM_LIBRARIES)
if(UNIX)
    # Find out if we have threading available
    set(CMAKE_THREAD_PREFER_PTHREADS ON)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)
    set(AVIF_PLATFORM_LIBRARIES m Threads::Threads)
endif()

# This version ALWAYS uses dav1d
set(AVIF_CODEC_DEFINITIONS)
set(AVIF_CODEC_INCLUDES)
set(AVIF_CODEC_LIBRARIES)
set(AVIF_CODEC_DEFINITIONS ${AVIF_CODEC_DEFINITIONS} -DAVIF_CODEC_DAV1D=1)
set(AVIF_SRCS ${AVIF_SRCS}
	src/codec_dav1d.c
)

if(AVIF_LOCAL_DAV1D)
	set(LIB_FILENAME "${CMAKE_CURRENT_SOURCE_DIR}/ext/dav1d/build/src/libdav1d.a")
	if(NOT EXISTS "${LIB_FILENAME}")
		message(FATAL_ERROR "libavif: ${LIB_FILENAME} is missing, bailing out")
	endif()

	set(AVIF_CODEC_INCLUDES ${AVIF_CODEC_INCLUDES}
		"${CMAKE_CURRENT_SOURCE_DIR}/../dav1d"
		"${CMAKE_CURRENT_SOURCE_DIR}/../dav1d/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../dav1d/arch"
		"${CMAKE_CURRENT_SOURCE_DIR}/../dav1d/arch/${CMAKE_HOST_SYSTEM_PROCESSOR}"
	)
	set(AVIF_CODEC_LIBRARIES ${AVIF_CODEC_LIBRARIES} ${LIB_FILENAME})
else()
	# Check to see if dav1d is independently being built by the outer CMake project
	if(NOT TARGET dav1d)
		find_package(dav1d REQUIRED)
	endif()
	set(AVIF_CODEC_INCLUDES ${AVIF_CODEC_INCLUDES} ${DAV1D_INCLUDE_DIR})
	set(AVIF_CODEC_LIBRARIES ${AVIF_CODEC_LIBRARIES} ${DAV1D_LIBRARY})
endif()

if(UNIX AND NOT APPLE)
	set(AVIF_PLATFORM_LIBRARIES ${AVIF_PLATFORM_LIBRARIES} ${CMAKE_DL_LIBS}) # for dlsym
endif()
message(STATUS "libavif: Codec enabled: dav1d (decode)")


add_library(avif STATIC ${AVIF_SRCS})
set_target_properties(avif
                      PROPERTIES
                          VERSION ${LIBRARY_VERSION}
                          SOVERSION ${LIBRARY_SOVERSION}
                          C_VISIBILITY_PRESET hidden)
target_compile_definitions(avif
                           PRIVATE ${AVIF_PLATFORM_DEFINITIONS} ${AVIF_CODEC_DEFINITIONS})
target_link_libraries(avif
                      PRIVATE ${AVIF_CODEC_LIBRARIES} ${AVIF_PLATFORM_LIBRARIES})
target_include_directories(avif
                           PUBLIC $<BUILD_INTERFACE:${libavif_SOURCE_DIR}/include>
                                  $<INSTALL_INTERFACE:include>
                           PRIVATE ${AVIF_PLATFORM_INCLUDES} ${AVIF_CODEC_INCLUDES})
set(AVIF_PKG_CONFIG_EXTRA_CFLAGS "")
if(BUILD_SHARED_LIBS)
    target_compile_definitions(avif PUBLIC AVIF_DLL
                                    PRIVATE AVIF_BUILDING_SHARED_LIBS)
    set(AVIF_PKG_CONFIG_EXTRA_CFLAGS " -DAVIF_DLL")
    if(AVIF_LOCAL_LIBGAV1)
        set_target_properties(avif PROPERTIES LINKER_LANGUAGE "CXX")
    endif()
endif()

if(CMAKE_SKIP_INSTALL_RULES)
    set(SKIP_INSTALL_ALL TRUE)
endif()

if(NOT SKIP_INSTALL_ALL)
    include(GNUInstallDirs)
endif()

if(AVIF_CODEC_LIBRARIES MATCHES vmaf)
    enable_language(CXX)
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS avif
        EXPORT ${PROJECT_NAME}-config
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )

    # Enable CMake configs in VCPKG mode
    if (BUILD_SHARED_LIBS OR VCPKG_TARGET_TRIPLET)
        install(EXPORT ${PROJECT_NAME}-config
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

        include(CMakePackageConfigHelpers)
        write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake
                                         VERSION ${PROJECT_VERSION}
                                         COMPATIBILITY SameMajorVersion)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
    endif()

    configure_file(libavif.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libavif.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libavif.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
    install(FILES include/avif/avif.h
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/avif"
    )
endif()

# ---------------------------------------------------------------------------------------
# Win32 (Visual Studio) fixups

macro(avif_set_folder_safe target folder)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES FOLDER ${folder})
    endif()
endmacro()

macro(avif_exclude_safe target)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD True)
    endif()
endmacro()

if(WIN32)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    avif_set_folder_safe(avif "ext/avif")
    if(AVIF_BUILD_EXAMPLES)
        foreach(EXAMPLE ${AVIF_EXAMPLES})
            avif_set_folder_safe(${EXAMPLE} "ext/avif/examples")
        endforeach()
    endif()
    if(AVIF_LOCAL_ZLIBPNG)
        avif_set_folder_safe(example "ext/zlibpng")
        avif_set_folder_safe(genfiles "ext/zlibpng")
        avif_set_folder_safe(minigzip "ext/zlibpng")
        avif_set_folder_safe(png_static "ext/zlibpng")
        avif_set_folder_safe(zlib "ext/zlibpng")
        avif_set_folder_safe(zlibstatic "ext/zlibpng")

        # Don't bother building these targets
        avif_exclude_safe(example)
        avif_exclude_safe(genfiles)
        avif_exclude_safe(minigzip)
    endif()
    if(AVIF_LOCAL_JPEG)
        avif_set_folder_safe(jpeg "ext/libjpeg")
    endif()
endif()

add_subdirectory(contrib)
