#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer session setup must not enter an ordinary build
#endif
#include <cstdint>
#include <cstddef>

namespace th10::multiplayer {
struct Loadout { std::uint32_t character=0,shot=0; };
struct SessionSetup {
    std::uint64_t sessionId=0;
    std::uint32_t playerCount=0,localPlayer=0,difficulty=0,seed=0;
    std::uint32_t input_delay=0;
    Loadout loadouts[3]{};
    bool configured=false,started=false;
};
// v1 (local test compatibility): version, player count, local seat, difficulty,
// seed, then three char/shot pairs.
// v2 (legacy real netplay): header + session-id low/high + loadouts.
// v3: v2 plus input-delay frames before the loadouts. Unused pairs must be
// zero. Reject before changing any owner.
bool DecodeSessionSetup(SessionSetup&,const std::uint32_t*,std::size_t) noexcept;
std::uint32_t GameplayContract(const SessionSetup&) noexcept;
std::uint32_t LegacyGameplayContractV4(const SessionSetup&) noexcept;
}
