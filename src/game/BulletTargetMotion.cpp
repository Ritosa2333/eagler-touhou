#include "BulletTargetMotion.hpp"
#include "../bullet.hpp"
#include "AnmTypes.hpp"
#include <cmath>

namespace th12::eb {
namespace {
float lerp(float start, float end, float amount) {
  const float difference = float(double(end) - start);
  const float product = float(double(difference) * amount);
  return float(double(start) + product);
}
void advance(float& value, int& current, float rate) {
  value = float(double(value) + rate);
  current = int(value);
}
} // namespace
bool beginTargetMotion(TargetMotion& motion, State& bullet, const Record& record, World& world) {
  const int mode = record.d & 255;
  if (mode == 7 || mode == 8 || mode == 17 || mode > 17 || !std::isfinite(record.a) ||
      !std::isfinite(record.b)) {
    world.unsupported(0x2000000);
    return false;
  }
  motion = {};
  motion.fromX = bullet.x;
  motion.fromY = bullet.y;
  motion.targetX = record.a;
  motion.targetY = record.b;
  if (record.d & 0x100) {
    motion.targetX = float(double(motion.targetX) + bullet.x);
    motion.targetY = float(double(motion.targetY) + bullet.y);
  }
  motion.savedSpeed = bullet.speed;
  motion.duration = record.c;
  motion.mode = mode;
  bullet.active |= 0x2000000;
  return true;
}
bool tickTargetMotion(TargetMotion& motion, State& bullet, float rate) {
  if (motion.elapsed >= motion.duration) {
    bullet.active &= ~0x2000000u;
    bullet.x = motion.targetX;
    bullet.y = motion.targetY;
    bullet.speed = motion.savedSpeed;
    velocity(bullet, bullet.speed);
    return true;
  }
  // Native405900 advances its private interpolation timer first. The movement
  // elapsed timer advances separately at the end of40c230; there are two clocks.
  advance(motion.interpolationValue, motion.interpolation, rate);
  float x = motion.targetX, y = motion.targetY;
  if (motion.interpolation < motion.duration) {
    const float fraction = float(double(motion.interpolationValue) / motion.duration);
    const float amount = anm_logic::curve(fraction, motion.mode);
    x = lerp(motion.fromX, motion.targetX, amount);
    y = lerp(motion.fromY, motion.targetY, amount);
  } else {
    motion.interpolation = motion.duration;
    motion.interpolationValue = float(motion.duration);
  }
  bullet.vx = float(double(x) - bullet.x);
  bullet.vy = float(double(y) - bullet.y);
  if (std::abs(bullet.vx) > .0001f || std::abs(bullet.vy) > .0001f)
    bullet.angle = float(std::atan2(double(bullet.vy), double(bullet.vx)));
  advance(motion.elapsedValue, motion.elapsed, rate);
  return false;
}
} // namespace th12::eb
