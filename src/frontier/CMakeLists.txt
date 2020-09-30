include_directories("include" "../motor/include")

set(SOURCES
	"src/loading_state.cpp"
	"src/main_state.cpp"
	"src/main.cpp"
)

set (HEADERS
	"include/loading_state.h"
	"include/main_state.h"

	"src/component_serialization.h"
)

add_executable(frontier ${SOURCES} ${HEADERS})
target_link_libraries(frontier PRIVATE
	motor
)

set_target_properties(frontier PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

doctest_force_implementation(frontier motor)
