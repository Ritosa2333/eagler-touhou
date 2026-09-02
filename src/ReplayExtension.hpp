#pragma once

#include <cstddef>
#include <string>

#include "inttypes.hpp"
#include "netplay/NetplayProtocol.hpp"

namespace ReplayExtension
{
struct MultiplayerReplayConfig
{
    u8 playerCount = 1;
    u8 difficulty = 1;
    u8 localPlayer = 0;
    bool showContributionStats = true;
    u8 characters[3] = {};
    u8 shots[3] = {};
    u32 gameplayAbi = 0;
};

struct MultiplayerPlayerResourceSnapshot
{
    i32 lives = 0;
    i32 bombs = 0;
    i32 power = 0;
};

struct MultiplayerContributionSnapshot
{
    u32 enemiesDefeated = 0;
    u32 damageDealt = 0;
};

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
void BeginMultiplayerRecording(const MultiplayerReplayConfig &config);
void CaptureMultiplayerStageResources(i32 stage,
                                      const MultiplayerPlayerResourceSnapshot *resources,
                                      std::size_t count);
void CaptureMultiplayerStageContributions(
    i32 stage, const MultiplayerContributionSnapshot *contributions,
    std::size_t count);
void RecordMultiplayerFrame(i32 stage, i32 frame, const Netplay::FrameInput *inputs,
                            std::size_t count);

std::string ResolveSavePath(const char *requestedPath);
void RemoveAlternateSave(const char *savedPath);
bool AppendRecording(const char *path);

void ClearPlayback();
bool LoadPlayback(const u8 *bytes, std::size_t size);
void SetPlaybackFrame(i32 stage, i32 frame);
bool PlaybackActive();
bool MultiplayerPlaybackActive();
bool GetMultiplayerPlaybackConfig(MultiplayerReplayConfig *out);
bool GetMultiplayerPlaybackStageResources(i32 stage, u8 playerId,
                                          MultiplayerPlayerResourceSnapshot *out);
bool GetMultiplayerPlaybackStageContributions(
    i32 stage, u8 playerId, MultiplayerContributionSnapshot *out);
bool GetMultiplayerPlaybackFrame(i32 stage, i32 frame, Netplay::FrameInput *inputs,
                                 std::size_t count);
bool UsesFixedTickTouchPlayback();
bool GetPlaybackJoystick(f32 *x, f32 *y);
bool GetPlaybackDirectTouch(f32 *x, f32 *y, bool *unlimited);
bool GetPlaybackTouchState(bool *usedThisRun, bool *bombedWithTouch, bool *cheatMovementUsed);
const TouchEvent *GetPlaybackTouchEvents(std::size_t *count);

std::size_t BaseFileSize(const u8 *bytes, std::size_t size);
bool MatchesPath(const char *path, const u8 *bytes, std::size_t size);

#ifdef TH_DEV_TOOLS
bool DebugRoundTrip(const char *path);
void DebugResetMultiplayerPlaybackAudit();
void DebugExpectMultiplayerPlaybackFrame(i32 stage, i32 frame,
                                         const Netplay::FrameInput *inputs,
                                         std::size_t count);
int DebugAuditMultiplayerPlaybackFrame(i32 stage, i32 frame,
                                       const Netplay::FrameInput *inputs,
                                       std::size_t count);
u32 DebugExpectedMultiplayerPlaybackFrames();
u32 DebugComparedMultiplayerPlaybackFrames();
bool DebugMultiplayerPlaybackMismatch();
u32 DebugExpectedMultiplayerPlaybackCoverage();
#endif
} // namespace ReplayExtension
