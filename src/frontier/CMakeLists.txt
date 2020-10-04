include_directories("include" "../motor/include" "src")

set(SOURCES
	# components

	# states
	"src/states/startup_state.cpp"
	"src/states/game_state.cpp"

	# main
	"src/main.cpp"
)

set (HEADERS
	# components
	"include/frontier/components/base.hpp"

	"src/components/serialization.hpp"

	# states
	"include/frontier/states/startup_state.hpp"
	"include/frontier/states/game_state.hpp"
)

add_executable(frontier ${SOURCES} ${HEADERS})
target_link_libraries(frontier PRIVATE
	motor
)

set_target_properties(frontier PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

doctest_force_implementation(frontier motor)
