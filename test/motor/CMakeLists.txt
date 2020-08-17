set(MYML_TEST_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/Lib/Myml.cpp
        ${FRONTIER_MAIN_SOURCE_DIR}/motor/Lib/Myml.cpp)

setup_test(myml ${MYML_TEST_SOURCES})
