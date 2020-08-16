set(MYML_TEST_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/Lib/Myml.cpp
        ${FRONTIER_MAIN_SOURCE_DIR}/motor/Lib/Myml.cpp)

setup_test(myml ${MYML_TEST_SOURCES})

#add_executable(myml ${MYML_TEST_SOURCES})
#target_include_directories(myml PRIVATE ${FRONTIER_MAIN_INCLUDE_DIR})
#target_link_libraries(myml PRIVATE GTest::gtest GTest::gtest_main fmt::fmt spdlog::spdlog EnTT::EnTT)
#add_test(NAME myml COMMAND myml)
