include_directories("include" "../../motor/include" "../../frontier/include")

set(SOURCES
	"src/entry.cpp"
)

add_library(core MODULE ${SOURCES})
target_include_directories(core PUBLIC ${REFL_CPP_INCLUDE_DIRS})
target_link_libraries(core PRIVATE
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)

set_target_properties(core PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/data/mods/core"
)
