#pragma once

#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th06LanStageProbe
{
// Owns one fixed-60-Hz calc-chain call for either the bounded LAN audit probe
// or the opt-in production netplayMode=lan session. Gameplay remains
// transport-agnostic; this driver is the network/rollback boundary.
int RunCalcChain();
bool Requested();
bool Active();
// ECL boss registrations are observed at their native construction point so
// rollback replaces the bookmark for the same logical frame.
void RecordBossRegistration(std::uint8_t slot);
bool SpectatorViewing();
const char *SpectatorState();
int SpectatorControl(const char *run, const char *action, std::uint32_t frame);
// Production RTC negotiation starts while the title/loading chain is still
// presenting. Stage 1 must not be dispatched until this becomes true.
bool TransportReady();
bool LastTickAdvanced();
void ReturnToRoom();
void BeginRoomRun();
void SetGameplayStartSeed(std::uint16_t seed);
// Presentation/wall-clock scheduler correction only. 1.0 is the native
// 60 Hz cadence; production LAN may make a small temporary adjustment to
// converge frame advantage without changing deterministic simulation state.
double SimulationIntervalScale();
} // namespace Netplay::Th06LanStageProbe

extern "C"
{
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
const char *th06_mp_spectator_state();
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int th06_mp_spectator_control(const char *run, const char *action, std::uint32_t frame);
}
