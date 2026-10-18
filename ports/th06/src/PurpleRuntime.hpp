#pragma once
#include "inttypes.hpp"
struct Enemy;
namespace PracticeRuntime::Purple {
u16 FilterInput(u16 buttons);
void RecordPlayerInput();
void DrawAdvanced();
void DrawIndicators();
void DrawTrackerExtras();
void FeedUiInput(u16 buttons);
bool EnemyInvincible();
bool DisableRankDrop();
bool PreserveLifeStock(int lives);
void OnMiss();
void GuiTick();
void SetBossBounds(Enemy* enemy);
void PrepareSeed();
void RememberWallBoss(const Enemy* enemy);
float AdjustWallAngle(const Enemy* enemy, float original);
float AdjustWallSnipe(float x, float y, float original);
void ResetLockTimer();
void TickLockTimer();
void CaptureBossLife(const Enemy* enemy);
float BossThresholdRatio(float healthRatio);
void SpellAttempt(int id);
void SpellCapture(int id);
void SpellTimeout(int id);
void GreyItem(bool inBomb);
void BookStart(unsigned offset,float x,float y);
void BookEnd(unsigned offset);
void SaveStatistics();
void ReleaseTextures();
void BookFailed();
double SimulationInterval();
bool BackgroundFix();
extern bool enemyInvincible;
}
