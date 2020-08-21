file(GLOB_RECURSE MOTOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(motor STATIC ${MOTOR_SOURCES})

target_include_directories(motor PUBLIC ${FRONTIER_MAIN_INCLUDE_DIR})
target_include_directories(motor PRIVATE
        ${READERWRITERQUEUE_INCLUDE_DIRS})
target_compile_definitions(motor PRIVATE
        -DMOTOR_ORG_NAME="${MOTOR_ORG_NAME}"
        -DMOTOR_APP_NAME="${MOTOR_APP_NAME}"
        -DMOTOR_DEFAULT_DATA_DIR="data"
        -DMOTOR_DEFAULT_LOG_PATH="logs/frontier.log")
if (${MOTOR_NOINSTALL})
    target_compile_definitions(motor PRIVATE
            -DMOTOR_NOINSTALL
            -DMOTOR_DEFAULT_BASE_DIR="${CMAKE_SOURCE_DIR}/data"
            -DMOTOR_DEFAULT_USER_DIR="${CMAKE_SOURCE_DIR}/save")
endif ()

target_link_libraries(motor PRIVATE
        cxxopts::cxxopts
        tomlplusplus::tomlplusplus
        spdlog::spdlog
        EnTT::EnTT
        SDL2::SDL2 SDL2::SDL2main)

install(TARGETS motor RUNTIME DESTINATION bin)
