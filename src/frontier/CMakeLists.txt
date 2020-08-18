file(GLOB_RECURSE FRONTIER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(frontier MODULE
        ${FRONTIER_SOURCES}
        ${FRONTIER_MAIN_SOURCE_DIR}/motor/Core/Logger.cpp
        ${FRONTIER_MAIN_SOURCE_DIR}/motor/Core/Shell.cpp)

target_include_directories(frontier PUBLIC ${FRONTIER_MAIN_INCLUDE_DIR})
target_include_directories(frontier PRIVATE ${CR_INCLUDE_DIRS})
target_link_libraries(frontier PRIVATE fmt::fmt spdlog::spdlog EnTT::EnTT)
