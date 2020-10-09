include_directories("include" "src")

set(SOURCES
	# core
	"src/core/binary_archive.cpp"
	"src/core/lua_archive.cpp"
	"src/core/filesystem.cpp"
	"src/core/scripts.cpp"

	# entity
	"src/entity/prototype_registry.cpp"
	"src/entity/scheduler.cpp"

	# mods
	"src/mods/mods_system.cpp"

	# resources
	# "src/resources/image.cpp"

	# graphics
	"src/graphics/graphics_system.cpp"

	# app
	"src/app/config_system.cpp"
	"src/app/window_system.cpp"
	"src/app/input_system.cpp"
	"src/app/app_builder.cpp"
	"src/app/app.cpp"

	# platform
	"src/platform/backward.cpp"
	"src/platform/platform.cpp"
	"src/platform/file_watcher.cpp"
)

set(HEADERS
	# core
	"include/motor/core/type_traits.hpp"
	"include/motor/core/utility.hpp"
	"include/motor/core/algorithm.hpp"
	"include/motor/core/archive.hpp"
	"include/motor/core/binary_archive.hpp"
	"include/motor/core/lua_archive.hpp"
	"include/motor/core/filesystem.hpp"
	"include/motor/core/time.hpp"
	"include/motor/core/events.hpp"
	"include/motor/core/input.hpp"
	"include/motor/core/scripts.hpp"

	"src/core/progress.hpp"

	# entity
	"include/motor/entity/component_registry.hpp"
	"include/motor/entity/prototype_registry.hpp"
	"include/motor/entity/scheduler.hpp"
	"include/motor/entity/executor.hpp"

	# mods
	"include/motor/mods/mod.hpp"

	"src/mods/mods_system.hpp"

	# resources
	# "include/motor/resources/image.hpp"
	# "include/motor/resources/image_loader.hpp"
	# "include/motor/resources/resources.hpp"

	# graphics
	"include/motor/graphics/screen.hpp"

	"src/graphics/graphics_system.hpp"

	# app
	"include/motor/app/app.hpp"
	"include/motor/app/app_builder.hpp"

	"src/app/config_system.hpp"
	"src/app/window_system.hpp"
	"src/app/time_system.hpp"
	"src/app/input_system.hpp"

	# platform
	"src/platform/platform.hpp"
	"src/platform/backward.hpp"
	"src/platform/file_watcher.hpp"
)

add_library(motor STATIC ${SOURCES} ${HEADERS})
target_compile_definitions(motor PRIVATE
	-DMOTOR_DATA_DIR="${PROJECT_DATA}"
	-DMOTOR_PROJECT_NAME="${PROJECT_NAME}"
	-DMOTOR_PROJECT_VERSION="${PROJECT_VERSION}"
	-DMOTOR_PROJECT_ORG="${PROJECT_ORG}"
	-DMOTOR_PROJECT_TITLE="${PROJECT_TITLE}"
	-DMOTOR_TEST_BASE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
	-DMOTOR_TEST_SCRIPT_DIR="${CMAKE_SOURCE_DIR}/data/mods/core"
)
target_include_directories(motor PRIVATE
	${STB_INCLUDE_DIRS}
	${SOKOL_INCLUDE_DIRS}
	${LUA_INCLUDE_DIR}
)
target_link_libraries(motor PRIVATE
	mimalloc
	Backward::Backward
	nlohmann_json::nlohmann_json
	efsw::efsw
	SDL2::SDL2
	GLEW::GLEW
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
