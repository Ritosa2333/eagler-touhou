#include "../../src/game/SpecialEnemyGeometry.hpp"
#include <array>
#include <vector>
using namespace th12::special_geometry;
namespace {
EnemyView enemy;
AnimationView animation;
std::array<int, 31> answers{};
std::vector<DamageArea> areas;
std::vector<Position> grazes;
std::vector<int> order;
Circle circle;
Line line;
Position player;
int playerState = 1, afterCircleState = 1, afterLineState = 1, stateSeenByLine = -1;
int circleAnswer = 0, lineAnswer = 0;
HurtResult hurt;
CollisionResult collision;
std::array<float, 16> snapshot;
World world() {
  World w;
  w.damageQuery = [](uint32_t id, const DamageArea& area) {
    if (id != 77)
      return 0;
    areas.push_back(area);
    return answers.at(area.sample);
  };
  w.circleCollision = [](const Circle& value) {
    circle = value;
    order.push_back(1);
    playerState = afterCircleState;
    return Collision(circleAnswer);
  };
  w.lineCollision = [](const Line& value) {
    line = value;
    order.push_back(3);
    stateSeenByLine = playerState;
    playerState = afterLineState;
    return Collision(lineAnswer);
  };
  w.playerPosition = [] { return player; };
  w.graze = [](const Position& value) { grazes.push_back(value); order.push_back(2); };
  return w;
}
} // namespace
extern "C" {
void reset() {
  enemy = {};
  enemy.id = 77;
  animation = {true, 9, 0, 64, 99, 101, 0x400};
  answers = {};
  areas.clear();
  grazes.clear();
  order.clear();
  circle = {};
  line = {};
  player = {};
  playerState = afterCircleState = afterLineState = 1;
  stateSeenByLine = -1;
  circleAnswer = lineAnswer = 0;
  hurt = {};
  collision = {};
}
void set_enemy(float x, float y, float z, int hp, int maximum, int flags, int age,
               float f0, float f1) {
  enemy.position = {x, y, z};
  enemy.health = hp;
  enemy.maximumHealth = maximum;
  enemy.flags = uint32_t(flags);
  enemy.age = age;
  enemy.privateFloat0 = f0;
  enemy.privateFloat1 = f1;
}
void set_animation(float rx, float rz, float sy, float sw, float sh, int flags) {
  animation = {true, rx, rz, sy, sw, sh, uint32_t(flags)};
}
void set_answer(int index, int value) { answers.at(index) = value; }
void set_player(float x, float y, float z, int state, int firstState, int secondState,
                int firstCode, int secondCode) {
  player = {x, y, z};
  playerState = state;
  afterCircleState = firstState;
  afterLineState = secondState;
  circleAnswer = firstCode;
  lineAnswer = secondCode;
}
int run_hurt() {
  hurt = hurtSelector1(enemy, animation, world());
  return hurt.damage;
}
int hurt_total() { return hurt.rawTotal; }
int query_count() { return int(areas.size()); }
int hurt_fault() { return int(hurt.fault); }
int run_collision() {
  collision = collisionSelector1(enemy, animation, world());
  return collision.nativeReturn;
}
int collision_fault() { return int(collision.fault); }
int circle_code() { return int(collision.circle); }
int line_code() { return int(collision.line); }
int graze_count() { return int(grazes.size()); }
int line_player_state() { return stateSeenByLine; }
int final_player_state() { return playerState; }
const float* query_state(int index) {
  const auto& a = areas.at(index);
  snapshot = {a.center.x, a.center.y, a.center.z, a.width, a.height, float(a.sample)};
  return snapshot.data();
}
const float* circle_state() {
  snapshot = {circle.center.x, circle.center.y, circle.center.z, circle.radius};
  return snapshot.data();
}
const float* line_state() {
  snapshot = {line.origin.x, line.origin.y, line.origin.z, line.angle,
              line.transverseSize, line.longitudinalSize};
  return snapshot.data();
}
const float* graze_state(int index) {
  const auto& p = grazes.at(index);
  snapshot = {p.x, p.y, p.z};
  return snapshot.data();
}
const float* animation_state() {
  snapshot = {animation.rotationX, animation.rotationZ, animation.scaleY,
              animation.spriteWidth, animation.spriteHeight};
  return snapshot.data();
}
int animation_flags() { return int(animation.flags); }
int order_count() { return int(order.size()); }
int order_kind(int index) { return order.at(index); }
void set_missing_animation() { animation.present = false; }
}
