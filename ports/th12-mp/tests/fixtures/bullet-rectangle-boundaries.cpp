#include "../../src/bullet.hpp"
extern "C" int rectangle_collision(int type, float x, float y, float playerX, float playerY,
                                   float playerHit) {
  th12::eb::State state;
  state.phase = th12::eb::Phase::Alive;
  state.type = type;
  state.x = x;
  state.y = y;
  return int(th12::eb::collide(state, playerX, playerY, playerHit));
}
