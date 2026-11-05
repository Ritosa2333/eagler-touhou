#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer spectator state must not enter an ordinary build
#endif

#include "ReplayArchive.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>

namespace th10::multiplayer {

// Fieldwise, versioned wire codec for the title-owned stage-entry state.
// Keep this list explicit: no native object, padding, or pointer is serialized.
constexpr std::uint32_t kSpectatorStageSchema = 4;
struct SpectatorStageState {
    ReplayCheckpoint checkpoint{};
    SessionSetup route{};
};
bool EncodeSpectatorStageState(const SpectatorStageState&, std::vector<std::uint8_t>&);
bool DecodeSpectatorStageState(const std::vector<std::uint8_t>&, SpectatorStageState&);

} // namespace th10::multiplayer
