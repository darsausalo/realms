include_directories("include" "src")

set(SOURCES
	# core
	"src/core/system_dispatcher.cpp"
	"src/core/binary_archive.cpp"
	"src/core/json_archive.cpp"
	"src/core/lua_archive.cpp"
	"src/core/prototype_registry.cpp"

	# services
	"src/services/files_service.cpp"
	"src/services/mods_service.cpp"
	"src/services/scripts_service.cpp"

	# host
	"src/host/game_state.cpp"
	"src/host/state_machine.cpp"
	"src/host/config_system.cpp"
	"src/host/window_system.cpp"
	"src/host/mods_system.cpp"
	"src/host/event_system.cpp"
	"src/host/mod.cpp"
	"src/host/application.cpp"

	# platform
	"src/platform/backward.cpp"
	"src/platform/platform.cpp"
	"src/platform/dynamic_library.cpp"
)

set(HEADERS
	# core
	"include/motor/core/attribute.h"
	"include/motor/core/system.h"
	"include/motor/core/system_dispatcher.h"
	"include/motor/core/type_traits.h"
	"include/motor/core/utility.h"
	"include/motor/core/archive.h"
	"include/motor/core/binary_archive.h"
	"include/motor/core/json_archive.h"
	"include/motor/core/lua_archive.h"
	"include/motor/core/plugin_context.h"
	"include/motor/core/prototype_registry.h"

	# services
	"include/motor/services/files_service.h"
	"include/motor/services/mods_service.h"
	"include/motor/services/scripts_service.h"

	# host
	"include/motor/host/transition.h"
	"include/motor/host/game_state.h"
	"include/motor/host/mod.h"
	"include/motor/host/application.h"

	"src/host/config_system.h"
	"src/host/window_system.h"
	"src/host/mods_system.h"
	"src/host/event_system.h"

	"src/host/state_machine.h"

	# platform
	"src/platform/platform.h"
	"src/platform/backward.hpp"
)

add_library(motor STATIC ${SOURCES} ${HEADERS})
target_compile_definitions(motor PRIVATE
	-DMOTOR_DATA_DIR="${PROJECT_DATA}"
	-DMOTOR_MODS_DIR="${PROJECT_MODS}"
	-DMOTOR_PROJECT_NAME="${PROJECT_NAME}"
	-DMOTOR_PROJECT_VERSION="${PROJECT_VERSION}"
	-DMOTOR_PROJECT_ORG="${PROJECT_ORG}"
	-DMOTOR_PROJECT_TITLE="${PROJECT_TITLE}"
	-DMOTOR_TEST_BASE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
	-DMOTOR_TEST_SCRIPT_DIR="${CMAKE_SOURCE_DIR}/data/mods/core"
)
target_include_directories(motor PRIVATE
	${LUA_INCLUDE_DIR}
)
target_link_libraries(motor PRIVATE
	mimalloc
	Backward::Backward
	nlohmann_json::nlohmann_json
	SDL2::SDL2
	sol2::sol2
	${LUA_LIBRARIES}
)
target_link_libraries(motor PUBLIC
	nameof::nameof
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)
add_backward(motor)

doctest_make_exe_for_static_lib(motor_test motor)
