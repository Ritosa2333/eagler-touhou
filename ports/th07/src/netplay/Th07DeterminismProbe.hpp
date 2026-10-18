#pragma once

namespace Netplay::Th07DeterminismProbe
{
// Called immediately around exactly one canonical fixed-60-Hz simulation tick.
// Ordinary builds are transparent unless debugHarness=netplay-dual-stage1.
void BeforeSimulationTick();
void AfterSimulationTick(int chainResult);

// Built-in demo replay oracle. These hooks are inert unless
// debugHarness=demo-determinism and run at the replay-frame boundary rather
// than the outer display/fixed-tick boundary, because TH07's demo callback can
// restart the calc chain and consume multiple replay frames in one tick.
void BeforeDemoReplayFrame();
void AfterDemoReplayFrame();
} // namespace Netplay::Th07DeterminismProbe
