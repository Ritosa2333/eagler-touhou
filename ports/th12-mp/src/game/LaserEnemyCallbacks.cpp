#include "LaserEnemyCallbacks.hpp"
#include <cmath>

namespace th12::laser {
namespace {
constexpr double sideRotation = 0.5890486240386963; // 4a42a0/4a4298.
constexpr float stepSize = 14.f;                    // 4a4110/4a4288.
constexpr double pi = 3.1415927410125732;           // 4a3cd8 (double containing float pi).
float sum(float a, float b) {
  return float(double(a) + b);
}
Vec3 add(Vec3 a, Vec3 b) {
  return {sum(a.x, b.x), sum(a.y, b.y), sum(a.z, b.z)};
}
Vec3 polar(float angle, float radius) {
  return {float(std::cos(double(angle)) * radius), float(std::sin(double(angle)) * radius), 0};
}
float normalize(float a) {
  return eb::normalize(a);
}
CallbackShooter resetShooter(int type) {
  CallbackShooter s;
  s.emission.type = type;
  s.emission.color = 0;
  // Selector3 writes the padding word+1fe instead of mode+1fc. It remains0.
  s.emission.mode = type == 23 ? 0 : 1;
  s.emission.program.flags = 0x83;
  s.emission.program.transformSound = 40;
  s.fireSound = 22;
  return s;
}
void emit(CallbackShooter& s, World& world, EnemyCallbackResult& result) {
  s.emission.x = s.position.x;
  s.emission.y = s.position.y;
  world.emitBullet(s.emission);
  // 40b5e0 plays Shooter+204 after allocating the count/layer sequence.
  if (s.emission.program.flags & 0x80)
    world.sound(s.fireSound, s.position.x);
  ++result.emitted;
}
Vec3 rotatedTarget(const State& s, bool left) {
  const float dx = float(double(s.position.x) - s.initialPosition.x);
  const float dy = float(double(s.position.y) - s.initialPosition.y);
  // Native stores each sine/cosine to float before the matrix multiplication.
  const float cosine = float(std::cos(left ? sideRotation : -sideRotation));
  const float sine = float(std::sin(left ? sideRotation : -sideRotation));
  const float x = float(double(cosine) * dx - double(sine) * dy);
  const float y = float(double(sine) * dx + double(cosine) * dy);
  return {sum(s.initialPosition.x, x), sum(s.initialPosition.y, y), 0};
}
void clear(Laser& l, Manager& manager, World& world, EnemyCallbackResult& result) {
  l.clearAll(manager, world, false, false);
  ++result.cleared;
}
void unsupported(World& world, EnemyCallbackResult& result, uint32_t code) {
  ++result.unsupported;
  world.unsupported(code);
}
} // namespace

BulletLaserSource laserSource(const eb::State& bullet, bool vmAlive) {
  return {{bullet.x, bullet.y, 0},
          vmAlive,
          bullet.phase,
          bullet.program.records[9].c,
          bullet.program.records[9].a,
          bullet.program.records[9].b,
          {bullet.program.records[10].a, bullet.program.records[10].b, 0}};
}
EnemyCallbackResult runEnemyCallback(int selector, EnemyCallbackContext& context, Manager& manager,
                                     World& world, std::span<const BulletLaserSource> bullets) {
  EnemyCallbackResult result;
  if (selector == 4) {
    for (const auto& source : bullets) {
      ++result.visited;
      if (!source.vmAlive || source.phase != eb::Phase::Alive || source.marker != 1)
        continue;
      ++result.matched;
      // Original capacity overflow falls through to a null-pointer write at
      // 41bce8. Keep the failed request observable without corrupting memory.
      if (manager.size() >= 256) {
        unsupported(world, result, 0xcb040100);
        continue;
      }
      Parameters p;
      p.position = source.position;
      p.angle = source.sourceAngle;
      p.speed = source.sourceSpeed;
      p.initialLength = p.maxLength = 128.f; // shared x87 value from 4a3db8.
      p.width = 14;
      p.type = 7;
      p.color = 6;
      p.sound = 19;
      p.flags = 1;
      p.program.records[0] = {0, 0, 1200, 0, 0x200, false};
      const int id = manager.spawn(Kind::Moving, p, world);
      if (auto* object = manager.find(id)) {
        object->state.initialPosition.x = source.initialPosition.x;
        object->state.initialPosition.y = source.initialPosition.y;
        ++result.spawned;
      } else
        unsupported(world, result, 0xcb040000);
    }
    return result;
  }
  if (selector != 3 && selector != 5) {
    unsupported(world, result, 0xcb000000u | uint32_t(selector));
    return result;
  }
  auto& shooter = selector == 3 ? context.shooter0 : context.shooter4;
  shooter = resetShooter(selector == 3 ? 23 : 7);
  // No kind/phase/cancellable filter in either original list traversal. The
  // clear hooks do not append lasers; emission only creates enemy bullets.
  for (auto* object : manager.ordered()) {
    ++result.visited;
    const auto& state = object->state;
    if (selector == 5 && state.extensionAge.current < context.integers[0])
      continue;
    ++result.matched;
    if (!std::isfinite(state.length) || state.length > 65536.f) {
      unsupported(world, result, 0xcb030001);
      continue;
    }
    Vec3 point = state.position;
    if (selector == 3) {
      const float angleA =
          normalize(float(double(state.angle) + 0.39269909262657166 + 0.19634954631328583));
      const float angleB =
          normalize(float(double(state.angle) - 0.39269909262657166 - 0.19634954631328583));
      Vec3 targetA = rotatedTarget(state, true), targetB = rotatedTarget(state, false);
      const auto mainStep = polar(state.angle, stepSize), stepA = polar(angleA, stepSize),
                 stepB = polar(angleB, stepSize);
      auto& program = shooter.emission.program;
      program.records[0] = {0, 0, 1200, 0, 0x200, false};
      program.records[1] = {0, 0, 180, 0, 0x400, true};
      program.records[3].kind = 0x2000;
      program.records[9].a = angleA;
      program.records[9].b = state.speed;
      program.records[9].c = 1;
      program.records[10].a = state.initialPosition.x;
      program.records[10].b = state.initialPosition.y;
      int i = 0;
      for (float distance = 0; distance < state.length; distance = sum(distance, stepSize)) {
        const auto target = i % 2 ? targetB : targetA;
        program.records[2] = {target.x, target.y, 180, 9, 0x2000000, true};
        shooter.position = point;
        point = add(point, mainStep);
        targetA = add(targetA, stepA);
        targetB = add(targetB, stepB);
        emit(shooter, world, result);
        program.records[9].c = 0;
        ++i;
      }
    } else {
      const float spacing = context.floats[0];
      if (state.length > 0 && (!std::isfinite(spacing) || spacing <= 0)) {
        unsupported(world, result, 0xcb050000);
        continue;
      }
      const auto step = polar(state.angle, spacing);
      for (float distance = 0; distance < state.length; distance = sum(distance, spacing)) {
        shooter.position = point;
        // 41be51/62/70/74 round the centered random value and pi product
        // separately; division and addition are one x87 expression.
        const float centered = float(double(world.next32()) * 0x1p-31 - 1.);
        const float spread = float(double(centered) * pi);
        shooter.emission.angle = float(double(spread) / 56. + state.angle);
        const float randomSpeed = float(double(world.next32()) * 0x1p-32);
        shooter.emission.speed = sum(context.floats[1], randomSpeed);
        point = add(point, step);
        emit(shooter, world, result);
      }
    }
    clear(*object, manager, world, result);
  }
  return result;
}
} // namespace th12::laser
