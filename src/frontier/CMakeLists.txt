file(GLOB_RECURSE FRONTIER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(frontier
        ${FRONTIER_SOURCES})

target_include_directories(frontier PUBLIC ${FRONTIER_MAIN_INCLUDE_DIR})
target_link_libraries(frontier PRIVATE
        motor)
