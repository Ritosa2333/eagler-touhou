#include "PracticeRuntime.hpp"

namespace PracticeRuntime
{
static Config g_Config;
void RefreshFromHost() { g_Config = {}; }
void SetConfig(const Config &) { g_Config = {}; }
const Config &GetConfig() { return g_Config; }
bool Active() { return false; }
bool Enabled() { return false; }
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
bool DebugRestartPreservesConfig() { return false; }
}
