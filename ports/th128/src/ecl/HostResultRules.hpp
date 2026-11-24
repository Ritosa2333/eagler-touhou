#pragma once
#include <cstdint>

namespace th128::ecl {

constexpr bool hostResultAdvances(std::int32_t result) noexcept { return result != -1; }
constexpr bool isOriginalHostTableOpcode(std::uint16_t opcode) noexcept { return opcode >= 256 && opcode <= 615; }

}
