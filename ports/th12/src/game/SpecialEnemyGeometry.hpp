// TH12 ECL 530 / 531 selector 1, independent of the core's object storage.
#pragma once

#include <cstdint>
#include <functional>

namespace th12::special_geometry {

struct Position {
  float x = 0, y = 0, z = 0;
};

struct EnemyView {
  uint32_t id = 0;
  Position position;
  int32_t health = 0, maximumHealth = 0;
  uint32_t flags = 0;
  int32_t age = 0;
  float privateFloat0 = 0, privateFloat1 = 0;
};

struct AnimationView {
  bool present = false;
  float rotationX = 0, rotationZ = 0, scaleY = 1;
  float spriteWidth = 0, spriteHeight = 0;
  uint32_t flags = 0;
};

struct DamageArea {
  Position center;
  float width = 16, height = 16;
  int sample = 0;
};

struct Circle {
  Position center;
  float radius = 40;
};

struct Line {
  Position origin;
  float angle = 0, transverseSize = 0, longitudinalSize = 0;
};

enum class Collision : int32_t { None = 0, Hit = 1, Graze = 2 };

struct World {
  // A COMPLETE 439ed0 query: friendly impact/phase callbacks, Source damage
  // accumulation/limit, Bomb query, cap80 and positive-query score award.
  // All 31 queries run synchronously, even if the eventual damage is capped.
  std::function<int32_t(uint32_t, const DamageArea&)> damageQuery;
  // 437980 / 437a80 semantics, including Player state changes on a hit. The
  // second call observes effects of the first. A line extends along positive
  // local X from origin; it is not a rectangle centered at the origin.
  std::function<Collision(const Circle&)> circleCollision;
  std::function<Collision(const Line&)> lineCollision;
  std::function<Position()> playerPosition;
  std::function<void(const Position&)> graze;
};

enum class Fault {
  None,
  MissingAnimation,
  ZeroMaximumHealth,
  MissingDamageQuery,
  MissingCircleQuery,
  MissingLineQuery,
  MissingGrazeHook,
};

struct HurtResult {
  int32_t damage = 0, rawTotal = 0, queryCount = 0;
  Fault fault = Fault::None;
};

struct CollisionResult {
  Collision circle = Collision::None, line = Collision::None;
  int32_t grazeCount = 0;
  Fault fault = Fault::None;
  // The native 41b4d0 callback always returns zero. Hit and graze are World
  // effects; do not interpret this function's return as a new hurt query.
  int32_t nativeReturn = 0;
};

HurtResult hurtSelector1(const EnemyView&, AnimationView&, const World&);
CollisionResult collisionSelector1(const EnemyView&, const AnimationView&, const World&);

} // namespace th12::special_geometry
