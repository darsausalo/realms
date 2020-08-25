include(FetchContent)
FetchContent_Declare(minctest
	GIT_REPOSITORY https://github.com/codeplea/minctest.git
	GIT_TAG c79fc75a55e826fc5742afc180fc198f81f9f13c)
FetchContent_MakeAvailable(minctest)

add_executable(test_myml
    test_myml.c
    ${CMAKE_SOURCE_DIR}/src/motor/os_api.c
    ${CMAKE_SOURCE_DIR}/src/motor/log.c
    ${CMAKE_SOURCE_DIR}/src/motor/myml.c
    ${CMAKE_SOURCE_DIR}/src/motor/sys/crash_handler_windows.c)
add_test(NAME test_myml COMMAND test_myml)

target_compile_definitions(test_myml PRIVATE -DUNIT_TESTING_DEBUG=1)
target_include_directories(test_myml PRIVATE
    ${minctest_SOURCE_DIR}
    ${FRONTIER_INCLUDE_DIRS}
    ${FRONTIER_INCLUDE_DIR}/motor/private)
target_link_libraries(test_myml PRIVATE mimalloc)
