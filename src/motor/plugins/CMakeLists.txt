# TODO add platforms support

add_library(plugins STATIC
	${CMAKE_CURRENT_LIST_DIR}/plugins.cpp
	${CMAKE_CURRENT_LIST_DIR}/plugins_windows.cpp
	${CMAKE_CURRENT_LIST_DIR}/../log.c)

target_include_directories(plugins PRIVATE
	${FRONTIER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}/..)
