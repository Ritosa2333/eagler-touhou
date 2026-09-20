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
    i64 score = 0;
    i32 life = 0;
    i32 bomb = 0;
    i32 power = 0;
    i32 graze = 0;
    i32 point = 0;
    i32 pointTotal = 0;
    i32 pointStage = 0;
    i32 cherry = 0;
    i32 cherryMax = 0;
    i32 cherryPlus = 0;
    i32 spellBonus = 0;
    i32 rank = 0;
    bool rankLock = false;
};

void RefreshFromHost();
void SetConfig(const Config &config);
const Config &GetConfig();
bool Active();
bool Enabled();
bool AdvancedActive();
i32 EffectivePlayerShot(i32 vanillaShot);
bool SuppressStageIntroTitles();
bool AdvancedSectionActive();
i32 InitialBgmIndex();
void FilterUnpauseInput();
void ReplayMenuReset();
bool ReplayMenuCheck(const char *replayPath);
void ReplayMenuActivate();
bool ReplayPlaybackActive();
bool ReplayStartupCommitted();
void FinishReplayStartup();
void UpdateOverlay();
void DrawOverlay();
bool AdvancedOptionsOpen();
bool CapturesGameInput();
bool OverlayInvincible();
bool OverlayInfiniteLives();
bool OverlayInfiniteBombs();
bool OverlayInfinitePower();
bool OverlayTimeLock();
bool OverlayAutoBomb();
bool OverlayEverlastingBgm();
bool AdvancedAllClearBonus();
bool AdvancedFixSpellBonusDisplay();
bool ConsumeResurrectionButterflySpawnSkip();
void ResetTracker();
void RecordBorderBreak();
void SetSupervisorFadeOutScope(bool active);
bool FilterAudioCommand(i32 opcode, i32 arg);
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
} // namespace PracticeRuntime
