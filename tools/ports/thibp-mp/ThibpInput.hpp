#pragma once

#include <array>
#include <cstdint>

namespace thibp {
constexpr int maxPlayers = 3;
constexpr int virtualKeyCount = 12;
constexpr unsigned retryButton = 1u << virtualKeyCount;
constexpr unsigned validButtons = (retryButton << 1) - 1;
constexpr int maxTouchX = 640;
constexpr int maxTouchY = 480;

using Buttons = std::array<std::uint16_t, maxPlayers>;
using InputFrame = std::array<std::uint32_t, maxPlayers>;

// The low 13 bits are virtual keys. The upper 19 bits store a target
// position with X biased by one, leaving zero for keyboard-only frames.
constexpr unsigned touchXField(std::uint32_t input) { return (input >> 13) & 0x3ff; }
constexpr bool hasTouchPosition(std::uint32_t input) { return touchXField(input) != 0; }
constexpr int touchX(std::uint32_t input) { return static_cast<int>(touchXField(input)) - 1; }
constexpr int touchY(std::uint32_t input) { return static_cast<int>(input >> 23); }
constexpr std::uint32_t withTouchPosition(unsigned buttons, unsigned x, unsigned y)
{
    return buttons | ((x + 1) << 13) | (y << 23);
}
constexpr bool validInput(std::uint32_t input)
{
    return (input >> 13) == 0 ||
        (hasTouchPosition(input) && touchX(input) <= maxTouchX && touchY(input) <= maxTouchY);
}
}
