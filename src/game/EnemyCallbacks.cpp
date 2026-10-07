#include "EnemyCallbacks.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "LaserCollision.hpp"
#include "LaserEnemyCallbacks.hpp"
#include "LaserWorld.hpp"
#include "SpecialEnemyGeometry.hpp"

namespace th12 {
std::vector<special_enemy::ScorePopup> scorePopups;
namespace {
laser::CallbackShooter readShooter(const Enemy& e, const Shooter& s) {
  laser::CallbackShooter result;
  result.position = {float(double(s.fixedOrigin ? s.originX : e.x) + s.x),
                     float(double(s.fixedOrigin ? s.originY : e.y) + s.y), 0};
  result.emission = {
      result.position.x, result.position.y, s.angle,  s.spacing, s.speed,     s.slow, s.type,
      s.color,           s.count,           s.layers, s.mode,    s.extensions};
  result.fireSound = s.fireSound;
  return result;
}
void writeShooter(Shooter& s, const laser::CallbackShooter& result) {
  s = Shooter{};
  s.fixedOrigin = true;
  s.originX = result.position.x;
  s.originY = result.position.y;
  const auto& a = result.emission;
  s.angle = a.angle;
  s.spacing = a.spacing;
  s.speed = a.speed;
  s.slow = a.slow;
  s.type = a.type;
  s.color = a.color;
  s.count = a.count;
  s.layers = a.layers;
  s.mode = a.mode;
  s.extensions = a.program;
  s.fireSound = result.fireSound;
}
special_geometry::EnemyView geometryEnemy(const Enemy& e) {
  return {e.id,
          {e.x, e.y, 0},
          e.life,
          e.maxLife,
          e.flags,
          e.age,
          asfloat(e.variables[4]),
          asfloat(e.variables[5])};
}
special_geometry::AnimationView geometryAnimation(const Enemy& e) {
  if (auto* node = animationScene.find(enemyAnimationKey(e.id, 0))) {
    const auto& a = node->vm.state();
    return {true, a.rx, a.rotation, a.sy, a.spriteWidth, a.spriteHeight, a.flags};
  }
  return {};
}
laser::PlayerCollision collisionPlayer() {
  return {{px / 128.f, py / 128.f, 0},
          eb::playerHitboxes[character] * .5f,
          eb::playerHitboxes[character] * .5f,
          playerState,
          invuln,
          playerFlags,
          dialogue > 0,
          loadouts[character * 2 + shot].hit};
}
} // namespace
int runEnemyCallback(Enemy& e, int selector) {
  if (selector == 0)
    return 0;
  if (selector >= 3 && selector <= 5) {
    laser::EnemyCallbackContext context;
    for (int i = 0; i < 4; ++i) {
      context.integers[i] = i32(e.variables[i]);
      context.floats[i] = asfloat(e.variables[4 + i]);
    }
    context.shooter0 = readShooter(e, e.shooters[0]);
    context.shooter4 = readShooter(e, e.shooters[4]);
    std::vector<laser::BulletLaserSource> sources;
    if (selector == 4)
      for (const auto* b : hostileSlots)
        if (b && b->active)
          sources.push_back(laser::laserSource(b->enemy));
    const auto result =
        laser::runEnemyCallback(selector, context, laserManager, laserWorld, sources);
    if (selector == 3)
      writeShooter(e.shooters[0], context.shooter0);
    if (selector == 5)
      writeShooter(e.shooters[4], context.shooter4);
    if (result.unsupported)
      ++unsupported[529];
    return 0;
  }
  special_enemy::EnemyView view{e.id, {e.x, e.y, 0}, e.age};
  for (int i = 0; i < 4; ++i)
    view.privateIntegers[i] = i32(e.variables[i]);
  special_enemy::World world;
  world.bulletSlotCount = [] { return hostileSlots.size(); };
  world.readBullet = [](size_t slot, special_enemy::BulletView& out) {
    if (slot >= hostileSlots.size() || !hostileSlots[slot])
      return false;
    const auto& b = *hostileSlots[slot];
    if (!b.active)
      return false;
    out = {1,          uint16_t(b.enemy.phase), b.enemy.tag,  {b.enemy.x, b.enemy.y, 0}, b.enemy.vx,
           b.enemy.vy, b.enemy.angle,           b.enemy.speed};
    return true;
  };
  world.writeBulletVelocity = [](size_t slot, const special_enemy::VelocityHeading& value) {
    if (slot >= hostileSlots.size() || !hostileSlots[slot])
      return;
    auto& b = *hostileSlots[slot];
    b.enemy.vx = value.vx;
    b.enemy.vy = value.vy;
    b.enemy.angle = b.angle = value.heading;
  };
  world.primaryBossRewardCounter = []() -> int32_t* {
    auto* boss = primaryBoss();
    return boss ? reinterpret_cast<int32_t*>(&boss->variables[3]) : nullptr;
  };
  world.scoreStored = &score;
  world.showScorePopup = [](const auto& popup) {
    if (scorePopups.size() < 320)
      scorePopups.push_back(popup);
  };
  world.writePrimaryAnimationWidth = [](u32 id, float width) {
    if (auto* node = animationScene.find(enemyAnimationKey(id, 0))) {
      node->vm.control().spriteWidth = width;
      node->vm.control().flags |= 8u;
      return true;
    }
    return false;
  };
  const auto result = special_enemy::run(selector, view, world);
  // Counter3 may alias the current enemy. Only callback2's actual I1 write
  // is copied from the view; writing the whole private array loses that alias.
  if (selector == 2)
    e.variables[1] = u32(view.privateIntegers[1]);
  if (result.fault != special_enemy::Fault::None)
    ++unsupported[529];
  return result.nativeReturn;
}
int queryPlayerDamage(const Enemy& target, float x, float y, float width, float height) {
  Enemy area;
  area.id = target.id;
  area.x = x;
  area.y = y;
  area.hitWidth = width;
  area.hitHeight = height;
  int damage = playerSourceDamage(area) + playerBombDamage(area);
  for (size_t i = 0; i < bullets.size(); ++i) {
    auto& b = bullets[i];
    if (b.active && b.friendly)
      damage += friendlyProjectileDamage(b, area);
  }
  damage = std::min(80, damage);
  if (damage > 0 && score < 999999999)
    ++score;
  return damage;
}
int customEnemyDamage(Enemy& e) {
  if (e.customDamage == 0)
    return queryPlayerDamage(e, e.x, e.y, e.hitWidth, e.hitHeight);
  if (e.customDamage != 1) {
    ++unsupported[530];
    return 0;
  }
  auto animation = geometryAnimation(e);
  special_geometry::World world;
  world.damageQuery = [&](u32, const special_geometry::DamageArea& area) {
    return queryPlayerDamage(e, area.center.x, area.center.y, area.width, area.height);
  };
  const auto result = special_geometry::hurtSelector1(geometryEnemy(e), animation, world);
  if (animation.present)
    if (auto* node = animationScene.find(enemyAnimationKey(e.id, 0))) {
      node->vm.control().rx = animation.rotationX;
      node->vm.control().flags = animation.flags;
    }
  if (result.fault != special_geometry::Fault::None)
    ++unsupported[530];
  return result.damage;
}
void customEnemyCollision(Enemy& e) {
  if (e.customCollision == 0) {
    const auto result =
        laser::collidePlayerCircle({e.x, e.y, 0}, e.bodyWidth * .5f, collisionPlayer());
    if (result.beginMiss)
      playerMissCollision();
    if (result.code == laser::Collision::Graze && (e.flags & 0x200u) && e.age % 3 == 0) {
      awardPlayerGraze(px / 128.f, py / 128.f);
    }
    return;
  }
  if (e.customCollision != 1) {
    ++unsupported[531];
    return;
  }
  special_geometry::World world;
  world.circleCollision = [](const special_geometry::Circle& c) {
    if (oracleFixtureFlags & 2)
      return special_geometry::Collision::None;
    const auto result = laser::collidePlayerCircle({c.center.x, c.center.y, c.center.z}, c.radius,
                                                   collisionPlayer());
    if (result.beginMiss)
      playerMissCollision();
    return special_geometry::Collision(result.code);
  };
  world.lineCollision = [](const special_geometry::Line& line) {
    const auto result =
        laserWorld.lineCollision({line.origin.x, line.origin.y, line.origin.z}, line.angle,
                                 line.transverseSize, line.longitudinalSize);
    return special_geometry::Collision(result);
  };
  world.playerPosition = [] { return special_geometry::Position{px / 128.f, py / 128.f, 0}; };
  world.graze = [](const special_geometry::Position& position) {
    awardPlayerGraze(position.x, position.y, position.z);
  };
  const auto result =
      special_geometry::collisionSelector1(geometryEnemy(e), geometryAnimation(e), world);
  if (result.fault != special_geometry::Fault::None)
    ++unsupported[531];
}
} // namespace th12
