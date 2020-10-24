include_directories("include" "../motor/include" "src")

set(SOURCES
	# components

	# main
	"src/main.cpp"
)

set (HEADERS
	# components
	"include/frontier/components/base.hpp"

	"src/components/serialization.hpp"
)

add_executable(frontier ${SOURCES} ${HEADERS})
target_link_libraries(frontier PRIVATE
	motor
)

set_target_properties(frontier PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

doctest_force_implementation(frontier motor)
