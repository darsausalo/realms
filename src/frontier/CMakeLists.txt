include_directories("include" "../motor/include")

set(SOURCES
	"src/loading_state.cpp"
	"src/main.cpp"
)

set (HEADERS
	"include/loading_state.h"
)

add_executable(frontier ${SOURCES} ${HEADERS})
target_link_libraries(frontier PRIVATE
	motor
)
