// TH12 1.00b EnemyState special callbacks 3/4/5 (41b740/41bb40/41bd20).
#pragma once
#include "Laser.hpp"
#include <span>

namespace th12::laser {
struct CallbackShooter {
  eb::Emission emission{};
  Vec3 position{};
  int fireSound = -1;
};
struct EnemyCallbackContext {
  std::array<int32_t, 4> integers{};
  std::array<float, 4> floats{};
  CallbackShooter shooter0{}, shooter4{};
};
// Native selector4 reads the immutable extension payload, not a movement timer:
// +628/+62c/+630 = records[9].a/b/c, +640/+644 = records[10].a/b.
// Preserve original BulletManager slot order when supplying this view.
struct BulletLaserSource {
  Vec3 position{};
  bool vmAlive = false;
  eb::Phase phase = eb::Phase::Inactive;
  int marker = 0;
  float sourceAngle = 0, sourceSpeed = 0;
  Vec3 initialPosition{};
};
BulletLaserSource laserSource(const eb::State&, bool vmAlive = true);
struct EnemyCallbackResult {
  size_t visited = 0, matched = 0, emitted = 0, spawned = 0, cleared = 0, unsupported = 0;
};
// Native return is always zero. The counts are research/diagnostic metadata.
// Shooter0/4 are replaced even for an empty list; write them back to EnemyState.
// World emit/ANM/clear hooks run synchronously in original traversal order.
EnemyCallbackResult runEnemyCallback(int selector, EnemyCallbackContext&, Manager&, World&,
                                     std::span<const BulletLaserSource> bullets = {});
} // namespace th12::laser
