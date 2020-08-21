cmake_minimum_required(VERSION 3.17)
project(frontier CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if (MSVC)
    add_compile_options(/FC)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

find_package(cxxopts CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(READERWRITERQUEUE_INCLUDE_DIRS "readerwriterqueue/atomicops.h")
find_package(entt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set(MOTOR_ORG_NAME "Auriga Games")
set(MOTOR_APP_NAME "Frontier")

set(FRONTIER_MAIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(FRONTIER_MAIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FRONTIER_DEPLOY_DIR ${CMAKE_BINARY_DIR}/src/frontier)

add_subdirectory(src)
add_subdirectory(test)
