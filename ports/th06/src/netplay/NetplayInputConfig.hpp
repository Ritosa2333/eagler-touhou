#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <eagler/netplay/NetplayProtocol.hpp>

extern std::uint16_t g_CurFrameGameInputs[Netplay::MAX_PLAYERS];
extern std::uint16_t g_LastFrameGameInputs[Netplay::MAX_PLAYERS];

namespace Netplay::InputConfig
{
inline void CommitGameInputs(
    const std::array<std::uint16_t, Netplay::MAX_PLAYERS> &buttons)
{
    for (std::size_t player = 0; player < buttons.size(); ++player)
    {
        g_LastFrameGameInputs[player] = g_CurFrameGameInputs[player];
        g_CurFrameGameInputs[player] = buttons[player];
    }
}
} // namespace Netplay::InputConfig
