#pragma once

#include <cstdint>

namespace Netplay::ProtocolConfig {
inline constexpr std::uint8_t MagicGame = 'I';
inline constexpr std::uint8_t MaxAnalogMode = 2; // DirectTouch
}
