enable_testing()

find_package(GTest CONFIG REQUIRED)

function(setup_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    target_include_directories(${TEST_NAME} PRIVATE ${FRONTIER_MAIN_INCLUDE_DIR})
    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main fmt::fmt spdlog::spdlog EnTT::EnTT)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_subdirectory(motor)
