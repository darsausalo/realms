cmake_minimum_required(VERSION 3.17)
project(frontier VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake")

set(FRONTIER_GAME_PLUGIN "game")
set(FRONTIER_MAIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(FRONTIER_MAIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FRONTIER_DEPLOY_DIR ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FRONTIER_DEPLOY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FRONTIER_DEPLOY_DIR})

add_subdirectory(src)

include(CTest)
enable_testing()

add_subdirectory(test)
