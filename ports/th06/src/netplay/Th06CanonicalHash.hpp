#pragma once

#include <cstdint>

namespace Netplay::Th06CanonicalHash
{
struct Sample
{
    std::uint64_t meta = 0;
    std::uint64_t metaRng = 0;
    std::uint64_t metaGame = 0;
    std::uint64_t metaInput = 0;
    std::uint64_t metaSupervisor = 0;
    std::uint64_t stage = 0;
    std::uint64_t player = 0;
    std::uint64_t player0 = 0;
    std::uint64_t player1 = 0;
    std::uint64_t player2 = 0;
    std::uint64_t enemies = 0;
    std::uint64_t bullets = 0;
    std::uint64_t items = 0;
    std::uint64_t composite = 0;
    std::uint32_t enemyCount = 0;
    std::uint32_t bulletCount = 0;
    std::uint32_t laserCount = 0;
    std::uint32_t itemCount = 0;
};

// Cross-instance diagnostic hash. It deliberately excludes heap addresses,
// renderer/Web handles, function pointers, object padding and
// wall-clock/presentation state. Pointer references that matter to gameplay are
// converted to stable pool indices before hashing.
Sample Capture();
} // namespace Netplay::Th06CanonicalHash
