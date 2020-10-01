include_directories("include" "../motor/include" "src")

set(SOURCES
	# components

	# states
	"src/states/loading_state.cpp"
	"src/states/main_state.cpp"

	# main
	"src/main.cpp"
)

set (HEADERS
	# components
	"include/frontier/components/base.h"

	"src/components/serialization.h"

	# states
	"include/frontier/states/loading_state.h"
	"include/frontier/states/main_state.h"
)

add_executable(frontier ${SOURCES} ${HEADERS})
target_link_libraries(frontier PRIVATE
	motor
)

set_target_properties(frontier PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

doctest_force_implementation(frontier motor)
