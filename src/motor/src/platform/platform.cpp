#include "platform.hpp"
#include "backward.hpp"
#include <SDL.h>

#if defined(_WIN32) && !defined(WINDOWS_STORE)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace motor::platform {

void setup_crash_handling(const std::filesystem::path& base_path) {
    backward::report_path = base_path.string();
    backward::report_path += "\\error_report.txt";

    backward::sym_search_path = base_path.wstring();
}

} // namespace motor::platform

#endif // defined(_WIN32) && !defines(WINDOWS_STORE)
