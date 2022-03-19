include_directories("include" "src")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(motor STATIC ${SOURCES} ${HEADERS})
target_compile_definitions(motor PRIVATE
	-DMOTOR_DATA_DIR="${PROJECT_DATA}"
	-DMOTOR_PROJECT_NAME="${PROJECT_NAME}"
	-DMOTOR_PROJECT_VERSION="${PROJECT_VERSION}"
	-DMOTOR_PROJECT_ORG="${PROJECT_ORG}"
	-DMOTOR_PROJECT_TITLE="${PROJECT_TITLE}"
	-DMOTOR_TEST_BASE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
	-DMOTOR_TEST_SCRIPT_DIR="${CMAKE_SOURCE_DIR}/data/mods/core"
	-DIMGUI_ENABLE_FREETYPE
)
target_compile_definitions(motor PUBLIC
	-DENTT_USE_ATOMIC)
target_include_directories(motor PRIVATE
	${STB_INCLUDE_DIRS}
	${SOKOL_INCLUDE_DIRS}
	${LUA_INCLUDE_DIR}
	${ATOMIC_QUEUE_INCLUDE_DIRS}
)
target_link_libraries(motor PRIVATE
	mimalloc
	Backward::Backward
	nlohmann_json::nlohmann_json
	efsw::efsw
	SDL2::SDL2
	GLEW::GLEW
	imgui::imgui
	sol2::sol2
	${LUA_LIBRARIES}
)
target_link_libraries(motor PUBLIC
	nameof::nameof
#	fmt::fmt
	spdlog::spdlog
	glm::glm
	EnTT::EnTT
)
add_backward(motor)

doctest_make_exe_for_static_lib(motor_test motor)
