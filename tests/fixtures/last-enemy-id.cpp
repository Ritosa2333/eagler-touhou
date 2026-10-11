#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C" {
void last_enemy_setup(int seed, int difficulty) { startGame(0, 0, difficulty, seed); }
void last_enemy_tick() {
  // Match the independently captured selected Enemy18 manager boundary.
  tickEnemies();
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                              [](const auto& enemy) { return !enemy->active; }),
                enemies.end());
}
unsigned last_enemy_registered() { return lastSpawnedEnemyId; }
unsigned last_enemy_next() { return nextEnemyId; }
unsigned last_enemy_get_int(unsigned value) {
  lastSpawnedEnemyId = value;
  return unsigned(globalInt(*enemies.front(), -9931));
}
float last_enemy_get_float(unsigned value) {
  lastSpawnedEnemyId = value;
  return global(*enemies.front(), -9931, true);
}
}
