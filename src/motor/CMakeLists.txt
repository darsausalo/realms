file(GLOB_RECURSE MOTOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(motor ${MOTOR_SOURCES})

target_include_directories(motor PUBLIC ${FRONTIER_MAIN_INCLUDE_DIR})
target_include_directories(motor PRIVATE ${CR_INCLUDE_DIRS})
target_compile_definitions(motor PRIVATE
        -DCR_HOST=CR_DISABLE
        -DCR_DEBUG
        -DMOTOR_HOST
        -DMOTOR_DEFAULT_DATA_DIR="data"
        -DMOTOR_DEFAULT_LOG_PATH="${CMAKE_CURRENT_BINARY_DIR}/logs/frontier.log"
        -DMOTOR_DEPLOY_DIR="${FRONTIER_DEPLOY_DIR}")
if (${MOTOR_NOINSTALL})
    target_compile_definitions(motor PRIVATE
            -DMOTOR_NOINSTALL)
endif ()

target_link_libraries(motor PRIVATE fmt::fmt spdlog::spdlog EnTT::EnTT)
