#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C" {
void player_bomb_age_prepare(int state, int age, int stock, int active) {
  startGame(2, 1, 3, 4796);
  bombs = stock;
  if (active)
    startPlayerBomb();
  if (state == 4) {
    invuln = 0;
    playerMissCollision();
  }
  playerStateTicks = age;
}
const int* player_bomb_age_update(int held) {
  tickPlayer(held, 0);
  static int result[6];
  result[0] = playerState;
  result[1] = playerStateTicks;
  result[2] = invuln;
  result[3] = bombs;
  result[4] = lives;
  result[5] = playerBomb.active;
  return result;
}
}
