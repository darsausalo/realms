include_directories("include" "../motor/include" "src")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(realms ${SOURCES} ${HEADERS})
target_link_libraries(realms PRIVATE
	motor
)

set_target_properties(realms PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	if (MSVC)
		set_target_properties(realms PROPERTIES
			LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)
	endif()
endif()

doctest_force_implementation(realms motor)
