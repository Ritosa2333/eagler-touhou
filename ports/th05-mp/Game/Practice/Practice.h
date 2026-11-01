#pragma once
#include <windows.h>
#include "PracticeConfig.h"
#include "PracticeAdvanced.h"
namespace th5w {
class CStage;
namespace practice {
enum Tool { Invincible, InfiniteLives, LockDream, InfiniteBombs, KeepPower, LockTimeout, AutoBomb, EnemyInvincible, ToolCount };
bool Enabled(Tool tool);
void PreparingSection(bool value);
void Toggle(Tool tool);
void ResetRun();
bool Assisted();
bool InGameInfoOpen();
void MarkAssisted();
unsigned int TimeoutLockRevision();
bool AllowTimeout(bool expired);
bool BeforeTimeout(bool beforeDeadline);
bool Initialize(HWND window);
void Shutdown();
void BeginFrame();
void EndFrame();
void WindowMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void FocusPause();
void DrawPause(CStage& stage);
void DrawStatus();
void OpenSetup(int stage);
int DrawSetup(int age); // 0 = editing, 1 = start, 2 = return
bool StartSetup();
Config& MenuConfig();
}
}
