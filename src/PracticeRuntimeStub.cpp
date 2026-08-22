#include "PracticeRuntime.hpp"

namespace PracticeRuntime
{
static Config g_Config;
void RefreshFromHost() { g_Config = {}; }
void SetConfig(const Config &) { g_Config = {}; }
const Config &GetConfig() { return g_Config; }
void SyncRuntimeDerivedSession() {}
bool Active() { return false; }
bool Enabled() { return false; }
bool AdvancedActive() { return false; }
i32 EffectivePlayerShot(i32 vanillaShot) { return vanillaShot; }
bool ForceFlandreFinalRage() { return false; }
i32 InitialBgmIndex() { return 0; }
void ApplyPendingBossSectionSfxFix() {}
void FilterUnpauseInput() {}
void UpdateOverlay() {}
void DrawOverlay() {}
bool ConsumeScreenshotRequest() { return false; }
bool AdvancedOptionsOpen() { return false; }
bool OverlayInvincible() { return false; }
bool OverlayInfiniteLives() { return false; }
bool OverlayInfiniteBombs() { return false; }
bool OverlayInfinitePower() { return false; }
bool OverlayTimeLock() { return false; }
bool OverlayAutoBomb() { return false; }
bool OverlayEverlastingBgm() { return false; }
void ResetTracker() {}
void RecordTrackerMiss() {}
void ResetBgmTracking() {}
void NotifyBgmPlay(const char *) {}
bool PreserveBgmOnRestart() { return false; }
void FinishBgmRestartPreservation() {}
bool SuppressStageIntroTitles() { return false; }
void OpenPracticeMenu(i32, i32) {}
MenuResult PollPracticeMenu() { return MenuResult::Cancelled; }
void DrawPracticeMenu() {}
void DebugAcceptPracticeMenu() {}
bool UpdatePauseMenu() { return false; }
void DrawPauseMenuPanel() {}
bool ConsumeResultReplaySaveRequest() { return false; }
void PrepareStart(GameManager &) {}
void ApplyInitialState(GameManager &, bool) {}
i32 ResolveWarpFrame(i32, i32) { return 0; }
bool LoadReplayMetadata(const char *) { return false; }
bool SaveReplayMetadata(const char *) { return false; }
void ReplayMenuReset() {}
bool ReplayMenuCheck(const char *) { return false; }
void ReplayMenuActivate() {}
bool ReplayPlaybackActive() { return false; }
bool ReplayStartupCommitted() { return false; }
void FinishReplayStartup() {}
bool DebugReplayMetadataRoundTrip(const char *) { return false; }
bool DebugRestartPreservesConfig() { return false; }
bool DebugSectionCatalogSelfTest() { return true; }
}
