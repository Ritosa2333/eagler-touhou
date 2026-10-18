#pragma once

namespace Netplay::Th07LanStageProbe
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
// True only while authoritative world state is between rollback restore and
// the final replay frame. Draw must not inspect this partial historical state.
bool ReconciliationInProgress();
// Presentation/wall-clock scheduler correction only. 1.0 is the native
// 60 Hz cadence; production LAN may make a small temporary adjustment to
// converge frame advantage without changing deterministic simulation state.
double SimulationIntervalScale();
// Optional read-only performance observation. No GPU readback or pacing input.
bool PerformanceTelemetryEnabled();
void RecordPresentationCost(double driverMs, double drawMs);
} // namespace Netplay::Th07LanStageProbe
