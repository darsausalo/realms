include_directories("include" "src")

set(SOURCES
	# core
	"src/core/game_state.cpp"
	"src/core/state_machine.cpp"
	"src/core/application.cpp"

	# platform
	"src/platform/platform.cpp"
	"src/platform/platform_windows.cpp"
)

set(HEADERS
	# core
	"include/motor/core/exception.h"
	"include/motor/core/transition.h"
	"include/motor/core/game_state.h"
	"include/motor/core/state_machine.h"
	"include/motor/core/application.h"

	# platform
	"src/platform/platform.h"
	"src/platform/backward.hpp"
)

add_library(motor ${SOURCES} ${HEADERS})
target_include_directories(motor PRIVATE ${UTFCPP_INCLUDE_DIRS})
target_link_libraries(motor PRIVATE
	mimalloc
	Backward::Backward
	SDL2::SDL2
)
target_link_libraries(motor PUBLIC
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)
add_backward(motor)
