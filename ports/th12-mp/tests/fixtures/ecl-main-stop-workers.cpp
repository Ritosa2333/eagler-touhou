#include "../../src/game/GameState.hpp"
#include "../../src/game/GameSession.hpp"
using namespace th12;
extern "C" {
void context_workers_setup(int seed) {
  startGame(0, 0, 1, seed);
  // The native sample's explicit Enemy412990 request is25,100. No workers
  // execute during constructor setup, so align the actor before Enemy18.
  auto& enemy = *enemies.front();
  enemy.x = enemy.ax = 25;
  enemy.y = enemy.ay = 100;
}
void context_workers_tick() {
  // Same selected-manager boundary as the independent native sample. This
  // fixture deliberately omits Player16/Bullet21/Primary27, without editing
  // their gameplay code or changing any scheduler inside tickEnemies.
  tickEnemies();
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                              [](const auto& enemy) { return !enemy->active; }),
                enemies.end());
}
}
