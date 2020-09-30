include_directories("include" "../../motor/include")

set(SOURCES
	"src/entry.cpp"
)

add_library(sample1 MODULE ${SOURCES})
target_include_directories(sample1 PUBLIC ${REFL_CPP_INCLUDE_DIRS})
target_link_libraries(sample1 PRIVATE
	fmt::fmt
	spdlog::spdlog
	EnTT::EnTT
)

set_target_properties(sample1 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/data/mods/sample1"
)
