#pragma once

namespace Netplay::Th06LanStageProbe
{
// Owns one fixed-60-Hz calc-chain call for either the bounded LAN audit probe
// or the opt-in production netplayMode=lan session. Gameplay remains
// transport-agnostic; this driver is the network/rollback boundary.
int RunCalcChain();
bool Requested();
bool Active();
// Production RTC negotiation starts while the title/loading chain is still
// presenting. Stage 1 must not be dispatched until this becomes true.
bool TransportReady();
bool LastTickAdvanced();
// Presentation/wall-clock scheduler correction only. 1.0 is the native
// 60 Hz cadence; production LAN may make a small temporary adjustment to
// converge frame advantage without changing deterministic simulation state.
double SimulationIntervalScale();
} // namespace Netplay::Th06LanStageProbe
