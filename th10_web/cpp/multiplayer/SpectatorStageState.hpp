#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer spectator state must not enter an ordinary build
#endif

#include "ReplayArchive.hpp"
#include "SpectatorStageScene.hpp"
#include "SpectatorStageActivation.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>

namespace th10::multiplayer {

// Fieldwise, versioned wire codec for the title-owned stage-entry state.
// Keep this list explicit: no native object, padding, or pointer is serialized.
constexpr std::uint32_t kSpectatorStageSchema = 3;
struct SpectatorStageState {
    ReplayCheckpoint checkpoint{};
    u8 optionSnapNext[Netplay::MAX_PLAYERS][4]{};
    TeamEconomy team{};
    PilotEconomy pilots[Netplay::MAX_PLAYERS]{};
    Netplay::FrameInput inputs[Netplay::MAX_PLAYERS]{};
    GameInput hostInput{};
    u16 pauseRising=0;
    SessionSetup route{};
    SpectatorStageSceneSnapshot scene{};
    SpectatorStageActivationState activation{};
};
bool EncodeSpectatorStageState(const SpectatorStageState&, std::vector<std::uint8_t>&);
bool DecodeSpectatorStageState(const std::vector<std::uint8_t>&, SpectatorStageState&);

} // namespace th10::multiplayer
