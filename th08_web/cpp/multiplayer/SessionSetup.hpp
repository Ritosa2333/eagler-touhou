#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer session setup must not enter an ordinary build
#endif
#include <cstdint>
#include <cstddef>

namespace th08::multiplayer {
struct SessionSetup {
    std::uint32_t player_count=0,local_player=0,difficulty=0,seed=0;
    std::uint32_t characters[3]{};
    bool configured=false,started=false;
};
// Shared session envelope: version, count, local seat, difficulty, seed and
// three character/shot pairs. TH08 has one loadout ID, so shot must be zero.
bool decode_session_setup(SessionSetup&,const std::uint32_t*,std::size_t) noexcept;
std::uint32_t gameplay_contract(const SessionSetup&) noexcept;
}
