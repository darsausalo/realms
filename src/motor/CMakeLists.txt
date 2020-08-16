file(GLOB_RECURSE MOTOR_SOURCES *.cpp)

add_executable(motor ${MOTOR_SOURCES})

target_include_directories(motor PUBLIC ${FRONTIER_MAIN_INCLUDE_DIR})

target_link_libraries(motor PRIVATE fmt::fmt spdlog::spdlog EnTT::EnTT)
