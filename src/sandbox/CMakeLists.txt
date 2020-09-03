include_directories("../motor/include")

set(SOURCES
	main3.cpp
	plugin_context.cpp
)

add_executable(sandbox ${SOURCES})
target_link_libraries(sandbox PRIVATE
	motor
	nameof::nameof
	EnTT::EnTT
	nlohmann_json::nlohmann_json
	cereal
)
