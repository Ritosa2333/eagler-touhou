#include "../../src/game/LaserCollision.hpp"
using namespace th12::laser;
LineCollisionResult result;
extern "C" {
int query_circle(float ox, float oy, float oz, float radius, float px, float py, float pz,
                 float hit, float hw, float hh, int state, int invuln, int flags, int dialogue) {
  PlayerCollision player;
  player.position = {px, py, pz};
  player.hitWidth = hw;
  player.hitHeight = hh;
  player.circleHitRadius = hit;
  player.state = state;
  player.invulnerability = invuln;
  player.flags = uint32_t(flags);
  player.dialogue = dialogue;
  result = collidePlayerCircle({ox, oy, oz}, radius, player);
  return int(result.code);
}
int begin_miss() { return result.beginMiss; }
int clears_laser() { return result.clearLaser; }
}
