cmake_minimum_required (VERSION 3.7)
project (libjuice VERSION 1.1.0 LANGUAGES C)
set(PROJECT_DESCRIPTION "UDP Interactive Connectivity Establishment (ICE) library")

option(USE_NETTLE "Use Nettle for hash functions" OFF)
option(NO_SERVER "Disable server support" OFF)
option(NO_EXPORT_HEADER "Disable export header" OFF)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(FUZZER "Enable oss-fuzz fuzzing" OFF)
option(CLANG_TIDY "Enable clang-tidy" OFF)
if(NOT MSVC)
	set(CMAKE_C_FLAGS "-fPIC")
	set(CMAKE_CXX_FLAGS "-fPIC")
endif()

# Mitigations
option(ENABLE_LOCALHOST_ADDRESS "List localhost addresses in candidates" OFF)
option(ENABLE_LOCAL_ADDRESS_TRANSLATION "Translate local addresses to localhost" OFF)

set(C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
set(JUICE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../deps/libjuice")
if(WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	if (MSVC)
		add_definitions(-DNOMINMAX)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	endif()
endif()

if(CLANG_TIDY)
	set(CMAKE_C_CLANG_TIDY clang-tidy)
endif()

set(LIBJUICE_SOURCES
	${JUICE_PATH}/src/addr.c
	${JUICE_PATH}/src/agent.c
	${JUICE_PATH}/src/crc32.c
	${JUICE_PATH}/src/const_time.c
	${JUICE_PATH}/src/conn.c
	${JUICE_PATH}/src/conn_poll.c
	${JUICE_PATH}/src/conn_thread.c
	${JUICE_PATH}/src/conn_mux.c
	${JUICE_PATH}/src/base64.c
	${JUICE_PATH}/src/hash.c
	${JUICE_PATH}/src/hmac.c
	${JUICE_PATH}/src/ice.c
	${JUICE_PATH}/src/juice.c
	${JUICE_PATH}/src/log.c
	${JUICE_PATH}/src/random.c
	${JUICE_PATH}/src/server.c
	${JUICE_PATH}/src/stun.c
	${JUICE_PATH}/src/timestamp.c
	${JUICE_PATH}/src/turn.c
	${JUICE_PATH}/src/udp.c
)
source_group("Source Files" FILES "${LIBJUICE_SOURCES}")

set(LIBJUICE_HEADERS
	${JUICE_PATH}/include/juice/juice.h
)
source_group("Header Files" FILES "${LIBJUICE_HEADERS}")

set(FUZZER_SOURCES
    ${JUICE_PATH}/fuzzer/fuzzer.c
)
source_group("Fuzzer Files" FILES "${FUZZER_SOURCES}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(juice STATIC EXCLUDE_FROM_ALL ${LIBJUICE_SOURCES})
set_target_properties(juice PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(juice PUBLIC
    $<BUILD_INTERFACE:${JUICE_PATH}/include>
    $<INSTALL_INTERFACE:include>)
target_include_directories(juice PRIVATE ${JUICE_PATH}/include/juice)
target_include_directories(juice PRIVATE ${JUICE_PATH}/src)
target_compile_definitions(juice PRIVATE $<$<CONFIG:Release>:RELEASE=1>)
target_compile_definitions(juice PUBLIC JUICE_STATIC)
target_link_libraries(juice PRIVATE Threads::Threads)

if(WIN32)
	target_link_libraries(juice PRIVATE
		ws2_32 # winsock2
		bcrypt)
endif()


target_compile_definitions(juice PRIVATE USE_NETTLE=0)

if (NO_SERVER)
	target_compile_definitions(juice PRIVATE NO_SERVER)
endif()

if(APPLE)
	# This seems to be necessary on MacOS
	target_include_directories(juice PRIVATE /usr/local/include)
endif()

set_target_properties(juice PROPERTIES EXPORT_NAME LibJuice)
add_library(LibJuice::LibJuice ALIAS juice)

target_compile_definitions(juice PRIVATE JUICE_EXPORTS)

if(NOT MSVC)
	target_compile_options(juice PRIVATE -Wall -Wextra)
endif()

if(WARNINGS_AS_ERRORS)
	if(MSVC)
		target_compile_options(juice PRIVATE /WX)
	else()
		target_compile_options(juice PRIVATE -Werror)
	endif()
endif()

if(ENABLE_LOCALHOST_ADDRESS)
	target_compile_definitions(juice PRIVATE JUICE_ENABLE_LOCALHOST_ADDRESS=1)
endif()

if(ENABLE_LOCAL_ADDRESS_TRANSLATION)
	target_compile_definitions(juice PRIVATE JUICE_ENABLE_LOCAL_ADDRESS_TRANSLATION=1)
endif()


# Fuzzer
if(FUZZER)
	add_executable(stun-fuzzer ${FUZZER_SOURCES})
	target_include_directories(stun-fuzzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
	target_include_directories(stun-fuzzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/juice)

	set_target_properties(stun-fuzzer PROPERTIES OUTPUT_NAME fuzzer)
	target_link_libraries(stun-fuzzer juice ${LIB_FUZZING_ENGINE})
endif()
