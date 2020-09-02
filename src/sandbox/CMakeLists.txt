set(SOURCES
	main2.cpp
)

add_executable(sandbox ${SOURCES})
target_link_libraries(sandbox PRIVATE
	motor
	EnTT::EnTT
	nlohmann_json::nlohmann_json
	cereal
)
