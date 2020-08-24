cmake_minimum_required(VERSION 3.17)

# Setup project

project(frontier VERSION 0.0.1)

# Define compiler options

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (MSVC)
	add_compile_options(/FC)
endif()

# Find packages

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)

find_package(mimalloc 1.6 CONFIG REQUIRED)
find_path(SOKOL_INCLUDE_DIRS "sokol_app.h")

# Setup include and source directories

set(FRONTIER_GAME_PLUGIN "game")
set(FRONTIER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(FRONTIER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FRONTIER_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

list(APPEND FRONTIER_INCLUDE_DIRS ${FRONTIER_INCLUDE_DIR} ${SOKOL_INCLUDE_DIRS})

# Setup output directories

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FRONTIER_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FRONTIER_BINARY_DIR})

# Setup targets

add_subdirectory(src)

# Setup tests

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

include(CTest)
enable_testing()

add_subdirectory(test)
