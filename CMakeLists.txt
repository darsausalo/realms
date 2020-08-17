cmake_minimum_required(VERSION 3.17)
project(frontier CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(CR_INCLUDE_DIRS "cr.h")
find_package(entt CONFIG REQUIRED)

set(FRONTIER_MAIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(FRONTIER_MAIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FRONTIER_DEPLOY_DIR ${CMAKE_BINARY_DIR}/src/frontier)

add_subdirectory(src)
add_subdirectory(test)
