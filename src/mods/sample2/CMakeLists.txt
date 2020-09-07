include_directories("include" "../../motor/include")

set(SOURCES
	"src/entry.cpp"
)

add_library(sample2 MODULE ${SOURCES})
target_include_directories(sample2 PUBLIC ${REFL_CPP_INCLUDE_DIRS})
target_link_libraries(sample2 PRIVATE
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)

set_target_properties(sample2 PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/data/mods/sample2"
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/data/mods/sample2"
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/data/mods/sample2"
)
