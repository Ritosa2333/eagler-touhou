#pragma once

#include "inttypes.hpp"
#include <SDL3/SDL.h>

namespace Touch
{
constexpr i32 DEATHBOMB_TOLERANCE = 5;

struct ReplayTouchPoint
{
    f32 x;
    f32 y;
};

void FingerDown(const SDL_TouchFingerEvent &f);
void FingerUp(const SDL_TouchFingerEvent &f);
void FingerMotion(const SDL_TouchFingerEvent &f);

u16 GetButtonBits();

bool IsFocus();
bool IsUnlimited();
bool GetFreeJoystickVector(f32 *x, f32 *y);

bool GetPlayerDelta(f32 *dx, f32 *dy);
void SetPlayerDelta(f32 dx, f32 dy);
void ConsumePlayerDelta(f32 dx, f32 dy);

void BeginReplayTouchFrame();
void ApplyReplayTouchEvent(i32 fingerId, f32 x, f32 y, u32 action, u32 role, u32 flags);
i32 GetReplayTouchPoints(ReplayTouchPoint *points, i32 capacity);
void ResetReplayTouch();

bool WasUsedThisRun();
bool UsedCheatMovementThisRun();
bool UsedTouchToBomb();
void SetReplayUsageState(bool usedThisRun, bool bombedWithTouch, bool cheatMovementUsed);
void ResetRunUsage();
void CancelTouches();

#ifdef TH_DEV_TOOLS
bool DebugStateSelfTest();
#endif
} // namespace Touch

