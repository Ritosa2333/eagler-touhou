// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
void playerMissCollision() {
  if (invuln || playerState != 1)
    return;
  // 438370 loses capture eligibility on collision, before the deathbomb window.
  spellState.onMiss(playerBomb.active);
  playerState = 4;
  playerStateTicks = 0;
  deathWindow = 8;
  invuln = 6;
  eventBits |= 32;
}
// Included inside the title namespace, after its entity definitions.
// Named TH12 systems; isolated native callback evidence is retained separately.
pw::Manager playerDamageSources;
pw::BombState playerBomb;
// Synchronous original call sites. An empty hook leaves isolated fixtures alone.
std::function<void(Projectile&)> friendlyBirthHook, friendlyHitHook;
std::function<void(Projectile&, int, bool)> friendlyInterruptHook; // bool = auxiliary VM
std::function<void(const pw::BombVisual&)> playerBombBindHook;
std::function<void(const pw::ReimuBOrb&)> reimuBOrbBindHook;
std::function<void(int, u32, float, float)>
    playerBombCustomBindHook; // effect profile/id/native position
Enemy* playerTarget(float, float, float, u32);
void explodeReimuBOrb(pw::ReimuBOrb& orb) {
  if (!orb.active)
    return;
  playerBomb.circles.push_back({orb.x, orb.y, 128});
  // Retail 408030 allocates the burst before disabling the prior source slot.
  playerDamageSources.circle(orb.x, orb.y, 64, 8, 11, 60);
  if (orb.sourceSlot >= 0) {
    auto& s = playerDamageSources.sources[orb.sourceSlot];
    if (s.id == orb.sourceId)
      s.flags &= ~1u;
  }
  orb.active = false;
  orb.sourceSlot = -1;
  orb.tailAge = 0;
  eventBits |= 1;
}
bool startPlayerBomb() {
  if (playerBomb.active)
    return false;
  const int loadout = character * 2 + shot;
  if (loadout < 0 || loadout > 5) {
    ++unsupported[997];
    return false;
  }
  auto leaves = std::move(playerBomb.leaves);
  auto beams = std::move(playerBomb.reimuABeams);
  auto retired = std::move(playerBomb.retiredVisuals);
  for (const auto& v : playerBomb.primaryVisuals) {
    int remaining = 0;
    if (playerBomb.loadout == 3 && v.interrupt == 1)
      remaining = 60 - v.age;
    if (playerBomb.loadout == 2 && v.interrupt == 1)
      remaining = 40 - v.age;
    if (playerBomb.loadout == 1 && v.interrupt == 1)
      remaining = 20 - v.age;
    if (remaining > 0)
      retired.push_back({v, remaining});
  }
  playerBomb.reset();
  playerBomb.leaves = std::move(leaves);
  playerBomb.reimuABeams = std::move(beams);
  playerBomb.retiredVisuals = std::move(retired);
  playerBomb.loadout = loadout;
  playerBomb.active = true;
  playerBomb.x = px / 128.f;
  playerBomb.y = py / 128.f;
  if (loadout == 3) {
    playerBomb.marisaB.begin(nextProjectileId++, playerBomb.x, playerBomb.y);
    const auto& b = playerBomb.marisaB;
    if (playerBombBindHook)
      playerBombBindHook({b.id, 1, 24, 0, 0, b.x, b.y, 0, true});
  } else if (loadout == 5)
    playerBomb.visuals.push_back(
        {nextProjectileId++, 2, 21, 0, 0, playerBomb.x, playerBomb.y, 0, true});
  else if (loadout == 2) {
    invuln = 120;
    playerBomb.visuals.push_back(
        {nextProjectileId++, 1, 16, 0, 0, playerBomb.x, playerBomb.y, 0, true});
  } else if (loadout == 0) {
    invuln = 200;
    playerBomb.reimuAEmitter.id = nextProjectileId++;
    if (playerBombCustomBindHook)
      playerBombCustomBindHook(4, playerBomb.reimuAEmitter.id, playerBomb.x, playerBomb.y);
  } // Original effect profile4, bullet ANM0.
  else if (loadout == 4) {
    // pl02 script19 opcode48 owns absolute screen (224,240), so its descriptor's
    // screen base is zero rather than the Bomb's captured player position.
    playerBomb.visuals.push_back({nextProjectileId++, 2, 19, 0, 0, -224, -16, 0, true});
  }
  for (const auto& v : playerBomb.visuals)
    if (playerBombBindHook)
      playerBombBindHook(v);
  playerBomb.primaryVisuals = playerBomb.visuals;
  eventBits |= 16;
  return true;
}
void tickPlayerBomb() {
  playerBomb.visuals = playerBomb.primaryVisuals;
  struct FinishVisuals {
    ~FinishVisuals() {
      playerBomb.primaryVisuals = playerBomb.visuals;
      for (auto& tail : playerBomb.retiredVisuals) {
        ++tail.visual.age;
        --tail.remaining;
        if (tail.remaining > 0)
          playerBomb.visuals.push_back(tail.visual);
      }
      playerBomb.retiredVisuals.erase(
          std::remove_if(playerBomb.retiredVisuals.begin(), playerBomb.retiredVisuals.end(),
                         [](const pw::RetiredVisual& v) { return v.remaining <= 0; }),
          playerBomb.retiredVisuals.end());
    }
  } finishVisuals;
  playerBomb.rectangles.clear();
  playerBomb.circles.clear();
  if (playerBomb.loadout == 3) {
    auto& b = playerBomb.marisaB;
    playerBomb.visuals.clear();
    if (b.tick())
      invuln = 40;
    playerBomb.active = b.active;
    playerBomb.previousAge = playerBomb.age;
    playerBomb.age = b.age;
    if (b.clear.enabled)
      playerBomb.rectangles.push_back(b.clear);
    if (b.visible)
      playerBomb.visuals.push_back({b.id, 1, 24, b.visualAge, b.interrupt, b.x, b.y, 0, true});
  }
  if (playerBomb.loadout == 5 && playerBomb.active) {
    invuln = 40;
    const int age = playerBomb.age;
    // pl02 script21 deletes at ANM tick280. The following Bomb update observes
    // the missing VM, before the otherwise dormant native 400-tick hard cap.
    if (age >= 281) {
      playerBomb.active = false;
      playerBomb.visuals.clear();
      return;
    }
    if (age == 0)
      playerDamageSources.circle(playerBomb.x, playerBomb.y, 16, .35555556416511536f, 180, 13);
    if (age == 180)
      playerDamageSources.circle(playerBomb.x, playerBomb.y, 80, 20, 100, 80);
    if (age >= 30) {
      float x, y;
      if (age < 250) {
        const float radius = pw::unitRandom(random.next32());
        const float direction = float(double(pw::signedRandom(random.next32())) * double(pi));
        x = float(double(playerBomb.x) + float(std::cos(double(direction)) * radius));
        y = float(double(playerBomb.y) + float(std::sin(double(direction)) * radius));
      } else {
        x = float(double(pw::signedRandom(random.next32())) * 192);
        y = float(double(pw::unitRandom(random.next32())) * 448);
      }
      pw::Leaf leaf;
      leaf.begin(nextProjectileId++, x, y, random);
      playerBomb.leaves.push_back(leaf);
      if (playerBombCustomBindHook)
        playerBombCustomBindHook(5, leaf.id, x, y);
    }
    playerBomb.circles.push_back({playerBomb.x, playerBomb.y, pw::sanaeBClearRadius(age)});
    if (!playerBomb.visuals.empty())
      playerBomb.visuals[0].age = age;
    playerBomb.previousAge = age;
    ++playerBomb.age;
  }
  if (playerBomb.loadout == 2) {
    if (!playerBomb.active) {
      if (!playerBomb.visuals.empty()) {
        playerBomb.visuals[0].age = ++playerBomb.tailAge;
        if (playerBomb.tailAge >= 40)
          playerBomb.visuals.clear();
      }
      return;
    }
    invuln = 40;
    const int age = playerBomb.age;
    if (age > 300) {
      playerBomb.active = false;
      playerBomb.speedMultiplier = 1;
      playerBomb.tailAge = 0;
      if (!playerBomb.visuals.empty()) {
        playerBomb.visuals[0].interrupt = 1;
        playerBomb.visuals[0].age = 0;
      }
      return;
    }
    playerBomb.speedMultiplier = .20000000298023224f;
    playerBomb.x = px / 128.f;
    playerBomb.y = py / 128.f;
    auto areas = pw::marisaAAreas(playerBomb.x, playerBomb.y);
    playerBomb.rectangles.assign(areas.begin(), areas.end());
    if (!playerBomb.visuals.empty()) {
      auto& v = playerBomb.visuals[0];
      v.x = playerBomb.x;
      v.y = playerBomb.y;
      v.age = age;
    }
    playerBomb.previousAge = age;
    ++playerBomb.age;
  }
  if (playerBomb.loadout == 0 && playerBomb.active) {
    invuln = 40;
    if (!playerBomb.reimuAEmitter.active) {
      playerBomb.active = false;
      playerBomb.speedMultiplier = 1;
      return;
    }
    playerBomb.speedMultiplier = .5f;
    playerBomb.x = px / 128.f;
    playerBomb.y = py / 128.f;
    // 40ff10(4,17) builds 17 mesh columns. Retail 46a970 advances two game RNG32
    // values per column even though this backdrop contributes no damage.
    for (int i = 0; i < 17; ++i) {
      random.next32();
      random.next32();
    }
    playerBomb.rectangles.push_back({true, playerBomb.x, 224, 160, 448});
    playerBomb.previousAge = playerBomb.age;
    ++playerBomb.age;
  }
  if (playerBomb.loadout == 1) {
    const int age = playerBomb.age;
    if (playerBomb.active)
      invuln = 40;
    playerBomb.visuals.clear();
    if (playerBomb.active && age == 0)
      for (int i = 0; i < 8; ++i) {
        auto& orb = playerBomb.reimuBOrbs[i];
        orb.begin(nextProjectileId++, i, px / 128.f, py / 128.f, playerDamageSources);
        if (reimuBOrbBindHook)
          reimuBOrbBindHook(orb);
      }
    for (auto& orb : playerBomb.reimuBOrbs) {
      if (orb.active) {
        if (age >= 200)
          explodeReimuBOrb(orb);
        else {
          auto* target =
              orb.age > 90 + orb.index * 10 ? playerTarget(orb.x, orb.y, 512, 0) : nullptr;
          orb.tick(px / 128.f, py / 128.f, target != nullptr, target ? target->x : 0,
                   target ? target->y : 0);
          if (orb.sourceSlot >= 0) {
            auto& s = playerDamageSources.sources[orb.sourceSlot];
            if (s.id == orb.sourceId) {
              if (s.accumulated >= 300)
                explodeReimuBOrb(orb);
              else {
                s.x = orb.x;
                s.y = orb.y;
              }
            }
          }
          if (orb.active)
            playerBomb.circles.push_back({orb.x, orb.y, 64});
        }
      } else
        ++orb.tailAge;
      if (orb.active || orb.tailAge < 20)
        playerBomb.visuals.push_back({orb.id, 0, 24, orb.active ? orb.age : orb.tailAge,
                                      orb.active ? 0 : 1, orb.x, orb.y, 0, true});
    }
    if (playerBomb.active) {
      playerBomb.previousAge = age;
      if (age >= 200)
        playerBomb.active = false;
      else
        ++playerBomb.age;
    }
  }
  if (playerBomb.loadout == 4 && playerBomb.active) {
    invuln = 40;
    const int age = playerBomb.age;
    if (age >= 241) {
      playerBomb.active = false;
      playerBomb.visuals.clear();
      return;
    }
    if (age < 240)
      playerBomb.rectangles.push_back({true, 0, 224, 384, 448});
    if (!playerBomb.visuals.empty()) {
      auto& v = playerBomb.visuals[0];
      v.interrupt = age == 240 ? 1 : 0;
      v.age = age == 240 ? 0 : age;
    }
    playerBomb.previousAge = age;
    ++playerBomb.age;
  }
}
int playerBombDamage(const Enemy& e) {
  if (!playerBomb.active)
    return 0;
  if (playerBomb.loadout == 3)
    return playerBomb.marisaB.damage(e.y);
  if (playerBomb.loadout == 2)
    return pw::marisaADamage(playerBomb.age, playerBomb.previousAge, playerBomb.x, playerBomb.y,
                             e.x, e.y, e.hitWidth, e.hitHeight);
  if (playerBomb.loadout == 4)
    return pw::sanaeADamage(e.y);
  return 0;
}
// Called by a logical ANM node BEFORE its bytecode tick (native 460fe0).
// Return true to retire that node and skip its bytecode tick. New beams created
// by profile4 must bind bullet ANM210 synchronously in the caller's append hook.
bool tickPlayerBombEffect(int profile, u32 id) {
  if (profile == 3) {
    for (auto& beam : playerBomb.reimuABeams)
      if (beam.id == id) {
        beam.tick(playerDamageSources);
        return !beam.active;
      }
    return true;
  }
  if (profile == 4) {
    auto& emitter = playerBomb.reimuAEmitter;
    if (emitter.id != id || !emitter.active)
      return true;
    if (emitter.age >= 300) {
      emitter.active = false;
      return true;
    }
    if (emitter.age % 4 == 0) {
      auto offsets = emitter.fire();
      for (float offset : offsets) {
        pw::ReimuABeam beam;
        beam.begin(nextProjectileId++, float(double(playerBomb.x) + offset), playerBomb.x,
                   playerDamageSources);
        playerBomb.reimuABeams.push_back(beam);
        if (playerBombCustomBindHook)
          playerBombCustomBindHook(3, beam.id, beam.x, beam.y);
      }
    }
    ++emitter.age;
    return false;
  }
  if (profile == 5) {
    for (auto& leaf : playerBomb.leaves)
      if (leaf.id == id) {
        leaf.tick(random);
        return !leaf.active;
      }
    return true;
  }
  return true;
}
// Presentation-data refresh only; no timers, movement, binding or RNG.
void buildPlayerBombTrails() {
  playerBomb.trails.clear();
  for (auto& beam : playerBomb.reimuABeams) {
    if (!beam.active)
      continue;
    pw::BombTrail trail;
    trail.id = beam.id;
    trail.kind = 12;
    trail.width = 24;
    trail.points.assign(beam.history.begin(), beam.history.end());
    playerBomb.trails.push_back(std::move(trail));
  }
  playerBomb.reimuABeams.erase(std::remove_if(playerBomb.reimuABeams.begin(),
                                              playerBomb.reimuABeams.end(),
                                              [](const pw::ReimuABeam& b) { return !b.active; }),
                               playerBomb.reimuABeams.end());
  for (auto& leaf : playerBomb.leaves) {
    if (!leaf.active)
      continue;
    pw::BombTrail trail;
    trail.id = leaf.id;
    trail.points.assign(leaf.points.begin(), leaf.points.begin() + leaf.age);
    playerBomb.trails.push_back(std::move(trail));
  }
  playerBomb.leaves.erase(std::remove_if(playerBomb.leaves.begin(), playerBomb.leaves.end(),
                                         [](const pw::Leaf& l) { return !l.active; }),
                          playerBomb.leaves.end());
}
// Isolated fallback without a shared ANM scheduler. The full game instead calls
// tickPlayerBombEffect for each native registered node and then refreshes trails.
void tickPlayerBombEffects() {
  const size_t beams = playerBomb.reimuABeams.size();
  for (size_t i = 0; i < beams; ++i)
    tickPlayerBombEffect(3, playerBomb.reimuABeams[i].id);
  if (playerBomb.loadout == 0 && playerBomb.reimuAEmitter.active)
    tickPlayerBombEffect(4, playerBomb.reimuAEmitter.id);
  for (auto& leaf : playerBomb.leaves)
    tickPlayerBombEffect(5, leaf.id);
  buildPlayerBombTrails();
}
int friendlyImpactDuration(int script) {
  // Retail pl00/pl01/pl02 ANM delete instructions; this is the phase's logic
  // lifetime, independent of the renderer's ANM clock.
  if (character == 0) {
    switch (script) {
    case 6:
      return 30;
    case 8:
    case 10:
      return 20;
    case 12:
      return 12;
    }
  }
  if (character == 1) {
    switch (script) {
    case 6:
      return 30;
    case 10:
      return 20;
    }
  }
  if (character == 2) {
    switch (script) {
    case 6:
      return 13;
    case 9:
      return 20;
    case 12:
      return 8;
    case 13:
    case 14:
      return 16;
    }
  }
  return 0;
}
void initializeFriendlyProjectile(Projectile& b, const ShotSpec& s) {
  b.weapon = {};
  b.hitAnimation = s.hitAnimation + 5;
  b.hitCallback = s.hitCallback;
  b.spawnCallback = s.spawn;
  b.option = s.option;
  b.update = s.update;
  b.extraCallback = s.extraCallback;
  b.hitWidth = s.w;
  b.hitHeight = s.spawn == 2 ? 0 : s.h;
  if (friendlyBirthHook)
    friendlyBirthHook(b);
}
void tickPlayerDamageSources() {
  playerDamageSources.tick();
}
int playerSourceDamage(const Enemy& e) {
  return playerDamageSources.damage(e.x, e.y, e.hitWidth, e.hitHeight);
}
float playerAngle(float a) {
  while (a > pi)
    a -= tau;
  while (a < -pi)
    a += tau;
  return a;
}
Enemy* playerTarget(float x, float y, float radius, u32 existing) {
  Enemy* nearest = nullptr;
  float distance = radius * radius;
  for (auto& p : enemies) {
    auto& e = *p;
    if (!e.active || e.hidden || e.invincible)
      continue;
    if (existing && e.id == existing)
      return &e;
    if (existing)
      continue;
    const float dx = e.x - x, dy = e.y - y, d = dx * dx + dy * dy;
    if (d < distance) {
      distance = d;
      nearest = &e;
    }
  }
  return nearest;
}
void updateFriendlyProjectile(Projectile& b) {
  if (!b.friendly)
    return;
  const bool hitPrevious = b.weapon.hit;
  b.weapon.previousAge = b.age;
  b.weapon.hit = false;
  ++b.weapon.visualAge;
  if (b.weapon.phase == pw::Phase::Impact) {
    if (b.weapon.visualAge >= b.weapon.impactDuration) {
      b.weapon.phase = pw::Phase::Inactive;
      b.active = false;
    }
    return;
  }
  // Native 43aa50: the six Sanae B micro-explosions slow down, then their
  // interrupt-2 animation continues while an independent expanding source lives.
  if (b.update == 4) {
    if (b.speed > 0)
      b.speed = float(double(b.speed) - 0.10000000149011612);
    else {
      b.weapon.phase = pw::Phase::Impact;
      b.weapon.interrupt = 2;
      b.weapon.visualAge = 0;
      b.weapon.impactDuration = 16;
      if (friendlyInterruptHook)
        friendlyInterruptHook(b, 2, false);
      playerDamageSources.circle(b.x, b.y, 12.f, 1.399999976158142f, 15, 4);
    }
    return;
  }
  if (b.update == 2) {
    if (b.option < 1 || b.option > playerMotion.count || shotSchedule.frame < 0 || deathWindow ||
        dialogue) {
      b.weapon.phase = pw::Phase::Impact;
      b.weapon.interrupt = 1;
      b.weapon.visualAge = 0;
      b.weapon.impactDuration = 8;
      if (friendlyInterruptHook) {
        friendlyInterruptHook(b, 1, false);
        friendlyInterruptHook(b, 1, true);
      }
      return;
    }
    if (b.weapon.auxiliaryHit && !hitPrevious) {
      if (friendlyInterruptHook)
        friendlyInterruptHook(b, 3, false);
      b.weapon.auxiliaryHit = false;
    }
    const auto& o = playerMotion.options[b.option - 1];
    b.x = o.x / 128.f;
    b.y = o.y / 128.f;
    // Original 43a6b0, from spawn callback 43a680's zero height.
    b.hitHeight = std::min(448.f, b.hitHeight + 28.f);
    return;
  }
  if (b.update == 1) {
    Enemy* target = playerTarget(b.x, b.y, 256, b.targetId);
    if (!target) {
      b.targetId = 0;
      target = playerTarget(b.x, b.y, 256);
    }
    if (!target)
      b.speed = std::min(16.f, float(double(b.speed) + 0.10000000149011612));
    else {
      b.targetId = target->id;
      const float desired = float(std::atan2(double(target->y - b.y), double(target->x - b.x)));
      const float turn = playerAngle(desired - b.angle);
      if (b.age < 60) {
        if (std::abs(turn) >= 0.7853981852531433)
          b.speed = std::max(4.f, float(double(b.speed) - 0.20000000298023224));
        else if (std::abs(turn) <= 0.2617993950843811f)
          b.speed = std::min(16.f, float(double(b.speed) + 0.20000000298023224));
        b.angle = playerAngle(float(double(b.angle) + double(turn) * 0.07999999821186066));
      } else
        b.speed = float(double(b.speed) + 0.20000000298023224);
    }
  }
  // Sanae A turns horizontally after reaching a nearby enemy's vertical band.
  // The original uses timer 1000..1004 to hold before the horizontal launch.
  if (b.update == 3) {
    if (b.age >= 1000) {
      if (b.age == 1004) {
        b.speed = 24;
        b.angle = b.targetId ? 0 : pi;
      }
      return;
    }
    if (b.age >= 5) {
      auto* target = playerTarget(b.x, b.y, 192);
      if (target && target->y > b.y - 16 && target->y <= b.y + 32) {
        b.speed = 0;
        b.age = 1000;
        b.targetId = target->x >= b.x ? 1 : 0;
      }
    }
  }
}
bool playerProjectileHits(const Projectile& b, const Enemy& e) {
  return b.weapon.phase == pw::Phase::Alive &&
         pw::rectangle(b.x, b.y, b.hitWidth, b.hitHeight, e.x, e.y, e.hitWidth, e.hitHeight,
                       b.type == 2);
}
void sanaeBExplosion(const Projectile& parent) {
  // 43a950 consumes one native game RNG32 and resolves six 60-degree directions.
  const float signedRandom = float(double(random.next32()) / 2147483648.0 - 1);
  float angle = playerAngle(float(double(signedRandom) * double(pi)));
  ShotSpec profile;
  profile.damage = 2;
  profile.w = profile.h = 24;
  profile.speed = 3;
  profile.type = 4;
  profile.animation = 8;
  profile.hitAnimation = 9;
  profile.sound = 40;
  profile.update = 4;
  for (int i = 0; i < 6; ++i) {
    if (std::count_if(bullets.begin(), bullets.end(),
                      [](const Projectile& b) { return b.active && b.friendly; }) >= 256)
      break;
    const float cx = float(std::cos(double(angle)) * 12), cy = float(std::sin(double(angle)) * 12);
    const float spawnX = float(double(parent.x) + cx), spawnY = float(double(parent.y) + cy);
    const float vx = float(std::cos(double(angle)) * 3), vy = float(std::sin(double(angle)) * 3);
    Projectile b{
        float(double(spawnX) - vx), float(double(spawnY) - vy), angle, 3, 12, 4, 0, 0, 2, 13, true};
    b.entityId = nextProjectileId++;
    initializeFriendlyProjectile(b, profile);
    bullets.push_back(b);
    angle = playerAngle(float(double(angle) + 1.0471975803375244));
  }
  eventBits |= 1;
}
int friendlyProjectileDamage(Projectile& b, const Enemy& e) {
  if (!b.active || !b.friendly || !playerProjectileHits(b, e))
    return 0;
  if (b.extraCallback == 1)
    sanaeBExplosion(b);
  b.weapon.hit = true;
  if (b.type == 2 && !b.weapon.auxiliaryHit) {
    if (friendlyInterruptHook)
      friendlyInterruptHook(b, 2, true);
    b.weapon.auxiliaryHit = true;
  }
  int damage = pw::projectileCadence(b.age, b.weapon.previousAge, b.type) ? b.damage : 0;
  if (b.type != 2 && b.type != 4) {
    b.weapon.phase = pw::Phase::Impact;
    b.weapon.visualAge = 0;
    b.weapon.interrupt = 0;
    b.weapon.impactDuration = friendlyImpactDuration(b.hitAnimation);
    b.animation = b.hitAnimation;
    b.speed = float(double(b.speed) * .125);
    if (friendlyHitHook)
      friendlyHitHook(b);
    if (b.weapon.impactDuration == 0) {
      b.weapon.phase = pw::Phase::Inactive;
      b.active = false;
    }
  }
  // All six supplied retail SHTs have hitCallback=0. An unknown custom profile
  // is surfaced instead of inventing a neighboring game's callback semantics.
  if (b.hitCallback)
    ++unsupported[998];
  return damage;
}
uint64_t bombEffectKey(u32 id) {
  return 0x600000000ull + id;
}
void bindBombEffect(int profile, u32 id);
void configurePlayerAnimations() {
  friendlyBirthHook = [](Projectile& b) {
    const auto key = 0x800000000ull + b.entityId;
    auto* node =
        animationScene.bind(key, 0x10000000u + b.entityId, 4 + character, b.animation,
                            anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 0, 0);
    if (node) {
      if (node->vm.state().flags & 0x20000000u) {
        node->vm.control().rotation = b.angle;
        node->vm.control().flags |= 4u;
      }
      animationScene.position(key, b.x + 224, b.y + 16);
    }
    if (b.type == 2)
      animationScene.bind(0x900000000ull + b.entityId, 0x10000000u + b.entityId, 4 + character, 8,
                          anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 0, 0, 23);
  };
  friendlyHitHook = [](Projectile& b) {
    const auto key = 0x800000000ull + b.entityId;
    float rotation = 0;
    if (auto* old = animationScene.find(key))
      rotation = old->vm.state().rotation;
    animationScene.remove(key);
    auto* node =
        animationScene.bind(key, 0x10000000u + b.entityId, 4 + character, b.animation,
                            anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 0, 0);
    if (node) {
      node->vm.control().rotation = rotation;
      animationScene.position(key, b.x + 224, b.y + 16);
    }
  };
  friendlyInterruptHook = [](Projectile& b, int code, bool auxiliary) {
    animationScene.interrupt((auxiliary ? 0x900000000ull : 0x800000000ull) + b.entityId, code);
  };
  playerBombBindHook = [](const pw::BombVisual& v) {
    const auto key = 0x700000000ull + v.id;
    animationScene.bind(key, 0x50000000u + v.id, 4 + v.bank, v.script,
                        anm_logic::Membership::Primary, v.x, v.y, {}, false, nullptr, 224, 16, 23);
  };
  reimuBOrbBindHook = [](const pw::ReimuBOrb& orb) {
    const auto key = 0x700000000ull + orb.id;
    animationScene.bind(key, 0x50000000u + orb.id, 4, 24, anm_logic::Membership::Primary, orb.x,
                        orb.y, {}, false, nullptr, 224, 16, 23);
  };
  playerBombCustomBindHook = [](int profile, u32 id, float x, float y) {
    bindBombEffect(profile, id);
    animationScene.position(bombEffectKey(id), x, y);
  };
}
constexpr uint64_t playerBodyKey = 0xa00000000ull;
void initializePlayerAnimation() {
  animationScene.bind(playerBodyKey, 0xffff0000u, 4 + character, 0, anm_logic::Membership::Embedded,
                      0, 0, {}, false, nullptr, 0, 0);
  animationScene.position(playerBodyKey, px / 128.f + 224, py / 128.f + 16);
}
void tickPlayerAnimation() {
  if (auto* node = animationScene.find(playerBodyKey)) {
    if (node->vm.state().script != playerMotion.bodyAnimation)
      animationScene.rebind(playerBodyKey, 4 + character, playerMotion.bodyAnimation, false);
    animationScene.position(playerBodyKey, px / 128.f + 224, py / 128.f + 16);
    animationScene.tickEmbedded(playerBodyKey);
    auto& pose = node->vm.control();
    const bool visible = playerState != 2 && playerState != 4 && (!invuln || frame % 6 < 3);
    pose.visible = visible;
    if (visible)
      pose.flags |= 1u;
    else
      pose.flags &= ~1u;
  }
}
void bindBombEffect(int profile, u32 id) {
  if (!id || animationScene.find(bombEffectKey(id)))
    return;
  auto* node =
      animationScene.bind(bombEffectKey(id), 0xa0000000u + id, 0, profile == 3 ? 210 : 0,
                          anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 224, 16, 23);
  if (!node)
    return;
  node->customGeometry = profile == 3;
  animationScene.custom(bombEffectKey(id), [profile, id]() {
    const bool retired = tickPlayerBombEffect(profile, id);
    if (profile == 4)
      for (const auto& beam : playerBomb.reimuABeams)
        if (beam.active)
          bindBombEffect(3, beam.id);
    return retired;
  });
}
void syncBombAnimations() {
  for (const auto& visual : playerBomb.visuals)
    if (visual.active) {
      const auto key = 0x700000000ull + visual.id;
      auto* node = animationScene.find(key);
      if (!node) {
        node = animationScene.bind(key, 0x50000000u + visual.id, 4 + visual.bank, visual.script,
                                   anm_logic::Membership::Primary, visual.x, visual.y, {}, false,
                                   nullptr, 224, 16, 23);
        if (node && visual.interrupt)
          animationScene.interrupt(key, visual.interrupt);
      } else {
        animationScene.position(key, visual.x, visual.y);
        if (visual.interrupt && node->vm.diagnostics().pendingInterrupt != visual.interrupt &&
            !node->vm.state().ended) {
          if (visual.age == 0)
            animationScene.interrupt(key, visual.interrupt);
        }
      }
    }
  if (playerBomb.loadout == 0 && playerBomb.reimuAEmitter.active)
    bindBombEffect(4, playerBomb.reimuAEmitter.id);
  for (const auto& leaf : playerBomb.leaves)
    if (leaf.active)
      bindBombEffect(5, leaf.id);
}
void fire() {
  const auto& s = loadouts[character * 2 + shot];
  const int level = std::clamp(power / s.powerStep, 0, s.maxLevel),
            index = level + ((previousHeld & 8) ? s.maxLevel + 1 : 0);
  if (index >= int(s.groups.size()))
    return;
  for (const auto& spec : s.groups[index])
    if (spec.interval > 0 && fireFrame % spec.interval == spec.delay) {
      if (std::count_if(bullets.begin(), bullets.end(),
                        [](const Projectile& p) { return p.friendly && p.active; }) >= 256)
        break;
      if (spec.option > playerMotion.count)
        continue;
      if (spec.type == 2 && std::any_of(bullets.begin(), bullets.end(), [&](const Projectile& b) {
            return b.active && b.friendly && b.type == 2 && b.option == spec.option;
          }))
        continue;
      float x = px / 128.f, y = py / 128.f;
      if (spec.option) {
        const auto& o = playerMotion.options[spec.option - 1];
        x = o.x / 128.f;
        y = o.y / 128.f;
      }
      // Native 439896 subtracts the initial velocity; its update occurs this tick.
      Projectile b{x + spec.x - float(std::cos(double(spec.angle)) * spec.speed),
                   y + spec.y - float(std::sin(double(spec.angle)) * spec.speed),
                   spec.angle,
                   spec.speed,
                   std::max(3.f, spec.w * .5f),
                   spec.type,
                   0,
                   0,
                   spec.damage,
                   spec.animation + 5,
                   true};
      b.option = spec.option;
      b.update = spec.update;
      b.hitWidth = spec.w;
      b.hitHeight = spec.h;
      b.extraCallback = spec.extraCallback;
      if (spec.spawn == 2)
        b.hitHeight = 0;
      b.entityId = nextProjectileId++;
      initializeFriendlyProjectile(b, spec);
      bullets.push_back(b);
    }
}

void tickPlayer(int held, int pressed) {
  const auto& s = loadouts[character * 2 + shot];
  const bool focus = held & 8;
  if (invuln > 0)
    --invuln;
  const int speed = int((focus ? s.focus : s.speed) * playerBomb.speedMultiplier * 128),
            diagonal = int((focus ? s.focusDiag : s.diag) * playerBomb.speedMultiplier * 128);
  int dir = 0;
  if ((held & 0x50) == 0x50)
    dir = 5;
  else if ((held & 0x60) == 0x60)
    dir = 7;
  else if ((held & 0x90) == 0x90)
    dir = 6;
  else if ((held & 0xa0) == 0xa0)
    dir = 8;
  else if (held & 0x20)
    dir = 2;
  else if (held & 0x10)
    dir = 1;
  else if (held & 0x40)
    dir = 3;
  else if (held & 0x80)
    dir = 4;
  const int beforeX = px;
  if (playerState == 1) {
    switch (dir) {
    case 1:
      py -= speed;
      break;
    case 2:
      py += speed;
      break;
    case 3:
      px -= speed;
      break;
    case 4:
      px += speed;
      break;
    case 5:
      px -= diagonal;
      py -= diagonal;
      break;
    case 6:
      px += diagonal;
      py -= diagonal;
      break;
    case 7:
      px -= diagonal;
      py += diagonal;
      break;
    case 8:
      px += diagonal;
      py += diagonal;
      break;
    }
  }
  playerMotion.body(px - beforeX);
  px = std::clamp(px, -0x5c00, 0x5c00);
  if (playerState == 1)
    py = std::clamp(py, 0x1000, 0xd800);
  playerMotion.updateOptions(s, power, px, py, focus);
  tickPlayerAnimation();
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  if (!(oracleFixtureFlags & 4) && (pressed & 2) && bombs > 0 &&
      (playerState == 1 || playerState == 4) && startPlayerBomb()) {
    syncBombAnimations();
    --bombs;
    deathWindow = 0;
    playerState = 1;
    playerStateTicks = 60;
    spellState.onBomb();
    eventBits |= 4;
  }
  if (playerState == 4) {
    deathWindow = std::max(0, 8 - playerStateTicks);
    if (playerStateTicks >= 8) {
      --lives;
      resources.addRank(-1024);
      playerState = 2;
      playerStateTicks = 0;
      deathWindow = 0;
      invuln = 180;
    }
  }
  if (playerState == 2) {
    if (playerStateTicks == 3) {
      const auto plan = item_system::deathPowerDrops(resources, {px / 128.f, py / 128.f});
      for (const auto& request : plan.spawns)
        itemManager.spawn(request);
    }
    if (playerStateTicks >= 30) {
      if (lives < 0)
        ended = 2;
      else {
        playerState = 0;
        playerStateTicks = 0;
        px = 0;
        py = 480 * 128;
        invuln = 280;
        if (bombs < 2) {
          bombs = 2;
          resources.bombFragments = 0;
        }
      }
    }
  } else if (playerState == 0) {
    py = 0xf000 - (playerStateTicks * 0x2800) / 60;
    if (playerStateTicks >= 30)
      for (auto& b : bullets)
        if (!b.friendly && b.active)
          eb::cancel(b.enemy, bulletWorld);
    if (playerStateTicks >= 60) {
      playerState = 1;
      playerStateTicks = 0;
      py = 400 * 128;
    }
  }
  ++playerStateTicks;
  previousHeld = held;
  fireFrame = shotSchedule.begin(held & 1, (playerState == 1 || playerState == 0) && !dialogue);
  if (fireFrame >= 0)
    fire();
  shotSchedule.finish(held & 1);
  tickPlayerDamageSources();
  for (size_t i = 0; i < bullets.size(); i++) {
    auto& b = bullets[i];
    if (!b.active || !b.friendly)
      continue;
    updateFriendlyProjectile(b);
    if (!b.active)
      continue;
    ++b.age;
    if (b.type != 2) {
      b.x = float(double(b.x) + float(std::cos(double(b.angle)) * b.speed));
      b.y = float(double(b.y) + float(std::sin(double(b.angle)) * b.speed));
    }
    b.x = enemy_motion::quantize(b.x);
    b.y = enemy_motion::quantize(b.y);
    if (b.x < -280 || b.x > 280 || b.y < -100 || b.y > 520)
      b.active = false;
  }
  for (const auto& b : bullets)
    if (b.friendly) {
      const auto key = 0x800000000ull + b.entityId;
      if (!b.active) {
        animationScene.remove(key);
        if (b.type == 2)
          animationScene.remove(0x900000000ull + b.entityId);
      } else {
        animationScene.position(key, b.x + 224, b.y + 16);
        if (b.type == 2)
          animationScene.position(0x900000000ull + b.entityId, b.x + 224, b.y + 16);
        if (auto* node = animationScene.find(key)) {
          auto& pose = node->vm.control();
          if (pose.flags & 0x20000000u)
            pose.rotation = b.angle;
        }
      }
    }
}

void tickBomb() {
  tickPlayerBomb();
  bombTimer = playerBomb.active ? playerBomb.age : 0;
  syncBombAnimations();
  for (auto& b : bullets)
    if (b.active && !b.friendly &&
        (b.enemy.phase == eb::Phase::Alive || b.enemy.phase == eb::Phase::Spawning) &&
        !b.enemy.protect.timer) {
      bool clear = false;
      for (const auto& a : playerBomb.rectangles)
        if (std::abs(b.x - a.x) <= a.width * .5f && std::abs(b.y - a.y) <= a.height * .5f)
          clear = true;
      for (const auto& a : playerBomb.circles) {
        const float r = float(double(a.radius) + eb::appearances[b.enemy.type].hitbox * .5f);
        const double x = double(b.x) - a.x, y = double(b.y) - a.y;
        if (x * x + y * y <= double(r) * r)
          clear = true;
      }
      if (clear) {
        eb::cancel(b.enemy, bulletWorld);
        if (!spell && b.x >= -192 && b.x <= 192 && b.y >= 0 && b.y <= 448)
          itemManager.spawn(9, {b.x, b.y}, -pi / 2, .6000000238418579f);
      }
    }
}

} // namespace th12
