include_directories("include" "src")

set(SOURCES
	# core
	"src/core/system_dispatcher.cpp"
	"src/core/binary_archive.cpp"
	"src/core/json_archive.cpp"
	"src/core/prefab_loader.cpp"

	# host
	"src/host/game_state.cpp"
	"src/host/state_machine.cpp"
	"src/host/storage.cpp"
	"src/host/config_system.cpp"
	"src/host/window_system.cpp"
	"src/host/event_system.cpp"
	"src/host/plugin_context.cpp"
	"src/host/application.cpp"

	# platform
	"src/platform/backward.cpp"

	"src/platform/platform_windows.cpp"
)

set(HEADERS
	# core
	"include/motor/core/system.h"
	"include/motor/core/system_dispatcher.h"
	"include/motor/core/type_traits.h"
	"include/motor/core/utility.h"
	"include/motor/core/archive.h"
	"include/motor/core/binary_archive.h"
	"include/motor/core/json_archive.h"
	"include/motor/core/prefab_loader.h"
	"include/motor/core/system_module.h"

	# host
	"include/motor/host/transition.h"
	"include/motor/host/game_state.h"
	"include/motor/host/storage.h"
	"include/motor/host/plugin_context.h"
	"include/motor/host/application.h"

	"src/host/config_system.h"
	"src/host/window_system.h"
	"src/host/event_system.h"

	"src/host/state_machine.h"

	# platform
	"src/platform/platform.h"
	"src/platform/backward.hpp"
)

add_library(motor STATIC ${SOURCES} ${HEADERS})
target_include_directories(motor PUBLIC ${REFL_CPP_INCLUDE_DIRS})
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
	SDL2::SDL2
)
target_link_libraries(motor PUBLIC
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)
add_backward(motor)

doctest_make_exe_for_static_lib(motor_test motor)
