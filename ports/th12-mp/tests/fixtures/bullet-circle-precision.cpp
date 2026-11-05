#include "../../src/bullet.hpp"
extern "C" {
int circle_collision(int type, float centerX, float centerY, float playerX, float playerY,
                     float playerHit) {
  th12::eb::State bullet;
  bullet.phase = th12::eb::Phase::Alive;
  bullet.type = type;
  bullet.x = centerX;
  bullet.y = centerY;
  return int(th12::eb::collide(bullet, playerX, playerY, playerHit));
}
}
