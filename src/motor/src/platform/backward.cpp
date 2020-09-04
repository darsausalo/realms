#include "backward.hpp"

namespace backward {

SignalHandling sh;

std::string report_path;
#ifdef BACKWARD_SYSTEM_WINDOWS
std::wstring sym_search_path;
#endif

} // namespace backward
