#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C" {
void player_bomb_position_prepare(int loadout, int state, int age, int stock, int x, int y,
                                  int messageActive, int liveEnemies, int establishRespawn) {
  startGame(loadout / 2, loadout % 2, 3, 4796);
  if (establishRespawn) {
    px = 20 * 128;
    py = 300 * 128;
    playerState = 2;
    playerStateTicks = 30;
    tickPlayer(0, 0);
  }
  frame = 120;
  px = x;
  py = y;
  bombs = stock;
  if (state == 4) {
    invuln = 0;
    playerMissCollision();
  } else
    playerState = state;
  dialogue = messageActive;
  if (!liveEnemies)
    enemies.clear();
  playerStateTicks = age;
}
const float* player_bomb_position_update(int held) {
  tickPlayer(held, 0);
  static float result[9];
  result[0] = float(playerState);
  result[1] = float(playerStateTicks);
  result[2] = px / 128.f;
  result[3] = py / 128.f;
  result[4] = float(px);
  result[5] = float(py);
  result[6] = float(playerBomb.active);
  result[7] = playerBomb.x;
  result[8] = playerBomb.y;
  return result;
}
const float* player_bomb_position_sources() {
  static float result[1 + 128 * 8];
  int count = 0;
  for (const auto& s : playerDamageSources.sources)
    if (s.flags & 1) {
      const float row[]{s.x,
                        s.y,
                        s.radius,
                        s.growth,
                        float(s.previousTicks),
                        float(s.ticks),
                        float(s.damage),
                        float(s.flags)};
      std::copy(std::begin(row), std::end(row), result + 1 + count * 8);
      ++count;
    }
  result[0] = float(count);
  return result;
}
const int* player_bomb_position_options() {
  static int result[1 + 4 * 2];
  result[0] = playerMotion.count;
  for (int i = 0; i < playerMotion.count; ++i) {
    result[1 + i * 2] = playerMotion.options[i].x;
    result[2 + i * 2] = playerMotion.options[i].y;
  }
  return result;
}
const float* player_bomb_position_clears(int state, int age) {
  static float result[1 + 8 * 3];
  auto operations = playerClearOperations(state, age);
  if (state == 0 && age >= 60) {
    auto normal = playerClearOperations(1, 0);
    operations.insert(operations.end(), normal.begin(), normal.end());
  }
  result[0] = float(operations.size());
  for (size_t i = 0; i < operations.size(); ++i) {
    result[1 + i * 3] = float(operations[i].kind);
    result[2 + i * 3] = operations[i].radius;
    result[3 + i * 3] = operations[i].protection;
  }
  return result;
}
}
