// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "EnemyCallbacks.hpp"
#include "GameState.hpp"

namespace th12 {
void finishEnemyUpdate(Enemy& e);
bool resolveEnemyInterrupt(Enemy& e) {
  e.phaseLife = e.life;
  e.lifeThreshold = 0;
  EnemyInterrupt* transition = nullptr;
  bool timeout = false;
  for (auto& interrupt : e.interrupts)
    if (interrupt.health >= 0) {
      e.phaseLife = e.life - interrupt.health;
      e.lifeThreshold = interrupt.health;
      if (e.life <= interrupt.health)
        transition = &interrupt;
      break;
    }
  if (!transition)
    for (auto& interrupt : e.interrupts)
      if (interrupt.health >= 0 && interrupt.time > 0) {
        if (e.phaseAge >= interrupt.time) {
          transition = &interrupt;
          timeout = true;
        }
        break;
      }
  if (!transition)
    return false;
  const std::string next = timeout ? transition->timeoutScript : transition->healthScript;
  e.life = transition->health;
  transition->health = -1;
  e.timedOut = timeout;
  if (timeout)
    e.flags |= 0x800000u;
  else
    e.flags &= ~0x800000u;
  if (timeout)
    spellState.onTimeout(playerBomb.active);
  resetPhase(e, next);
  return true;
}
void finishEnemyUpdate(Enemy& e) {
  if (!e.active) {
    if (e.isUfo)
      ufoManager.notifyEnemyRemoved(e.id, ufo_system::RemovalReason::ScriptDelete);
    return;
  }
  // 413c76 calls the continuous callback after ECL, before the hurt query.
  if (e.customUpdate && runEnemyCallback(e, e.customUpdate)) {
    e.active = false;
    if (e.isUfo)
      ufoManager.notifyEnemyRemoved(e.id, ufo_system::RemovalReason::NaturalExit);
    return;
  }
  // 413d8b calls 439ed0 before the HP immunity filters. Impact callbacks,
  // source accumulation and the stored-score hit award still run in immunity.
  if (!(oracleFixtureFlags & 1) && !e.hidden && !(e.flags & 0x21u)) {
    int damage = customEnemyDamage(e);
    if (dialogue)
      damage = 0; // Native413db9 still runs the query and impact callbacks first.
    if (playerState == 0 || playerState == 2)
      damage /= 5;
    if (spellState.active() && spellState.cardId >= 93 && spellState.cardId <= 99 &&
        (playerBomb.active || invuln || (playerFlags & 2u)) && !(playerFlags & 4u))
      damage /= 5;
    const bool spellShield = spellState.active() && spellState.cardId >= 103 &&
                             spellState.cardId <= 112 && playerBomb.active &&
                             (e.flags & 0x400000u) && !(playerFlags & 4u);
    if (spellState.pendingBomb())
      damage /= 30;
    if (!spellShield && damage > 0) {
      if (!(e.flags & 0x10u) && !e.immunityTicks) {
        enemy_life::hurt(e.life, e.lifeRaw, e.lifeThreshold, e.lifeFlags, damage);
        eventBits |= 8;
      }
      // The first interrupt/death check belongs to the effective hurt path.
      // HP0 alone does not kill an enemy on a frame with no hurt query.
      resolveEnemyInterrupt(e);
      if (!e.active)
        return;
      // 413f19 checks HP after the synchronous health interrupt too. A zero
      // threshold death routine does not postpone removal by a fresh tick.
      if (e.life <= 0 && !(e.flags & 0x80u)) {
        drop(e);
        e.active = false;
        return;
      }
      if (!e.active)
        return;
      e.flags |= 0x100000u;
    }
  }
  if (e.lifeFlags & 2u) {
    drop(e);
    e.active = false;
    return;
  }
  resolveEnemyInterrupt(e);
  if (!e.active)
    return;
  if (!(oracleFixtureFlags & 2) && e.active && !(e.flags & (2 | 32 | 0x2000000)) &&
      !e.bodyImmunityTicks) {
    customEnemyCollision(e);
  }
  // 414a29..414a65: timers and age advance after interrupts and body collision.
  if (e.immunityTicks > 0)
    --e.immunityTicks;
  if (e.bodyImmunityTicks > 0)
    --e.bodyImmunityTicks;
  e.invincible = bool(e.flags & 17) || e.immunityTicks > 0;
  ++e.age;
  ++e.phaseAge;
  ++e.animationAge;
}
bool enemyOutside(Enemy& e) {
  if (auto* node = animationScene.find(enemyAnimationKey(e.id, 0))) {
    const auto& pose = node->vm.state();
    if (auto bank =
            animationScene.registry.bank(pose.spriteBank >= 0 ? pose.spriteBank : pose.bank);
        bank && pose.sprite >= 0 && pose.sprite < int(bank->sprites.size())) {
      const auto& sprite = bank->sprites[pose.sprite];
      // 413d3e crosses the axes: X extent uses sprite height*scaleY, Y uses
      // sprite width*scaleX. A missing slot retains the previous extents.
      e.renderWidth = std::abs(float(double(sprite.height) * pose.sy));
      e.renderHeight = std::abs(float(double(sprite.width) * pose.sx));
    }
  }
  const bool outsideX = float(double(e.x) + e.renderWidth * .5f) < -192 ||
                        float(double(e.x) - e.renderWidth * .5f) > 192;
  if (outsideX)
    return (e.flags & 0x8000u) && !(e.flags & 4);
  const bool outsideY = float(double(e.y) + e.renderHeight * .5f) < 0 ||
                        float(double(e.y) - e.renderHeight * .5f) > 448;
  if (outsideY)
    return (e.flags & 0x8000u) && !(e.flags & 8);
  e.flags |= 0x8000u;
  return false;
}
Enemy* spawn(const std::string& sub, float x, float y, int life, int points, int drop, bool mirror,
             const Enemy* parent, u32 bornFlags, float z) {
  const auto* pc = program.find(sub);
  if (!pc || enemies.size() > 1024) {
    unsupported[999]++;
    return nullptr;
  }
  auto p = std::make_unique<Enemy>();
  // Binding needs a unique owner while native+27b4 is not assigned yet.
  // This counter never consumes the gameplay ID sequence.
  static u32 temporaryEnemyOwner = 0x7fffffffu;
  p->id = temporaryEnemyOwner--;
  p->x = x;
  p->y = y;
  p->z = z;
  p->ax = enemy_motion::quantize(x);
  p->ay = enemy_motion::quantize(y);
  combinePosition(*p);
  p->life = p->maxLife = life;
  p->lifeRaw = enemy_life::expanded(life);
  p->phaseLife = life;
  p->score = points;
  p->drop = drop;
  p->mirror = mirror;
  // 412a8e..412ab5 assigns these before the synchronous full birth update.
  // In particular background bodies must not collide during their constructor.
  p->flags |= bornFlags;
  if (life >= 1000)
    p->flags |= 0x20000000u;
  if (parent)
    p->variables = parent->variables;
  p->boss = sub.find("Boss") != std::string::npos && sub.find("At") == std::string::npos;
  p->controller = sub == "main" || sub.find("Logo") != std::string::npos ||
                  sub.find("Shadow") != std::string::npos ||
                  sub.find("Maple") != std::string::npos ||
                  sub.find("EtBreak") != std::string::npos;
  p->hidden = p->controller;
  p->contexts.emplace_back(pc);
  Enemy* result = p.get();
  enemyOutside(*result);
  runContext(*result, result->contexts.front());
  for (int slot = 0; slot < 16; ++slot)
    if (result->animationBound[slot])
      if (auto* node = animationScene.find(enemyAnimationKey(result->id, slot)))
        node->vm.control().engineZ = result->z;
  // 412990 executes a complete birth update, then sets 20000. The next
  // 413840 call only clears that guard. Birth motion has zero initial speed.
  finishEnemyUpdate(*result);
  result->flags |= 0x20000u;
  // 412acb assigns the ID after full birth/ECL recursion, before registration.
  // Keep every already-bound VM and child, including its current RNG/state.
  const u32 nativeId = nextEnemyId++;
  if (animationScene.reassignEnemyOwner(result->id, nativeId))
    result->id = nativeId;
  else
    ++unsupported[997]; // preserve the safe owner if an unexpected key collides.
  result->deathSound = 3 + int(nativeId & 1u);
  result->deathAnimation = 84;
  // Native412c60 copies BulletManager's resource into EnemyManager slot0.
  result->deathAnimationBank = 0;
  if (result->bank == 1)
    switch (result->baseAnimation) {
    case 5:
    case 55:
      result->deathAnimation = 81;
      break;
    case 10:
    case 56:
      result->deathAnimation = 87;
      break;
    case 15:
    case 57:
      result->deathAnimation = 90;
      break;
    }
  // Registration follows the synchronous constructor. Births made by its ECL
  // are already in the manager's chain when this node is appended.
  enemies.push_back(std::move(p));
  return result;
}
void resetPhase(Enemy& e, const std::string& sub) {
  e.contexts.clear();
  e.contexts.emplace_back(program.find(sub));
  e.phaseAge = e.age = 0;
  runContext(e, e.contexts.front());
}
void dropItems(Enemy& e) {
  const auto ordinary = [](float x, float y, int type, int source) {
    if (type >= 1 && type <= 18)
      itemManager.spawn(type, {x, y});
    else if (type != 0)
      unsupported[source]++;
  };
  ordinary(e.x, e.y, e.drop, 410);
  e.drop = 0;
  // Native 412140 consumes the initial angle even when all counts are zero.
  float angle = float(double(random.signedUnit()) * pi);
  for (int type = 1; type <= 18; type++)
    for (int i = 0; i < e.dropCounts[type - 1]; i++) {
      const float offsetX = float(std::cos(double(angle)) * e.dropSpreadX),
                  offsetY = float(std::sin(double(angle)) * e.dropSpreadY);
      const float radial = float(double(random.unit()) * .5 + .5);
      const float x = float(double(e.x) + float(double(offsetX) * radial)),
                  y = float(double(e.y) + float(double(offsetY) * radial));
      ordinary(x, y, type, 407);
      const float jitter = float(double(random.signedUnit()) * pi);
      angle = normalize(float(double(angle) + pi / 2 + double(jitter) * .25));
    }
  e.dropCounts.fill(0);
}
void drop(Enemy& e) {
  if (e.hidden)
    return;
  enemyDeathVisual(e);
  dropItems(e); // Native414af0 creates the visual, drops, then calls Enemy+103c.
  if (e.isUfo) {
    applyUfoPlan(ufoManager.finish(e.id, resources, e.deathReason));
    ufoManager.notifyEnemyRemoved(e.id, e.deathReason == ufo_system::DeathReason::Dialogue
                                            ? ufo_system::RemovalReason::Dialogue
                                            : ufo_system::RemovalReason::Defeated);
  }
  eventBits |= 2;
}
void enemyDeathVisual(const Enemy& e) {
  sound_system::queue.play(e.deathSound, e.x);
  if (e.deathAnimation < 0)
    return;
  const auto key = 0x100000000ull + nextDetachedAnimation++;
  animationScene.bind(key, 0, e.deathAnimationBank, e.deathAnimation,
                      anm_logic::Membership::Primary, e.x, e.y, {}, false, nullptr, 224, 16, 4,
                      false, e.z);
  // Native454d10 clears the factory's flags2 before the first script tick.
}
void requestEnemyClear() {
  // Native414c60 spawns only the clear visual, then marks deferred removal.
  // Protected/hidden bodies survive unless flag100 overrides that protection.
  for (auto& pointer : enemies) {
    auto& e = *pointer;
    if (!e.active || ((e.flags & 0x6004a0u) && !(e.flags & 0x100u)))
      continue;
    enemyDeathVisual(e);
    e.flags |= 0x1000000u;
  }
}
void tickEnemies() {
  for (size_t i = 0; i < enemies.size();) {
    auto& e = *enemies[i];
    // 413190 saves this node's successor before updating it. A birth appended
    // behind an existing successor is visited, but a birth at the old tail is
    // deferred until the next manager pass.
    i = i + 1 < enemies.size() ? i + 1 : size_t(-1);
    if (!e.active)
      continue;
    if (e.flags & 0x1000000u) {
      e.active = false;
      if (e.isUfo)
        ufoManager.notifyEnemyRemoved(e.id, ufo_system::RemovalReason::ScriptDelete);
      continue;
    }
    if (e.flags & 0x20000u) {
      e.flags &= ~0x20000u;
      continue;
    }
    e.invincible = bool(e.flags & 17) || e.immunityTicks > 0;
    if (e.polarTicks) {
      float t = easing(float(++e.polarTime) / e.polarTicks, e.polarMode);
      e.angle = normalize(e.fromA + (e.toA - e.fromA) * t);
      e.speed = e.fromS + (e.toS - e.fromS) * t;
      if (e.polarTime >= e.polarTicks)
        e.polarTicks = 0;
    }
    if (e.relativePolarTicks) {
      float t = easing(float(++e.relativePolarTime) / e.relativePolarTicks, e.relativePolarMode);
      e.relativeAngle = normalize(e.relativeFromA + (e.relativeToA - e.relativeFromA) * t);
      e.relativeSpeed = e.relativeFromS + (e.relativeToS - e.relativeFromS) * t;
      if (e.relativePolarTime >= e.relativePolarTicks)
        e.relativePolarTicks = 0;
    }
    for (int relative = 0; relative < 2; relative++) {
      auto& m = relative ? e.relativeCircle : e.absoluteCircle;
      if (m.ticks) {
        float t = easing(float(++m.time) / m.ticks, m.mode);
        (relative ? e.relativeSpeed : e.speed) = m.fromOmega + (m.toOmega - m.fromOmega) * t;
        m.radius = m.fromRadius + (m.toRadius - m.fromRadius) * t;
        m.radialSpeed = m.fromRadial + (m.toRadial - m.fromRadial) * t;
        if (m.time >= m.ticks)
          m.ticks = 0;
      }
    }
    if (e.positionTicks) {
      ++e.positionTime;
      if (e.positionMode == 8) {
        const auto value = e.positionHermite.tick(3, 1);
        e.ax = value[0];
        e.ay = value[1];
      } else {
        float t = easing(float(e.positionTime) / e.positionTicks, e.positionMode);
        e.ax = e.fromX + (e.toX - e.fromX) * t;
        e.ay = e.fromY + (e.toY - e.fromY) * t;
      }
      if (e.positionTime >= e.positionTicks)
        e.positionTicks = 0;
    } else if (e.absoluteCircle.enabled) {
      e.angle = normalize(float(double(e.angle) + e.speed));
      e.absoluteCircle.radius =
          float(double(e.absoluteCircle.radius) + e.absoluteCircle.radialSpeed);
      circularPosition(e, false);
    } else {
      e.ax += float(std::cos(double(e.angle)) * e.speed);
      e.ay += float(std::sin(double(e.angle)) * e.speed);
    }
    if (e.relativePositionTicks) {
      ++e.relativePositionTime;
      if (e.relativePositionMode == 8) {
        const auto value = e.relativePositionHermite.tick(3, 1);
        e.rx = value[0];
        e.ry = value[1];
      } else {
        float t =
            easing(float(e.relativePositionTime) / e.relativePositionTicks, e.relativePositionMode);
        e.rx = e.relativeFromX + (e.relativeToX - e.relativeFromX) * t;
        e.ry = e.relativeFromY + (e.relativeToY - e.relativeFromY) * t;
      }
      if (e.relativePositionTime >= e.relativePositionTicks)
        e.relativePositionTicks = 0;
    } else if (e.relativeCircle.enabled) {
      e.relativeAngle = normalize(float(double(e.relativeAngle) + e.relativeSpeed));
      e.relativeCircle.radius =
          float(double(e.relativeCircle.radius) + e.relativeCircle.radialSpeed);
      circularPosition(e, true);
    } else {
      e.rx += float(std::cos(double(e.relativeAngle)) * e.relativeSpeed);
      e.ry += float(std::sin(double(e.relativeAngle)) * e.relativeSpeed);
    }
    e.ax = enemy_motion::quantize(e.ax);
    e.ay = enemy_motion::quantize(e.ay);
    e.rx = enemy_motion::quantize(e.rx);
    e.ry = enemy_motion::quantize(e.ry);
    combinePosition(e);
    if (e.flags & 0x10000u) {
      e.x = std::clamp(e.x, e.clampX - e.clampWidth * .5f, e.clampX + e.clampWidth * .5f);
      e.y = std::clamp(e.y, e.clampY - e.clampHeight * .5f, e.clampY + e.clampHeight * .5f);
      e.ax = float(double(e.x) - e.rx);
      e.ay = float(double(e.y) - e.ry);
    }
    for (int slot = 0; slot < 16; slot++)
      if (e.animationBound[slot])
        animationScene.position(enemyAnimationKey(e.id, slot), e.x, e.y);
    if (enemyOutside(e)) {
      e.active = false;
      if (e.isUfo)
        ufoManager.notifyEnemyRemoved(e.id, ufo_system::RemovalReason::NaturalExit);
      continue;
    }
    // Snapshot existing contexts; asynchronous children start on a later tick.
    std::vector<Context*> existing;
    for (auto& c : e.contexts)
      existing.push_back(&c);
    for (auto* c : existing)
      if (c->pc)
        runContext(e, *c);
    finishEnemyUpdate(e);
  }
}

} // namespace th12
