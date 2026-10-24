#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C" {
void death_resource_begin(int loadout, int stock, int fragments) {
  startGame(loadout / 2, loadout % 2, 3, 4796);
  frame = 120; px = 111; py = 54144;
  lives = 4; resources.lifeFragments = 2; bombs = stock;
  resources.bombFragments = fragments; resources.rank = -300; power = 100;
  invuln = 0;
}
void death_resource_collision() { playerMissCollision(); }
void death_resource_window() { playerStateTicks = 7; }
void death_resource_tick(int held) { tickPlayer(held, 0); }
const int* death_resource_snapshot() {
  static int row[10];
  row[0] = playerState; row[1] = playerStateTicks; row[2] = invuln;
  row[3] = lives; row[4] = resources.lifeFragments; row[5] = bombs;
  row[6] = resources.bombFragments; row[7] = resources.rank;
  row[8] = power; row[9] = playerBomb.active;
  return row;
}
}
