#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer spectator activation must not enter an ordinary build
#endif
#include <cstdint>
#include <vector>
namespace th10::browser { struct World; }
namespace Netplay::SpectatorScalars { class Writer; class Reader; }
namespace th10::multiplayer {
struct SpectatorStageState;
// A later-stage checkpoint is taken inside native activation, before the
// controller is spawned. The first replayed input resumes that same branch.
struct SpectatorStageActivationState {
    bool present=false;
    std::int32_t elapsedPrevious=0,elapsedCurrent=0;
    float elapsedFractional=0;
    std::uint32_t timerFlags=0,sessionFlags=0;
    std::uint32_t uncompressedBytes=0,checksum=0;
    std::vector<std::uint8_t> compressed;
};
bool CaptureSpectatorStageActivation(browser::World&,SpectatorStageState&);
bool PrepareSpectatorStageActivation(browser::World&,const SpectatorStageState&);
bool RestoreSpectatorStageActivation(browser::World&,const SpectatorStageState&);
bool WriteSpectatorStageActivation(Netplay::SpectatorScalars::Writer&,const SpectatorStageActivationState&);
bool ReadSpectatorStageActivation(Netplay::SpectatorScalars::Reader&,SpectatorStageActivationState&);
}
