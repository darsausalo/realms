include_directories("include" "../motor/include" "src")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(sandbox ${SOURCES} ${HEADERS})
target_link_libraries(sandbox PRIVATE
	motor
)

set_target_properties(sandbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	if (MSVC)
		set_target_properties(sandbox PROPERTIES
			LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)
	endif()
endif()

doctest_force_implementation(sandbox motor)
