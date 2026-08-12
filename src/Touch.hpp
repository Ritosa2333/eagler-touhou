#pragma once

#include "inttypes.hpp"
#include <SDL3/SDL.h>

namespace Touch
{
constexpr i32 DEATHBOMB_TOLERANCE = 5;

void FingerDown(const SDL_TouchFingerEvent &f);
void FingerUp(const SDL_TouchFingerEvent &f);
void FingerMotion(const SDL_TouchFingerEvent &f);

u16 GetButtonBits();

bool IsFocus();
bool IsUnlimited();

bool GetPlayerDelta(f32 *dx, f32 *dy);
void SetPlayerDelta(f32 dx, f32 dy);
void ConsumePlayerDelta(f32 dx, f32 dy);

bool WasUsedThisRun();
bool UsedTouchToBomb();
void ResetRunUsage();
void CancelTouches();
} // namespace Touch

