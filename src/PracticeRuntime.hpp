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
    i32 life = 0;
    i32 bomb = 0;
    i32 power = 0;
    i32 graze = 0;
    i32 point = 0;
    i32 rank = 0;
    bool rankLock = false;
    i32 fakeType = 0;
};

void RefreshFromHost();
void SetConfig(const Config &config);
const Config &GetConfig();
void SyncRuntimeDerivedSession();
bool Active();
bool Enabled();
bool AdvancedActive();
i32 EffectivePlayerShot(i32 vanillaShot);
bool ForceFlandreFinalRage();
i32 InitialBgmIndex();
void ApplyPendingBossSectionSfxFix();
void FilterUnpauseInput();
void UpdateOverlay();
void DrawOverlay();
bool ConsumeScreenshotRequest();
bool AdvancedOptionsOpen();
bool OverlayInvincible();
bool OverlayInfiniteLives();
bool OverlayInfiniteBombs();
bool OverlayInfinitePower();
bool OverlayTimeLock();
bool OverlayAutoBomb();
bool OverlayEverlastingBgm();
void ResetReplayDeterminismUsage();
bool ReplayUnsafeAssistUsedThisRun();
void ResetTracker();
void RecordTrackerMiss();
void ResetBgmTracking();
void NotifyBgmPlay(const char *path);
bool PreserveBgmOnRestart();
void FinishBgmRestartPreservation();
bool SuppressStageIntroTitles();
enum class MenuResult { Waiting, Accepted, Cancelled };
void OpenPracticeMenu(i32 difficulty, i32 shotType);
MenuResult PollPracticeMenu();
void DrawPracticeMenu();
void DebugAcceptPracticeMenu();
#ifdef TH_DEV_TOOLS
bool DebugLoadSessionFile(const char *path);
#endif
bool UpdatePauseMenu();
void DrawPauseMenuPanel();
bool ConsumeResultReplaySaveRequest();
void PrepareStart(GameManager &gameManager);
void ApplyInitialState(GameManager &gameManager, bool applyStats);
i32 ResolveWarpFrame(i32 stage, i32 portion);
bool LoadReplayMetadata(const char *replayPath);
bool SaveReplayMetadata(const char *replayPath);
void ReplayMenuReset();
bool ReplayMenuCheck(const char *replayPath);
void ReplayMenuActivate();
bool ReplayPlaybackActive();
bool ReplayStartupCommitted();
void FinishReplayStartup();
bool DebugReplayMetadataRoundTrip(const char *replayPath);
bool DebugRestartPreservesConfig();
bool DebugSectionCatalogSelfTest();
} // namespace PracticeRuntime
