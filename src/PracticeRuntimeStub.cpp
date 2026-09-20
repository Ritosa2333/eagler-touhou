#include "PracticeRuntime.hpp"

namespace PracticeRuntime
{
static Config g_Config;
void RefreshFromHost() { g_Config = {}; }
void SetConfig(const Config &) { g_Config = {}; }
const Config &GetConfig() { return g_Config; }
bool Active() { return false; }
bool Enabled() { return false; }
bool AdvancedActive() { return false; }
i32 EffectivePlayerShot(i32 vanillaShot) { return vanillaShot; }
bool SuppressStageIntroTitles() { return false; }
bool AdvancedSectionActive() { return false; }
i32 InitialBgmIndex() { return 0; }
void FilterUnpauseInput() {}
void ReplayMenuReset() {}
bool ReplayMenuCheck(const char *) { return false; }
void ReplayMenuActivate() {}
bool ReplayPlaybackActive() { return false; }
bool ReplayStartupCommitted() { return false; }
void FinishReplayStartup() {}
void UpdateOverlay() {}
void DrawOverlay() {}
bool AdvancedOptionsOpen() { return false; }
bool CapturesGameInput() { return false; }
bool OverlayInvincible() { return false; }
bool OverlayInfiniteLives() { return false; }
bool OverlayInfiniteBombs() { return false; }
bool OverlayInfinitePower() { return false; }
bool OverlayTimeLock() { return false; }
bool OverlayAutoBomb() { return false; }
bool OverlayEverlastingBgm() { return false; }
bool AdvancedAllClearBonus() { return false; }
bool AdvancedFixSpellBonusDisplay() { return false; }
bool ConsumeResurrectionButterflySpawnSkip() { return false; }
void ResetTracker() {}
void RecordBorderBreak() {}
void SetSupervisorFadeOutScope(bool) {}
bool FilterAudioCommand(i32, i32) { return false; }
void OpenPracticeMenu(i32, i32) {}
MenuResult PollPracticeMenu() { return MenuResult::Cancelled; }
void DrawPracticeMenu() {}
void DebugAcceptPracticeMenu() {}
bool UpdatePauseMenu() { return false; }
void DrawPauseMenuPanel() {}
void PrepareStart(GameManager &) {}
void ApplyInitialState(GameManager &, bool) {}
i32 ResolveWarpFrame(i32, i32) { return 0; }
bool LoadReplayMetadata(const char *) { return false; }
bool SaveReplayMetadata(const char *) { return false; }
bool DebugReplayMetadataRoundTrip(const char *) { return false; }
}
