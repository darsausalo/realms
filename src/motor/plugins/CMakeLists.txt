add_library(plugins STATIC
	${CMAKE_CURRENT_LIST_DIR}/plugins.cpp)

target_include_directories(plugins PRIVATE
	${FRONTIER_INCLUDE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..)
