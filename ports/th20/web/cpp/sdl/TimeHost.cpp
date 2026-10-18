#include "../platform/Time.hpp"
#include <ctime>

extern "C" double sdl_loop_time();

namespace th20::web::time {
double monotonic_seconds() { return ::sdl_loop_time(); }
std::uint32_t monotonic_milliseconds() { return static_cast<std::uint32_t>(::sdl_loop_time() * 1000.0); }
std::int64_t unix_seconds() { return static_cast<std::int64_t>(std::time(nullptr)); }
void sleep(std::uint32_t) {}
}
