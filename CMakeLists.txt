cmake_minimum_required(VERSION 3.17)

project(realms VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Dependencies)
include(BackwardConfig)
include(DocTest)

set(PROJECT_TITLE "Realms")
set(PROJECT_ORG "Auriga Games")

set(PROJECT_BIN "bin")
set(PROJECT_LIB "lib")
set(PROJECT_DATA "data")

add_subdirectory(src)
