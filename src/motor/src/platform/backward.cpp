#include "backward.hpp"

namespace backward {

#ifdef MOTOR_SIGNAL_HANDLING
SignalHandling sh;
#endif // MOTOR_SIGNAL_HANDLING

std::string report_path;
#ifdef BACKWARD_SYSTEM_WINDOWS
std::wstring sym_search_path;
#endif

} // namespace backward
