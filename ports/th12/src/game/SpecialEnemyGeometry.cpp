#include "SpecialEnemyGeometry.hpp"
#include "../enemy_motion.hpp"
#include <bit>
#include <cmath>

namespace th12::special_geometry {
namespace {

Position pointOnCircle(const Position& origin, float angle, float radius) {
  // 41c580 uses real x87 FSINCOS and stores the two offset components as
  // float32 before the caller adds the enemy position. Neither callback
  // calls D3DX; the native probe executes FSINCOS, not a geometry stub.
  const float dx = float(std::cos(double(angle)) * radius);
  const float dy = float(std::sin(double(angle)) * radius);
  return {float(double(origin.x) + dx), float(double(origin.y) + dy), origin.z};
}

int32_t addNative(int32_t a, int32_t b) {
  return std::bit_cast<int32_t>(uint32_t(a) + uint32_t(b));
}

int32_t compressDamage(int32_t total) {
  if (total < 30)
    return total;
  // Preserve the original discontinuity: total96 returns69, total97 returns
  // 54. The second branch subtracts50, not70; do not smooth this curve.
  const int32_t tripled = std::bit_cast<int32_t>(uint32_t(total) * 3u - 90u);
  int32_t damage = addNative(30, tripled / 5);
  if (damage < 70)
    return damage;
  damage = addNative(50, addNative(damage, -50) / 5);
  return damage < 80 ? damage : 80;
}

bool grazeEnabled(const EnemyView& enemy) {
  return (enemy.flags & 0x200u) && enemy.age % 3 == 0;
}

void emitGraze(const World& world, CollisionResult& result) {
  if (!world.playerPosition || !world.graze) {
    result.fault = Fault::MissingGrazeHook;
    return;
  }
  world.graze(world.playerPosition());
  result.grazeCount++;
}

} // namespace

HurtResult hurtSelector1(const EnemyView& enemy, AnimationView& animation, const World& world) {
  if (!animation.present)
    return {0, 0, 0, Fault::MissingAnimation};
  if (enemy.maximumHealth == 0)
    return {0, 0, 0, Fault::ZeroMaximumHealth};
  if (!world.damageQuery)
    return {0, 0, 0, Fault::MissingDamageQuery};

  // Native constants contain the original float32-derived pi values. This
  // modifies VM+24 (rotationX), not scaleX, spriteWidth or the enemy hitbox.
  animation.rotationX =
      float(double(enemy.health) * 5.890486240386963 / enemy.maximumHealth + 0.39269909262657166);
  animation.flags |= 4u;
  const float first = float(double(animation.rotationZ) - double(animation.rotationX) * .5 +
                            double(animation.rotationX) / 64.0);
  float angle = enemy_motion::normalize(first);
  HurtResult result;
  for (int sample = 0; sample < 31; sample++) {
    DamageArea area;
    area.center = pointOnCircle(enemy.position, angle, animation.scaleY);
    area.sample = sample;
    result.rawTotal = addNative(result.rawTotal, world.damageQuery(enemy.id, area));
    result.queryCount++;
    const float step = float(double(animation.rotationX) / 32.0);
    angle = enemy_motion::normalize(float(double(angle) + step));
  }
  result.damage = compressDamage(result.rawTotal);
  return result;
}

CollisionResult collisionSelector1(const EnemyView& enemy, const AnimationView& animation,
                                   const World& world) {
  if (!animation.present)
    return {Collision::None, Collision::None, 0, Fault::MissingAnimation};
  if (!world.circleCollision)
    return {Collision::None, Collision::None, 0, Fault::MissingCircleQuery};
  if (!world.lineCollision)
    return {Collision::None, Collision::None, 0, Fault::MissingLineQuery};

  CollisionResult result;
  Circle circle;
  circle.center = pointOnCircle(enemy.position, animation.rotationZ, 24);
  result.circle = world.circleCollision(circle);
  if (result.circle == Collision::Graze && grazeEnabled(enemy))
    emitGraze(world, result);

  Line line;
  line.origin = enemy.position;
  line.angle = animation.rotationZ;
  line.transverseSize = enemy.privateFloat0;
  line.longitudinalSize = enemy.privateFloat1;
  result.line = world.lineCollision(line);
  if (result.line == Collision::Graze && grazeEnabled(enemy))
    emitGraze(world, result);
  return result;
}

} // namespace th12::special_geometry
