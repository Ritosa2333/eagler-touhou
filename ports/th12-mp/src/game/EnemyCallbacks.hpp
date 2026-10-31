#pragma once
#include "GameTypes.hpp"
#include "SpecialEnemyCallbacks.hpp"

namespace th12 {
extern std::vector<special_enemy::ScorePopup> scorePopups;
int runEnemyCallback(Enemy&, int selector);
int queryPlayerDamage(const Enemy&, float x, float y, float width, float height);
int customEnemyDamage(Enemy&);
void customEnemyCollision(Enemy&);
} // namespace th12
