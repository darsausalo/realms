#include "backward.hpp"
#include "platform.h"
#include <SDL.h>

#if defined(_WIN32) && !defined(WINDOWS_STORE)

#define UTF_CPP_CPLUSPLUS __cplusplus
#include <utf8.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <psapi.h>

namespace motor {

void platform::setup_crash_handling(std::string_view base_path) {
    SetConsoleOutputCP(CP_UTF8);

    backward::report_path = base_path;
    backward::report_path += "\\error_report.txt";

    std::u16string sym_search_path = utf8::utf8to16(std::string(base_path));

    backward::sym_search_path.clear();
    for (auto c : sym_search_path) {
        backward::sym_search_path += c;
    }
}

} // namespace motor

namespace backward {

std::wstring sym_search_path;

} // namespace backward


#endif // defined(_WIN32) && !defines(WINDOWS_STORE)
