file(GLOB_RECURSE MOTOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)

add_executable(frontier
    ${MOTOR_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/host/host.cpp)

target_include_directories(frontier PRIVATE
    ${FRONTIER_MAIN_INCLUDE_DIR})
target_compile_definitions(frontier PRIVATE
    -DFRONTIER_GAME_PLUGIN="${FRONTIER_GAME_PLUGIN}"
    -DFRONTIER_DEPLOY_DIR="${FRONTIER_DEPLOY_DIR}")

if (MOTOR_NOINSTALL)
    target_compile_definitions(frontier PRIVATE
        -DMOTOR_NOINSTALL
        -DMOTOR_DEFAULT_BASE_DIR="${FRONTIER_DEPLOY_DIR}/data"
        -DMOTOR_DEFAULT_SAVE_DIR="${FRONTIER_DEPLOY_DIR}/save")
endif()
