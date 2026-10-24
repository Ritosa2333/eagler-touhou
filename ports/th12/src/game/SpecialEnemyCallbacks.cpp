#include "SpecialEnemyCallbacks.hpp"
#include <bit>
#include <cmath>

namespace th12::special_enemy {
namespace {

bool enabledAlive(const BulletView& bullet) {
  return (bullet.objectFlags & 1u) && bullet.phase == 1;
}

float distance(const Position& a, const Position& b) {
  // Native keeps the coordinate subtraction/products in x87 registers, then
  // stores the sum and the square root separately as float32.
  const double dx = double(a.x) - b.x;
  const double dy = double(a.y) - b.y;
  const float squared = float(dx * dx + dy * dy);
  return float(std::sqrt(double(squared)));
}

VelocityHeading attract(const EnemyView& enemy, const BulletView& bullet, float length) {
  const float dx = float(double(enemy.position.x) - bullet.position.x);
  const float dy = float(double(enemy.position.y) - bullet.position.y);
  // D3DX9_40's SSE2 implementation tests a float32 squared norm against
  // 2^-46. It returns the zero vector below that threshold, including the
  // coincident-position case. Its rsqrt/Newton result is CPU dependent by a
  // few float32 ULPs; a portable sqrt retains the verified behavior/threshold.
  const float squared = float(float(dx * dx) + float(dy * dy));
  float nx = 0, ny = 0;
  if (!(squared < 0x1p-46f)) {
    const float reciprocalLength = float(1.0 / std::sqrt(double(squared)));
    nx = float(dx * reciprocalLength);
    ny = float(dy * reciprocalLength);
  }
  const float strength = float((128.0 - length) / 4800.0);
  const float vx = float(double(nx) * strength + bullet.vx);
  const float vy = float(double(ny) * strength + bullet.vy);
  return {vx, vy, float(std::atan2(double(vy), double(vx)))};
}

Result attractBullets(const EnemyView& enemy, const World& world) {
  if (!world.bulletSlotCount || !world.readBullet)
    return {0, Fault::MissingBulletReader};
  if (!world.writeBulletVelocity)
    return {0, Fault::MissingBulletWriter};
  Result result;
  const size_t count = world.bulletSlotCount();
  for (size_t slot = 0; slot < count; slot++) {
    BulletView bullet;
    if (!world.readBullet(slot, bullet) || !enabledAlive(bullet))
      continue;
    const float length = distance(bullet.position, enemy.position);
    if (!(length < 128.0f))
      continue;
    world.writeBulletVelocity(slot, attract(enemy, bullet, length));
    result.affectedBullets++;
  }
  return result;
}

bool validRewardIndex(int32_t index) {
  return index >= 0 && index < int32_t(extraScoreTable.size());
}

Result extraUfoScore(EnemyView& enemy, const World& world) {
  if (enemy.privateIntegers[0] == 1) {
    int32_t* counter = world.primaryBossRewardCounter ? world.primaryBossRewardCounter() : nullptr;
    if (!counter)
      return {0, Fault::MissingPrimaryBoss};
    // Valid scripts keep this counter in 0..14. Native code performs an
    // unchecked table lookup outside that range; do not synthesize a reward.
    if (!validRewardIndex(*counter))
      return {0, Fault::InvalidRewardIndex};
    if (!world.scoreStored)
      return {0, Fault::MissingScoreStorage};
    enemy.privateIntegers[1] = *counter;
    *counter += 1;
    if (*counter >= 15)
      *counter = 0;
    const int32_t rewardStored = extraScoreTable[enemy.privateIntegers[1]] / 10;
    // Explicit unsigned addition preserves native 32-bit wrap without signed
    // C++ overflow. Ordinary score is already in [0,999999999].
    const int32_t updated =
        std::bit_cast<int32_t>(uint32_t(*world.scoreStored) + uint32_t(rewardStored));
    *world.scoreStored = updated >= 1000000000 ? 999999999 : updated;
  }
  if (!validRewardIndex(enemy.privateIntegers[1]))
    return {0, Fault::InvalidRewardIndex};
  if (!world.showScorePopup)
    return {0, Fault::MissingPopupWriter};
  ScorePopup popup;
  popup.screenPosition = {float(double(enemy.position.x) + 224.0),
                          float(double(enemy.position.y) + 16.0), enemy.position.z};
  popup.displayedValue = extraScoreTable[enemy.privateIntegers[1]];
  world.showScorePopup(popup);
  return {};
}

Result linkedBulletLine(const EnemyView& enemy, const World& world) {
  if (!world.bulletSlotCount || !world.readBullet)
    return {0, Fault::MissingBulletReader};
  const size_t count = world.bulletSlotCount();
  for (size_t slot = 0; slot < count; slot++) {
    BulletView bullet;
    if (!world.readBullet(slot, bullet) || !enabledAlive(bullet) ||
        bullet.tag != enemy.privateIntegers[2])
      continue;
    // Only the first enabled/alive matching native pool slot is used.
    const float width = distance(enemy.position, bullet.position);
    if (!world.writePrimaryAnimationWidth || !world.writePrimaryAnimationWidth(enemy.id, width))
      return {0, Fault::MissingPrimaryAnimation};
    return {};
  }
  return {enemy.age >= 20 ? 1 : 0};
}

} // namespace

bool supports(int32_t selector) {
  return selector == int32_t(Callback::AttractBullets) ||
         selector == int32_t(Callback::ExtraUfoScore) ||
         selector == int32_t(Callback::LinkedBulletLine);
}

Result run(int32_t selector, EnemyView& enemy, const World& world) {
  switch (Callback(selector)) {
  case Callback::AttractBullets:
    return attractBullets(enemy, world);
  case Callback::ExtraUfoScore:
    return extraUfoScore(enemy, world);
  case Callback::LinkedBulletLine:
    return linkedBulletLine(enemy, world);
  default:
    return {0, Fault::UnsupportedSelector};
  }
}

} // namespace th12::special_enemy
