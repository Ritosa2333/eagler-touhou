#pragma once
#include <cstdint>
namespace th20::source::platform {
// Signed high word plus unsigned low word, in original SSE2 operation order.
double signed_counter_to_double(std::uint64_t) noexcept; // 0x542e00 SSE2 path
double performance_clock(std::uint64_t sample, std::uint64_t origin,
                         std::uint64_t frequency, double& offset) noexcept;
double multimedia_clock(std::uint32_t milliseconds, double& offset) noexcept;
bool set_rounding_mode(std::uint32_t) noexcept;           // 0x54a4e0
}
