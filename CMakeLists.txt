cmake_minimum_required(VERSION 3.17)
project(frontier VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake")

set(FRONTIER_GAME_PLUGIN "game")
set(FRONTIER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(FRONTIER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FRONTIER_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FRONTIER_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FRONTIER_BINARY_DIR})

if (MSVC)
	add_compile_options(/FC)
endif()

add_subdirectory(src)

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

include(CTest)
enable_testing()

add_subdirectory(test)
