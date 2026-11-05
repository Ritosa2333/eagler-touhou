#pragma once
#include "Types.hpp"
#include <utility>
namespace th15 {
// JP 1.00b GetKeyboardState mapping (401f50), before gamepad merging.
u32 keyboard_keys(const bool* virtual_keys)noexcept;
}
