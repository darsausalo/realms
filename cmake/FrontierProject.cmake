# C++17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup vcpkg
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)

find_package(mimalloc 1.6 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nameof CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(efsw CONFIG REQUIRED)

include(FindLua)
find_package(Lua REQUIRED)

find_package(doctest CONFIG REQUIRED)

# Compiler-specific flags
if (MSVC)
	add_compile_options(/FC /utf-8 /Zc:__cplusplus)
endif()
