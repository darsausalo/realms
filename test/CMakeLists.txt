add_executable(test_myml
    test_myml.c
    ${CMAKE_SOURCE_DIR}/src/motor/myml.c
    ${CMAKE_SOURCE_DIR}/src/motor/sys/crash_handler_windows.c)
add_test(NAME test_myml COMMAND test_myml)
target_include_directories(test_myml PRIVATE
    ${minctest_SOURCE_DIR}
    ${FRONTIER_INCLUDE_DIR})
