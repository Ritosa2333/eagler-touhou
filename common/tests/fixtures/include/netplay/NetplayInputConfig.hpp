#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <eagler/netplay/NetplayProtocol.hpp>

namespace Netplay::InputConfig
{
inline std::array<std::uint16_t, Netplay::MAX_PLAYERS> committed{};
inline std::size_t commitCount = 0;

inline void CommitGameInputs(
    const std::array<std::uint16_t, Netplay::MAX_PLAYERS> &buttons)
{
    committed = buttons;
    ++commitCount;
}

inline void ResetFixture()
{
    committed.fill(0);
    commitCount = 0;
}
} // namespace Netplay::InputConfig
