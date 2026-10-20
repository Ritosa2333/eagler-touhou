#include "../../src/bullet.hpp"
#include "../../src/game/LaserCollision.hpp"
extern "C" {
int graze_bullet_query(float x, float y, int type, int phase, int collisionAge, float px,
                       float py, float hit) {
  th12::eb::State state;
  state.x=x;state.y=y;state.type=type;
  state.phase=th12::eb::Phase(phase);state.collisionAge=collisionAge;
  return int(th12::eb::collide(state,px,py,hit));
}
int graze_laser_query(float x, float y, float angle, float transverse, float longitudinal,
                     float px, float py, float hit, int state, int invulnerability,
                     int dialogue, int flags) {
  th12::laser::PlayerCollision player;
  player.position={px,py,0};player.hitWidth=player.hitHeight=hit*.5f;
  player.state=state;player.invulnerability=invulnerability;
  player.dialogue=dialogue;player.flags=flags;
  return int(th12::laser::collidePlayerLine({x,y,0},angle,transverse,longitudinal,player).code);
}
int graze_enemy_circle_query(float x, float y, float radius, float px, float py,
                            float hitRadius, int state, int invulnerability, int dialogue) {
  th12::laser::PlayerCollision player;
  player.position={px,py,0};player.circleHitRadius=hitRadius;
  player.state=state;player.invulnerability=invulnerability;player.dialogue=dialogue;
  return int(th12::laser::collidePlayerCircle({x,y,0},radius,player).code);
}
}
