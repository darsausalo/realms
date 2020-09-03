include_directories("include" "src")

set(SOURCES
	# core
	"src/core/game_state.cpp"
	"src/core/state_machine.cpp"
	"src/core/storage.cpp"
	"src/core/application.cpp"
	"src/core/config_system.cpp"
	"src/core/window_system.cpp"
	"src/core/event_system.cpp"

	# platform
	"src/platform/platform.cpp"
	"src/platform/platform_windows.cpp"

	# systems
	"src/systems/system_dispatcher.cpp"
)

set(HEADERS
	# core
	"include/motor/core/utility.h"
	"include/motor/core/transition.h"
	"include/motor/core/game_state.h"
	"include/motor/core/storage.h"
	"include/motor/core/core_context.h"
	"include/motor/core/application.h"

	"src/core/config_system.h"
	"src/core/window_system.h"
	"src/core/event_system.h"

	"src/core/state_machine.h"

	# platform
	"src/platform/platform.h"
	"src/platform/backward.hpp"

	# systems
	"include/motor/systems/system.h"
	"include/motor/systems/system_dispatcher.h"
)

add_library(motor ${SOURCES} ${HEADERS})
target_include_directories(motor PRIVATE ${UTFCPP_INCLUDE_DIRS})
target_compile_definitions(motor PRIVATE
	-DMOTOR_DATA_DIR="data"
	-DMOTOR_PROJECT_NAME="${PROJECT_NAME}"
	-DMOTOR_PROJECT_VERSION="${PROJECT_VERSION}"
	-DMOTOR_PROJECT_ORG="${PROJECT_ORG}"
	-DMOTOR_PROJECT_TITLE="${PROJECT_TITLE}"
)
target_link_libraries(motor PRIVATE
	mimalloc
	Backward::Backward
	nlohmann_json::nlohmann_json
	cereal
	SDL2::SDL2
)
target_link_libraries(motor PUBLIC
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)
add_backward(motor)
