add_subdirectory(plugins)

file(GLOB_RECURSE MOTOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    list(FILTER MOTOR_SOURCES EXCLUDE REGEX "\\\\*_linux\\.c")
    list(FILTER MOTOR_SOURCES EXCLUDE REGEX "\\\\*_osx\\.c")
    list(FILTER MOTOR_SOURCES EXCLUDE REGEX "\\\\*_unix\\.c")
else()
    # TODO: support for Linux, OSX
    message(SEND_ERROR "Not supported platform: " ${CMAKE_SYSTEM_NAME})
endif()

add_executable(frontier
    ${MOTOR_SOURCES})

target_include_directories(frontier PRIVATE
    ${FRONTIER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(frontier PRIVATE
    -DFRONTIER_GAME_PLUGIN="${FRONTIER_GAME_PLUGIN}"
    -DFRONTIER_BINARY_DIR="${FRONTIER_BINARY_DIR}")
target_link_libraries(frontier PRIVATE
    mimalloc
    plugins)
