#include "../../src/game/SpecialEnemyCallbacks.hpp"
#include <array>
#include <vector>
using namespace th12::special_enemy;

namespace {
EnemyView enemy;
std::array<BulletView, 2000> bullets;
std::array<float, 2000> zVelocity;
std::vector<ScorePopup> popups;
int32_t bossCounter = 0, score = 0;
bool bossAlias = false, hasBoss = true, hasAnimation = true;
float width = 13, height = 17;
uint32_t animationFlags = 0x400;
Result last;
std::array<float, 16> snapshot;

World world() {
  World w;
  w.bulletSlotCount = [] { return bullets.size(); };
  w.readBullet = [](size_t slot, BulletView& out) {
    out = bullets[slot];
    return true;
  };
  w.writeBulletVelocity = [](size_t slot, const VelocityHeading& v) {
    bullets[slot].vx = v.vx;
    bullets[slot].vy = v.vy;
    bullets[slot].heading = v.heading;
  };
  w.primaryBossRewardCounter = []() -> int32_t* {
    return hasBoss ? (bossAlias ? &enemy.privateIntegers[3] : &bossCounter) : nullptr;
  };
  w.scoreStored = &score;
  w.showScorePopup = [](const ScorePopup& popup) { popups.push_back(popup); };
  w.writePrimaryAnimationWidth = [](uint32_t id, float value) {
    if (!hasAnimation || id != enemy.id)
      return false;
    width = value;
    animationFlags |= 8;
    return true;
  };
  return w;
}
} // namespace

extern "C" {
void reset() {
  enemy = {};
  enemy.id = 77;
  bullets = {};
  zVelocity = {};
  popups.clear();
  bossCounter = score = 0;
  bossAlias = false;
  hasBoss = hasAnimation = true;
  width = 13;
  height = 17;
  animationFlags = 0x400;
  last = {};
}
void set_enemy(float x, float y, float z, int age, int p0, int p1, int p2, int p3) {
  enemy.position = {x, y, z};
  enemy.age = age;
  enemy.privateIntegers = {p0, p1, p2, p3};
}
void set_resources(int value, int counter, int alias) {
  score = value;
  bossCounter = counter;
  bossAlias = alias;
  if (alias)
    enemy.privateIntegers[3] = counter;
}
void set_bullet(int slot, int flags, int phase, int tag, float x, float y, float z,
                float vx, float vy, float vz, float speed, float heading) {
  bullets.at(slot) = {uint32_t(flags), uint16_t(phase), tag, {x, y, z}, vx, vy, heading, speed};
  zVelocity.at(slot) = vz;
}
void set_missing(int boss, int animation) {
  hasBoss = !boss;
  hasAnimation = !animation;
}
int run_callback(int selector) {
  last = run(selector, enemy, world());
  return last.nativeReturn;
}
int fault() { return int(last.fault); }
int affected() { return int(last.affectedBullets); }
int private_integer(int index) { return enemy.privateIntegers.at(index); }
int score_stored() { return score; }
int primary_counter() { return bossAlias ? enemy.privateIntegers[3] : bossCounter; }
const float* bullet_state(int slot) {
  const auto& b = bullets.at(slot);
  snapshot = {b.position.x, b.position.y, b.position.z, b.vx, b.vy, zVelocity.at(slot),
              b.nominalSpeed, b.heading};
  return snapshot.data();
}
int bullet_field(int slot, int field) {
  const auto& b = bullets.at(slot);
  return field == 0 ? int(b.objectFlags) : field == 1 ? b.phase : b.tag;
}
int popup_count() { return int(popups.size()); }
const float* popup_state(int index) {
  const auto& p = popups.at(index);
  snapshot = {p.screenPosition.x, p.screenPosition.y, p.screenPosition.z, p.scaleX, p.scaleY};
  return snapshot.data();
}
int popup_field(int index, int field) {
  const auto& p = popups.at(index);
  switch (field) {
  case 0: return p.displayedValue;
  case 1: return p.font;
  case 2: return int(p.color);
  case 3: return p.horizontalAlignment;
  case 4: return p.verticalAlignment;
  default: return p.nativeTextFlag;
  }
}
const float* animation_state() {
  snapshot = {width, height};
  return snapshot.data();
}
int animation_flags() { return int(animationFlags); }
}
