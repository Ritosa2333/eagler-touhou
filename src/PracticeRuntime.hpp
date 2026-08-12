#pragma once

#include "inttypes.hpp"

struct GameManager;

namespace PracticeRuntime
{
struct Config
{
    bool active = false;
    i32 mode = 0;
    i32 stage = 0;
    i32 warp = 0;
    i32 section = 0;
    i32 phase = 0;
    i32 frame = 0;
    bool dialogue = false;
    std::int64_t score = 0;
    i32 life = 8;
    i32 bomb = 8;
    i32 power = 128;
    i32 graze = 0;
    i32 point = 0;
    i32 rank = 32;
    bool rankLock = false;
    i32 fakeType = 0;
};

void RefreshFromHost();
void SetConfig(const Config &config);
const Config &GetConfig();
bool Active();
bool Enabled();
enum class MenuResult { Waiting, Accepted, Cancelled };
void OpenPracticeMenu(i32 difficulty, i32 shotType);
MenuResult PollPracticeMenu();
void DrawPracticeMenu();
void DebugAcceptPracticeMenu();
bool UpdatePauseMenu();
void DrawPauseMenuPanel();
void PrepareStart(GameManager &gameManager);
void ApplyInitialState(GameManager &gameManager, bool applyStats);
i32 ResolveWarpFrame(i32 stage, i32 portion);
bool LoadReplayMetadata(const char *replayPath);
bool SaveReplayMetadata(const char *replayPath);
bool DebugReplayMetadataRoundTrip(const char *replayPath);
bool DebugRestartPreservesConfig();
} // namespace PracticeRuntime
