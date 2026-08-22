#pragma once

#include <cstddef>
#include <string>

#include "inttypes.hpp"

namespace ReplayExtension
{
enum TouchAction : u32
{
    TOUCH_ACTION_DOWN = 1,
    TOUCH_ACTION_MOTION = 2,
    TOUCH_ACTION_UP = 3,
    TOUCH_ACTION_CANCEL_ALL = 4,
};

enum TouchRole : u32
{
    TOUCH_ROLE_NONE = 0,
    TOUCH_ROLE_MOVE = 1,
    TOUCH_ROLE_FOCUS = 2,
    TOUCH_ROLE_DIALOGUE = 3,
};

enum TouchFlags : u32
{
    TOUCH_FLAG_UNLIMITED = 1,
};

struct TouchEvent
{
    u32 frame;
    i32 fingerId;
    f32 x;
    f32 y;
    u32 action;
    u32 role;
    u32 flags;
};

void ResetRecording();
void BeginStageRecording(i32 stage);
void BeginInputFrame();
void CaptureJoystick(f32 x, f32 y);
void CaptureDirectTouch(f32 x, f32 y, bool unlimited);
void CaptureTouchState(bool usedThisRun, bool bombedWithTouch, bool cheatMovementUsed);
void CaptureTouchEvent(i32 fingerId, f32 x, f32 y, u32 action, u32 role, u32 flags);
void CaptureTouchCancelAll();
void RecordFrame(i32 stage, i32 frame);

std::string ResolveSavePath(const char *requestedPath);
void RemoveAlternateSave(const char *savedPath);
bool AppendRecording(const char *path);

void ClearPlayback();
bool LoadPlayback(const u8 *bytes, std::size_t size);
void SetPlaybackFrame(i32 stage, i32 frame);
bool PlaybackActive();
bool UsesFixedTickTouchPlayback();
bool GetPlaybackJoystick(f32 *x, f32 *y);
bool GetPlaybackDirectTouch(f32 *x, f32 *y, bool *unlimited);
bool GetPlaybackTouchState(bool *usedThisRun, bool *bombedWithTouch, bool *cheatMovementUsed);
const TouchEvent *GetPlaybackTouchEvents(std::size_t *count);

std::size_t BaseFileSize(const u8 *bytes, std::size_t size);
bool MatchesPath(const char *path, const u8 *bytes, std::size_t size);

#ifdef TH_DEV_TOOLS
bool DebugRoundTrip(const char *path);
#endif
} // namespace ReplayExtension
