#pragma once

#include <array>
#include <cstddef>
#include <eagler/netplay/DirectTouchState.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>

namespace Netplay::Input
{
using FrameInput = Netplay::FrameInput;
using AnalogMode = Netplay::AnalogMode;

// A speculative pass samples physical producers exactly once. The committed
// replay pass then uses the captured logical input without touching SDL/DOM or
// consuming Touch state a second time.
void BeginCapture();
FrameInput EndCapture();
bool CaptureActive();

// The normal path is transparent: ResolveLocal() returns physicalBits. While
// a capture is active it also records the fully composed button word.
std::uint16_t ResolveLocal(std::uint16_t physicalBits);
void CaptureJoystick(float x, float y);
void CaptureDirectTouch(float x, float y, bool unlimited);
void CaptureDirectTouchDelta(float x, float y, bool unlimited, bool begin);

using DirectTouchStates = std::array<DirectTouchState, MAX_PLAYERS>;
DirectTouchStates &GetDirectTouchStates();
void ResetDirectTouchStates();
void ResetPlayerDirectTouch(std::size_t player);
bool UsesIncrementalDirectTouch(std::size_t player);
void SetDirectTouchRemainder(std::size_t player, float x, float y);
void ConsumeDirectTouchRemainder(std::size_t player, float x, float y);

void SetReplayOverride(const FrameInput &input);
void SetReplayOverride(std::uint16_t bits);
void ClearReplayOverride();
bool ReplayOverrideActive();
bool ReplayJoystick(std::size_t player, float *x, float *y);
bool ReplayDirectTouch(std::size_t player, float *x, float *y, bool *unlimited);
bool PlayerTouchUsed(std::size_t player);
bool PlayerTouchBomb(std::size_t player);

// Per-player logical buttons are installed immediately before ReplayManager's
// fixed-tick input handoff. This keeps Player transport-agnostic: gameplay
// only reads the title-owned current input lanes, while netplay owns when
// these words are supplied for a predicted/resimulated frame.
void SetPlayerButtonOverrides(const std::uint16_t *buttons, std::size_t count);
void SetPlayerInputOverrides(const FrameInput *inputs, std::size_t count);
void ClearPlayerButtonOverrides();
bool PlayerButtonOverridesActive();
std::uint16_t PlayerButtonOverride(std::size_t player);
bool PlayerInputOverride(std::size_t player, FrameInput *out);
} // namespace Netplay::Input
