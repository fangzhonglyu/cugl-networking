# Copyright (c) 2012, Intel Corporation
# Copyright (c) 2019 Sony Interactive Entertainment Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# * Neither the name of Intel Corporation nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Try to find Harfbuzz include and library directories.
#
# After successful discovery, this will set for inclusion where needed:
# harfbuzz_INCLUDE_DIRS - containg the harfbuzz headers
# harfbuzz_LIBRARIES - containg the harfbuzz library

#[=======================================================================[.rst:
Findharfbuzz
--------------

Find harfbuzz headers and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

``harfbuzz::harfbuzz``
  The harfbuzz library, if found.

``harfbuzz::icu``
  The harfbuzz ICU library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``harfbuzz_FOUND``
  true if (the requested version of) harfbuzz is available.
``harfbuzz_VERSION``
  the version of harfbuzz.
``harfbuzz_LIBRARIES``
  the libraries to link against to use harfbuzz.
``harfbuzz_INCLUDE_DIRS``
  where to find the harfbuzz headers.
``harfbuzz_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_HARFBUZZ QUIET harfbuzz)
set(harfbuzz_COMPILE_OPTIONS ${PC_HARFBUZZ_CFLAGS_OTHER})
set(harfbuzz_VERSION ${PC_HARFBUZZ_CFLAGS_VERSION})

find_path(harfbuzz_INCLUDE_DIR
    NAMES hb.h
    HINTS ${PC_HARFBUZZ_INCLUDEDIR} ${PC_HARFBUZZ_INCLUDE_DIRS}
    PATH_SUFFIXES harfbuzz
)

find_library(harfbuzz_LIBRARY
    NAMES ${harfbuzz_NAMES} harfbuzz
    HINTS ${PC_HARFBUZZ_LIBDIR} ${PC_HARFBUZZ_LIBRARY_DIRS}
)

if (harfbuzz_INCLUDE_DIR AND NOT harfbuzz_VERSION)
    if (EXISTS "${harfbuzz_INCLUDE_DIR}/hb-version.h")
        file(READ "${harfbuzz_INCLUDE_DIR}/hb-version.h" _harfbuzz_version_content)

        string(REGEX MATCH "#define +HB_VERSION_STRING +\"([0-9]+\\.[0-9]+\\.[0-9]+)\"" _dummy "${_harfbuzz_version_content}")
        set(harfbuzz_VERSION "${CMAKE_MATCH_1}")
    endif ()
endif ()

if ("${harfbuzz_FIND_VERSION}" VERSION_GREATER "${harfbuzz_VERSION}")
  if (harfbuzz_FIND_REQUIRED)
    message(FATAL_ERROR
      "Required version (" ${harfbuzz_FIND_VERSION} ")"
      " is higher than found version (" ${harfbuzz_VERSION} ")")
  else ()
    message(WARNING
      "Required version (" ${harfbuzz_FIND_VERSION} ")"
      " is higher than found version (" ${harfbuzz_VERSION} ")")
    unset(harfbuzz_VERSION)
    unset(harfbuzz_INCLUDE_DIRS)
    unset(harfbuzz_LIBRARIES)
    return ()
  endif ()
endif ()

# Find components
if (harfbuzz_INCLUDE_DIR AND harfbuzz_LIBRARY)
    set(_harfbuzz_REQUIRED_LIBS_FOUND ON)
    set(harfbuzz_LIBS_FOUND "harfbuzz (required): ${harfbuzz_LIBRARY}")
else ()
    set(_harfbuzz_REQUIRED_LIBS_FOUND OFF)
    set(harfbuzz_LIBS_NOT_FOUND "harfbuzz (required)")
endif ()

if (NOT CMAKE_VERSION VERSION_LESS 3.3)
  if ("ICU" IN_LIST harfbuzz_FIND_COMPONENTS)
      pkg_check_modules(PC_HARFBUZZ_ICU QUIET harfbuzz-icu)
      set(harfbuzz_ICU_COMPILE_OPTIONS ${PC_HARFBUZZ_ICU_CFLAGS_OTHER})

      find_path(harfbuzz_ICU_INCLUDE_DIR
          NAMES hb-icu.h
          HINTS ${PC_HARFBUZZ_ICU_INCLUDEDIR} ${PC_HARFBUZZ_ICU_INCLUDE_DIRS}
          PATH_SUFFIXES harfbuzz
      )

      find_library(harfbuzz_ICU_LIBRARY
          NAMES ${harfbuzz_ICU_NAMES} harfbuzz-icu
          HINTS ${PC_HARFBUZZ_ICU_LIBDIR} ${PC_HARFBUZZ_ICU_LIBRARY_DIRS}
      )

      if (harfbuzz_ICU_LIBRARY)
          if (harfbuzz_FIND_REQUIRED_ICU)
              list(APPEND harfbuzz_LIBS_FOUND "ICU (required): ${harfbuzz_ICU_LIBRARY}")
          else ()
            list(APPEND harfbuzz_LIBS_FOUND "ICU (optional): ${harfbuzz_ICU_LIBRARY}")
          endif ()
      else ()
          if (harfbuzz_FIND_REQUIRED_ICU)
            set(_harfbuzz_REQUIRED_LIBS_FOUND OFF)
            list(APPEND harfbuzz_LIBS_NOT_FOUND "ICU (required)")
          else ()
            list(APPEND harfbuzz_LIBS_NOT_FOUND "ICU (optional)")
          endif ()
      endif ()
  endif ()
endif ()

if (NOT harfbuzz_FIND_QUIETLY)
    if (harfbuzz_LIBS_FOUND)
        message(STATUS "Found the following harfbuzz libraries:")
        foreach (found ${harfbuzz_LIBS_FOUND})
            message(STATUS " ${found}")
        endforeach ()
    endif ()
    if (harfbuzz_LIBS_NOT_FOUND)
        message(STATUS "The following harfbuzz libraries were not found:")
        foreach (found ${harfbuzz_LIBS_NOT_FOUND})
            message(STATUS " ${found}")
        endforeach ()
    endif ()
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(harfbuzz
    FOUND_VAR harfbuzz_FOUND
    REQUIRED_VARS harfbuzz_INCLUDE_DIR harfbuzz_LIBRARY _harfbuzz_REQUIRED_LIBS_FOUND
    VERSION_VAR harfbuzz_VERSION
)

if (NOT CMAKE_VERSION VERSION_LESS 3.1)
  if (harfbuzz_LIBRARY AND NOT TARGET harfbuzz::harfbuzz)
      add_library(harfbuzz::harfbuzz UNKNOWN IMPORTED GLOBAL)
      set_target_properties(harfbuzz::harfbuzz PROPERTIES
          IMPORTED_LOCATION "${harfbuzz_LIBRARY}"
          INTERFACE_COMPILE_OPTIONS "${harfbuzz_COMPILE_OPTIONS}"
          INTERFACE_INCLUDE_DIRECTORIES "${harfbuzz_INCLUDE_DIR}"
      )
  endif ()

  if (harfbuzz_ICU_LIBRARY AND NOT TARGET harfbuzz::icu)
      add_library(harfbuzz::icu UNKNOWN IMPORTED GLOBAL)
      set_target_properties(harfbuzz::icu PROPERTIES
          IMPORTED_LOCATION "${harfbuzz_ICU_LIBRARY}"
          INTERFACE_COMPILE_OPTIONS "${harfbuzz_ICU_COMPILE_OPTIONS}"
          INTERFACE_INCLUDE_DIRECTORIES "${harfbuzz_ICU_INCLUDE_DIR}"
      )
  endif ()
endif ()

mark_as_advanced(
    harfbuzz_INCLUDE_DIR
    harfbuzz_ICU_INCLUDE_DIR
    harfbuzz_LIBRARY
    harfbuzz_ICU_LIBRARY
)

if (harfbuzz_FOUND)
   set(harfbuzz_LIBRARIES ${harfbuzz_LIBRARY} ${harfbuzz_ICU_LIBRARY})
   set(harfbuzz_INCLUDE_DIRS ${harfbuzz_INCLUDE_DIR} ${harfbuzz_ICU_INCLUDE_DIR})
endif ()
