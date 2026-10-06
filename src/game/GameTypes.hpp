#pragma once
#include "../anm_scene.hpp"
#include "../bullet.hpp"
#include "../bullet_visuals.hpp"
#include "../enemy_motion.hpp"
#include "../message.hpp"
#include "../player.hpp"
#include "../player_weapons.hpp"
#include "../spell.hpp"
#include "../stage_award.hpp"
#include "../stage_logic.hpp"
#include "Items.hpp"
#include "Laser.hpp"
#include "Ufo.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iomanip>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace th12 {
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i32 = int32_t;
template <class T> T rd(const u8* p) {
  T x;
  std::memcpy(&x, p, sizeof x);
  return x;
}
inline float asfloat(u32 x) {
  float f;
  std::memcpy(&f, &x, 4);
  return f;
}
inline u32 asbits(float f) {
  u32 x;
  std::memcpy(&x, &f, 4);
  return x;
}
constexpr float pi = 3.1415927410125732f, tau = 6.2831854820251465f;
struct Random {
  u16 seed = 0;
  u32 calls = 0;
  u16 next16() {
    const u16 a = u16((seed ^ 0x9630) - 0x6553);
    seed = u16((a << 2) | (a >> 14));
    calls++;
    return seed;
  }
  u32 next32() {
    u16 a = u16((seed ^ 0x9630) - 0x6553), b = u16((a << 2) | (a >> 14)),
        c = u16((b ^ 0x9630) - 0x6553);
    seed = u16((c << 2) | (c >> 14));
    calls += 2;
    return u32(a) << 16 | c;
  }
  float unit() { return float(double(next32()) / 4294967296.0); }
  float signedUnit() { return float(double(next32()) / 2147483648.0 - 1); }
};
struct Program {
  std::vector<std::vector<u8>> files;
  std::unordered_map<std::string, const u8*> routines;
  bool add(const u8* input, u32 size);
  const u8* find(const std::string& name) const;
};
inline i32 nativeInt(float value) {
  return !std::isfinite(value) || double(value) < -2147483648.0 || double(value) >= 2147483648.0
             ? i32(0x80000000u)
             : i32(value);
}
struct Value {
  u32 bits = 0;
  bool integer = false;
  float floating() const { return integer ? float(i32(bits)) : asfloat(bits); }
  i32 integral() const { return integer ? i32(bits) : nativeInt(asfloat(bits)); }
};
struct CallFrame {
  const u8* pc;
  float time;
  std::array<u32, 256> locals;
};
struct Context {
  const u8* pc = nullptr;
  float time = 0;
  int id = -1;
  std::array<u32, 256> locals{};
  std::vector<Value> stack;
  std::vector<CallFrame> calls;
  Context(const u8* p = nullptr) : pc(p) {}
  Value popValue() {
    if (stack.empty())
      return {};
    Value value = stack.back();
    stack.pop_back();
    return value;
  }
  float pop() { return popValue().floating(); }
  i32 popInt() { return popValue().integral(); }
  void push(float f, bool integer = false) {
    stack.push_back({integer ? u32(nativeInt(f)) : asbits(f), integer});
  }
  void pushInt(i32 value) { stack.push_back({u32(value), true}); }
};
struct Shooter {
  float x = 0, y = 0, originX = 0, originY = 0, radius = 0, angle = 0, spacing = 0, speed = 2,
        slow = 0;
  int count = 1, layers = 1, mode = 0, type = 0, color = 0, fireSound = 22;
  bool fixedOrigin = false;
  eb::Program extensions{};
  laser::Parameters laserConfig{};
  Shooter() {
    extensions.flags = 0x83;
    extensions.transformSound = 40;
  }
};
struct EnemyInterrupt {
  int health = -1, time = -1;
  std::string healthScript, timeoutScript;
};
struct CircularMotion {
  bool enabled = false;
  float centerX = 0, centerY = 0, radius = 0, radialSpeed = 0, fromOmega = 0, toOmega = 0,
        fromRadius = 0, toRadius = 0, fromRadial = 0, toRadial = 0;
  int ticks = 0, time = 0, mode = 0;
};
struct Enemy {
  u32 id = 0;
  float x = 0, y = 0, ax = 0, ay = 0, angle = 0, speed = 0, rx = 0, ry = 0, relativeAngle = 0,
        relativeSpeed = 0, hit = 12;
  int life = 30, score = 1000, drop = 1, age = 0, bank = 1, boundBank = 1, animation = 0,
      baseAnimation = 0, animationAge = 0;
  bool active = true, boss = false, hidden = false, invincible = false, mirror = false,
       timedOut = false;
  u32 flags = 0;
  int immunityTicks = 2, bodyImmunityTicks = 0;
  // Native +265c life-control word is distinct from +26f8 behavior flags.
  u32 lifeFlags = 0;
  ufo_system::DeathReason deathReason = ufo_system::DeathReason::Defeated;
  std::array<u32, 12> variables{};
  std::array<EnemyInterrupt, 8> interrupts{};
  std::array<int, 19> dropCounts{};
  float dropSpreadX = 32, dropSpreadY = 32;
  float fromX = 0, fromY = 0, toX = 0, toY = 0, fromA = 0, toA = 0, fromS = 0, toS = 0;
  int positionTicks = 0, positionTime = 0, positionMode = 0, polarTicks = 0, polarTime = 0,
      polarMode = 0;
  float relativeFromX = 0, relativeFromY = 0, relativeToX = 0, relativeToY = 0, relativeFromA = 0,
        relativeToA = 0, relativeFromS = 0, relativeToS = 0;
  int relativePositionTicks = 0, relativePositionTime = 0, relativePositionMode = 0,
      relativePolarTicks = 0, relativePolarTime = 0, relativePolarMode = 0;
  float clampX = 0, clampY = 0, clampWidth = 0, clampHeight = 0;
  int phaseAge = 0;
  CircularMotion absoluteCircle{}, relativeCircle{};
  int maxLife = 30;
  float hitWidth = 24, hitHeight = 24, bodyWidth = 24, bodyHeight = 24, renderWidth = 0,
        renderHeight = 0;
  bool isUfo = false;
  std::array<int, 16> animationSlots{};
  std::array<int, 16> animationBanks{};
  std::array<bool, 16> animationBound{};
  std::list<Context> contexts;
  std::array<Shooter, 16> shooters{};
};
struct Projectile {
  float x, y, angle, speed, radius;
  int type, color, age = 0, damage = 0, animation = 0;
  bool friendly = false, active = true, grazed = false;
  int option = 0, update = 0;
  float hitWidth = 0, hitHeight = 0;
  u32 targetId = 0;
  int extraCallback = 0;
  u32 entityId = 0;
  eb::State enemy{};
  pw::State weapon{};
  int hitAnimation = 0, hitCallback = 0, spawnCallback = 0;
};

} // namespace th12
