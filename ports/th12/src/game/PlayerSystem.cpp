// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "AdaptedInput.hpp"
#include "BulletCancellation.hpp"
#include "CameraEffects.hpp"
#include "FriendlyPool.hpp"
#include "GameState.hpp"
#include "LaserWorld.hpp"
#include "PlayerBounds.hpp"
#include "Practice.hpp"
#include "PracticeTools.hpp"

namespace th12 {
bool playerFocused = false;
bool movementFocus(int held, int totalAge, bool liveEnemies) {
  // 4365a3..4365cc publishes C598 before movement/options/fire consume it.
  return liveEnemies && totalAge >= 4 && (held & 8);
}
namespace {
bool playerMovementFocus(int held) {
  const bool liveEnemies =
      std::any_of(enemies.begin(), enemies.end(), [](const auto& enemy) { return enemy->active; });
  // Player+a48 advances at437475 through every player state and is reset
  // only by436100 stage/session initialization, rather than by respawn.
  return movementFocus(held, gameSpeed == 1 ? frame : int(playerTotalTime), liveEnemies);
}
} // namespace
void awardPlayerGraze(float x, float y, float z) {
  sound_system::queue.play(44, x);
  // 4391c0 awards PIV even when the graze counter has reached its cap.
  if (graze < 99999999)
    ++graze;
  resources.addPointValue(100);
  const auto key = 0x100000000ull + nextDetachedAnimation++;
  animationScene.bind(key, 0, 0, 144, anm_logic::Membership::Primary, x, y, {}, false, nullptr, 224,
                      16, 23, false, z);
  eventBits |= 16;
}
void playerMissCollision() {
  if (invuln || playerState != 1)
    return;
  sound_system::queue.play(2, 0, false);
  // Purple patches43837f to state1 and bypasses the death-effect birth,
  // while retaining4384fd's miss sound and438546's six invulnerability ticks.
  if (practice::tool(practice::Invulnerable)) {
    invuln = 6;
    return;
  }
  // 438370 loses capture eligibility on collision, before the deathbomb window.
  spellState.onMiss(playerBomb.active);
  playerState = 4;
  // 43839e..438498 creates79 then32 copies of80. Original ANM80's
  // first tick owns its four random samples; presentation never creates them.
  const auto effect = [](int script) {
    const auto key = 0x100000000ull + nextDetachedAnimation++;
    animationScene.bind(key, 0, 0, script, anm_logic::Membership::Primary, px / 128.f + 224,
                        py / 128.f + 16, {}, false, nullptr, 0, 0, 0);
  };
  effect(79);
  for (int i = 0; i < 32; ++i)
    effect(80);
  playerStateTicks = 0;
  playerStateTime = 0;
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
namespace {
std::unordered_map<uint64_t, int> bombDeliveredInterrupts;
std::array<float, 3> respawnClearOrigin{}; // Native Player+a08, saved death position.
// Stage primitive handles occupy c00000000+slot. A Bomb must never replace
// one of those embedded VMs or let Stage12 advance its backdrop a second time.
constexpr uint64_t BombBackdropKeyBase = 0x7800000000000000ull;
} // namespace
std::vector<PlayerClearOperation> playerClearOperations(int state, int age) {
  if ((state == 1 && age < 30) || (state == 0 && age >= 30))
    return {{2, 0, true}, {3, 0, true}};
  if (state != 0)
    return {};
  const float outer = float(double(age) * 512 / 30 + 64);
  const float inner = float(double(outer) * .25);
  return {{0, outer, true}, {0, inner, false}, {1, outer, true}, {1, inner, false}};
}
namespace {
void clearPlayerField(int state, int age) {
  // Passive Enemy/Laser/ANM fixtures omit the original Player16 callback.
  // Their explicit flag8 omits this separate entry-clear path too. Active
  // replay restores always start with flags0 and execute all native clears.
  if (oracleFixtureFlags & 8)
    return;
  for (const auto& operation : playerClearOperations(state, age)) {
    if (operation.kind == 0)
      cancelHostileCircle(respawnClearOrigin[0], respawnClearOrigin[1], operation.radius, false,
                          operation.protection);
    else if (operation.kind == 1)
      laserManager.clearCircle(
          laserWorld, {respawnClearOrigin[0], respawnClearOrigin[1], respawnClearOrigin[2]},
          operation.radius, 0, operation.protection);
    else if (operation.kind == 2)
      cancelHostileBullets(operation.protection);
    else
      laserManager.clearAll(laserWorld, false, operation.protection);
  }
}
} // namespace
Enemy* playerTarget(float, float, float, u32);
void explodeReimuBOrb(pw::ReimuBOrb& orb) {
  if (!orb.active)
    return;
  sound_system::queue.play(28, orb.x); // Native408047, each orb's burst.
  // Native4084a8 queues this pulse for a damage-triggered individual burst.
  // The full timeout burst owns one shared pulse in408380 instead.
  if (playerBomb.age < 200)
    cameraEffects.pulse(8, 6, 6);
  playerBomb.circles.push_back({orb.x, orb.y, 128, 2});
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
  // Original ReimuA46a5f0 increments at46a7de and46a82c; the other
  // five constructors increment EnemyManager+14 once.
  phasePlayerCounters[1] += loadout == 0 ? 2u : 1u;
  playerBomb.x = px / 128.f;
  playerBomb.y = py / 128.f;
  sound_system::queue.play(46, playerBomb.x); // Native406c6c, common declaration before loadout.
  // Native46a83e uses the player's pan; the other constructors use453d90.
  sound_system::queue.play(loadout == 5 ? 31 : 51, playerBomb.x, loadout == 0);
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
  if (loadout == 2)
    cameraEffects.shake(3, 60, 240, 30); // Actual4071aa profile8.
  else if (loadout == 0)
    cameraEffects.shake(2, 60, 240, 30); // Actual46a950 profile8.
  else if (loadout == 3)
    cameraEffects.shake(2, 60, 120, 30); // Actual407860 profile8.
  else if (loadout == 4)
    cameraEffects.shake(2, 180, 60, 1); // Actual408aa3 profile8.
  {
    // Native407250/408eba bind front1 synchronously after the player's main
    // VM. Its independent handle receives IRQ1 when the Bomb stops.
    const auto key = BombBackdropKeyBase + nextDetachedAnimation++;
    auto* backdrop =
        animationScene.bind(key, 0x50000000u, 7, 1, anm_logic::Membership::Primary,
                            playerBomb.x + 224, playerBomb.y + 16, {}, false, nullptr, 0, 0, 23);
    if (loadout == 2 && backdrop) {
      // Actual4072a5 writes the primary alpha after front1's birth tick.
      // 4072bf also writes the unused secondary-alpha curveB (+27c); front1
      // uses mode0, so that retained coefficient does not change its fade.
      auto& pose = backdrop->vm.control();
      pose.primary = (pose.primary & 0xffffffu) | 0xe0000000u;
    }
    playerBomb.backgroundAnimation = key;
  }
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
    if (b.interrupt && b.visualAge == 0)
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
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
    // Actual408f00 resolves the main handle before creating sources/leaves.
    // Without a resource bank, isolated logic fixtures retain their legacy
    // scripted lifetime; the game uses the actual primary VM's deletion.
    const bool sceneLifetime = animationScene.registry.bank(6) != nullptr;
    const bool mainMissing =
        sceneLifetime && (playerBomb.visuals.empty() ||
                          !animationScene.find(0x700000000ull + playerBomb.visuals[0].id));
    if (mainMissing || age >= (sceneLifetime ? 400 : 281)) {
      playerBomb.active = false;
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
      playerBomb.visuals.clear();
      return;
    }
    if (age == 0)
      playerDamageSources.circle(playerBomb.x, playerBomb.y, 16, .35555556416511536f, 180, 13);
    if (age == 180) {
      sound_system::queue.play(6, 0, false); // Native408f53, frog burst.
      cameraEffects.shake(4, 1, 60, 10); // Actual408f89 profile8.
      playerDamageSources.circle(playerBomb.x, playerBomb.y, 80, 20, 100, 80);
    }
    if (age >= 30) {
      float x, y;
      if (age < 250) {
        const auto* main = playerBomb.visuals.empty()
                               ? nullptr
                               : animationScene.find(0x700000000ull + playerBomb.visuals[0].id);
        // 40902b scales the random radius by the live main VM's scaleX.
        const float radius =
            float(double(main ? main->vm.state().sx : 1.f) * pw::unitRandom(random.next32()));
        const float direction = float(double(pw::signedRandom(random.next32())) * double(pi));
        x = float(double(playerBomb.x) + float(std::cos(double(direction)) * radius));
        y = float(double(playerBomb.y) + float(std::sin(double(direction)) * radius));
      } else {
        x = float(double(pw::signedRandom(random.next32())) * 192);
        y = float(double(pw::unitRandom(random.next32())) * 448);
      }
      pw::Leaf leaf;
      // 40908f adds the viewport before4105e0 stores point0. The custom
      // history therefore integrates screen coordinates, including rounding.
      leaf.begin(nextProjectileId++, float(double(x) + 224), float(double(y) + 16), random);
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
    const bool mainMissing = animationScene.registry.bank(5) &&
                             (playerBomb.visuals.empty() ||
                              !animationScene.find(0x700000000ull + playerBomb.visuals[0].id));
    if (mainMissing || age > 300) {
      playerBomb.active = false;
      playerBomb.speedMultiplier = 1;
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
      playerBomb.tailAge = 0;
      if (mainMissing)
        playerBomb.visuals.clear();
      else if (!playerBomb.visuals.empty()) {
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
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
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
    if (playerBomb.active && age >= 200) {
      cameraEffects.pulse(8, 6, 6); // Actual408380, once for the whole burst.
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
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
              if (s.accumulated >= 300) {
                explodeReimuBOrb(orb);
                sound_system::queue.play(28, orb.x); // Native408473 adds a second request.
              } else {
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
    if (age == 240) {
      animationScene.interrupt(playerBomb.backgroundAnimation, 1);
      sound_system::queue.play(46, 0, false); // Native408be4, wind release.
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
      sound_system::queue.play(28, playerBomb.x); // Native46a364, repeated beam volleys.
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
  if (s.spawn == 2)
    sound_system::queue.play(21, px / 128.f); // Native43a699, Marisa A option laser.
  if (friendlyBirthHook)
    friendlyBirthHook(b);
}
void tickPlayerDamageSources() {
  playerDamageSources.tick(gameSpeed);
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
    // Native41a9f0/43a54e exclude these targeting flags, not damage immunity.
    // Newly spawned enemies remain valid targets during their HP immunity tick.
    if (!e.active || (e.flags & 0x6000021u))
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
  // Native43a6b0 restores the source anchor even during fade and State4.
  // Read the retained option coordinates before checking active option count.
  if (b.update == 2 && b.option >= 1 && b.option <= 4) {
    const auto& option = playerMotion.options[b.option - 1];
    b.x = option.x / 128.f;
    b.y = option.y / 128.f;
  }
  b.weapon.previousAge = b.age;
  b.weapon.hit = false;
  ++b.weapon.visualAge;
  if (b.weapon.phase == pw::Phase::Impact) {
    const bool sceneOwnsLifetime = bool(animationScene.registry.bank(4 + character));
    if (sceneOwnsLifetime ? !animationScene.find(0x800000000ull + b.entityId)
                          : b.weapon.visualAge >= b.weapon.impactDuration) {
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
    // 43a73d advances collision height before testing the shooting latch.
    //  The release pass grows once, then the retained phase2 tail stops growing.
    b.hitHeight = std::min(448.f, b.hitHeight + 28.f);
    if (b.option < 1 || b.option > playerMotion.count || shotSchedule.frame < 0 || dialogue) {
      b.weapon.phase = pw::Phase::Impact;
      b.weapon.interrupt = 1;
      b.weapon.visualAge = 0;
      b.weapon.impactDuration = 8;
      sound_system::queue.stop(21); // Native43a7cd, release the option laser's loop.
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
    return;
  }
  if (b.update == 1) {
    bool rejectedRetainedTarget = false;
    if (b.targetId)
      for (const auto& pointer : enemies)
        if (pointer->active && pointer->id == b.targetId) {
          // Native43a55f releases a still-linked target that becomes hidden or
          // untargetable, then takes the no-target acceleration path. A new
          // acquisition starts on the following update, not this one.
          rejectedRetainedTarget = bool(pointer->flags & 0x6000021u);
          if (rejectedRetainedTarget)
            b.targetId = 0;
          break;
        }
    Enemy* target = rejectedRetainedTarget ? nullptr : playerTarget(b.x, b.y, 256, b.targetId);
    if (!target && !rejectedRetainedTarget) {
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
      // Native43a89c/43a8b4 include both vertical-band endpoints. Equality
      // at y-16 must stop now, before another20px upward movement.
      if (target && target->y >= b.y - 16 && target->y <= b.y + 32) {
        b.speed = 0;
        b.weapon.interrupt = 2;
        if (friendlyInterruptHook)
          friendlyInterruptHook(b, 2, false); // Native461970, deferred VM IRQ.
        b.weapon.previousAge = 999;
        b.age = 1000;
        b.weapon.ageTime = 1000.f; // Native4067e0 resets the complete timer.
        b.targetId = target->x >= b.x ? 1 : 0;
      }
    }
  }
}
bool playerProjectileHits(const Projectile& b, const Enemy& e) {
  return b.weapon.phase == pw::Phase::Alive &&
         player_bounds::hitRectangle(b.x, b.y, b.hitWidth, b.hitHeight, e.x, e.y, e.hitWidth,
                                     e.hitHeight, b.type == 2);
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
    const int slot = reserveFriendlySlot();
    if (slot < 0)
      break;
    const float cx = float(std::cos(double(angle)) * 12), cy = float(std::sin(double(angle)) * 12);
    const float spawnX = float(double(parent.x) + cx), spawnY = float(double(parent.y) + cy);
    const float vx = float(std::cos(double(angle)) * 3), vy = float(std::sin(double(angle)) * 3);
    Projectile b{
        float(double(spawnX) - vx), float(double(spawnY) - vy), angle, 3, 12, 4, 0, 0, 2, 13, true};
    b.entityId = nextProjectileId++;
    b.physicalSlot = slot;
    initializeFriendlyProjectile(b, profile);
    bullets.push_back(b);
    attachFriendlySlot(bullets.back());
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
  respawnClearOrigin = {};
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
    bombDeliveredInterrupts.erase(key);
    // 4077d6..4077e7 creates pl01/24 with a null position and flags0.
    // Its children25/26 supply absolute screen XY via ANM48; adding the
    // captured player position/playfield origin shifts both sweep beams.
    const bool screenSweep = v.bank == 1 && v.script == 24;
    animationScene.bind(key, 0x50000000u + v.id, 4 + v.bank, v.script,
                        anm_logic::Membership::Primary, screenSweep ? 0 : v.x,
                        screenSweep ? 0 : v.y, {}, false, nullptr,
                        screenSweep ? 0 : 224, screenSweep ? 0 : 16, 23);
  };
  reimuBOrbBindHook = [](const pw::ReimuBOrb& orb) {
    const auto key = 0x700000000ull + orb.id;
    bombDeliveredInterrupts.erase(key);
    animationScene.bind(key, 0x50000000u + orb.id, 4, 24, anm_logic::Membership::Primary, orb.x,
                        orb.y, {}, false, nullptr, 224, 16, 23);
  };
  playerBombCustomBindHook = [](int profile, u32 id, float x, float y) {
    bindBombEffect(profile, id);
    animationScene.position(bombEffectKey(id), x, y);
  };
}
constexpr uint64_t playerBodyKey = 0xa00000000ull;
void updatePlayerFocusAnimation(bool focused) {
  // Native4364f0 only updates the handle while movement is active. The
  // independent primary VMs keep running through a miss/respawn.
  if (playerState != 1)
    return;
  auto* node = animationScene.find(playerFocusAnimation);
  if (focused) {
    if (!node) {
      const auto key = 0xb00000000ull + nextDetachedAnimation++;
      // Native436606 creates bullet77 at zero before writing the moved
      // player position. It creates child76; both scripts select layer16.
      node = animationScene.bind(key, 0xffff0020u, 0, 77, anm_logic::Membership::Primary, 0, 0, {},
                                 false, nullptr, 0, 0, 12);
      playerFocusAnimation = node ? key : 0;
    }
    if (node)
      animationScene.position(playerFocusAnimation, px / 128.f + 224, py / 128.f + 16);
  } else {
    // Native436684 queues interrupt1 on the linked VMs and releases the
    // player handle. A fresh press can overlap their eight-tick retirement.
    if (node)
      animationScene.interrupt(playerFocusAnimation, 1);
    playerFocusAnimation = 0;
  }
}
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
  node->customGeometry = profile == 3 || profile == 5;
  if (profile == 5)
    node->vm.control().layer = 13; // Actual410706 custom leaf draw layer.
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
      // Birth hooks own registration. A missing native461920 handle stays
      // missing; synchronizing presentation must not resurrect a deleted VM.
      if (node) {
        const bool screenSweep = visual.bank == 1 && visual.script == 24;
        animationScene.position(key, screenSweep ? 0 : visual.x, screenSweep ? 0 : visual.y);
        if (visual.interrupt && bombDeliveredInterrupts[key] != visual.interrupt &&
            !node->vm.state().ended) {
          animationScene.interrupt(key, visual.interrupt);
          bombDeliveredInterrupts[key] = visual.interrupt;
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
            index = level + (playerFocused ? s.maxLevel + 1 : 0);
  if (index >= int(s.groups.size()))
    return;
  for (const auto& spec : s.groups[index])
    if (spec.interval > 0 && fireFrame % spec.interval == spec.delay) {
      const int slot = reserveFriendlySlot();
      if (slot < 0)
        break;
      if (spec.option > playerMotion.count)
        continue;
      if (spec.type == 2 && std::any_of(bullets.begin(), bullets.end(), [&](const Projectile& b) {
            // 43a7c1 releases the option's C430 latch when its old beam enters
            // phase2. Its eight-tick ANM tail must not block a newly fired beam.
            return b.active && b.friendly && b.type == 2 && b.weapon.phase == pw::Phase::Alive &&
                   b.option == spec.option;
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
      b.physicalSlot = slot;
      initializeFriendlyProjectile(b, spec);
      sound_system::queue.play(spec.sound, x);
      bullets.push_back(b);
      attachFriendlySlot(bullets.back());
    }
}

void tickPlayer(int held, int pressed) {
  if (int(playerStateTime) != playerStateTicks) playerStateTime = float(playerStateTicks);
  if (int(playerInvulnerabilityTime) != invuln) playerInvulnerabilityTime = float(invuln);
  const auto& s = loadouts[character * 2 + shot];
  // The state0 branch computes its entry position and clears first. At age60
  // it falls through to the normal state1 branch in this same Player pass.
  if (playerState == 0) {
    playerMotion.resetOptionPositions(); // 436c07 ->412610 retains positions, marks reset.
    py = 0xf000 - (playerStateTicks * 0x2800) / 60;
    clearPlayerField(0, playerStateTicks);
    if (playerStateTicks >= 60) {
      playerState = 1;
      playerStateTicks = 0;
      playerStateTime = 0;
    }
  }
  const bool beganMoving = playerState == 1;
  const bool beganInDeathWindow = playerState == 4;
  const bool liveEnemies =
      std::any_of(enemies.begin(), enemies.end(), [](const auto& enemy) { return enemy->active; });
  // 436d18 constructs the Bomb before436d47 advances the player's position.
  // SanaeB retains that birth center for sources, clearing and leaf effects.
  // The deathbomb branch436da8 bypasses movement for this entire Player pass.
  if (!(oracleFixtureFlags & 4) && (held & 2) && bombs > 0 &&
      ((playerState == 1 && !dialogue && liveEnemies) ||
       (playerState == 4 && playerStateTicks < 8)) &&
      startPlayerBomb()) {
    syncBombAnimations();
    practice::recordBomb();
    if (!practice::tool(practice::InfiniteBombs)) --bombs;
    deathWindow = 0;
    playerState = 1;
    // 436d18 preserves a normal state's age; only436da8 deathbomb resets60.
    if (beganInDeathWindow)
      playerStateTicks = 60;
    if (beganInDeathWindow) playerStateTime = 60;
    spellState.onBomb();
    //4385b0 refreshes options here, but43875c returns without resetting
    //their positions when the option count is unchanged.
    eventBits |= 4;
  }
  if (beganMoving)
    clearPlayerField(1, playerStateTicks);
  if (beganMoving)
    playerFocused = playerMovementFocus(held);
  const bool focus = playerFocused;
  const int speed = int((focus ? s.focus : s.speed) * playerBomb.speedMultiplier * 128),
            diagonal = int((focus ? s.focusDiag : s.diag) * playerBomb.speedMultiplier * 128);
  // 43666d2 truncates SHT fixed speed*Bomb factor first.436782 then
  // scales each signed displacement by4b2ed0 and truncates again.
  const int scaledSpeed = int(float(double(speed) * gameSpeed)),
            scaledDiagonal = int(float(double(diagonal) * gameSpeed));
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
  if (beganMoving && !((held & 0xf0) == 0 && adaptedMotion.apply(px, py, scaledSpeed))) {
    switch (dir) {
    case 1:
      py -= scaledSpeed;
      break;
    case 2:
      py += scaledSpeed;
      break;
    case 3:
      px -= scaledSpeed;
      break;
    case 4:
      px += scaledSpeed;
      break;
    case 5:
      px -= scaledDiagonal;
      py -= scaledDiagonal;
      break;
    case 6:
      px += scaledDiagonal;
      py -= scaledDiagonal;
      break;
    case 7:
      px -= scaledDiagonal;
      py += scaledDiagonal;
      break;
    case 8:
      px += scaledDiagonal;
      py += scaledDiagonal;
      break;
    }
  }
  if (!beganInDeathWindow && playerState != 4)
    playerMotion.body(px - beforeX);
  px = std::clamp(px, -0x5c00, 0x5c00);
  if (playerState == 1)
    py = std::clamp(py, 0x1000, 0xd800);
  if (beganMoving)
    playerMotion.updateOptions(s, power, px, py, focus);
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  if (playerState == 4) {
    deathWindow = std::max(0, 8 - playerStateTicks);
    if (playerStateTicks >= 8) {
      // Native436d61 commits a miss only after the deathbomb window expires.
      ++phasePlayerCounters[0];
      phasePlayerCounters[2] = 0;
      practice::recordMiss();
      if (!practice::tool(practice::InfiniteLives) ||
          (practice::toolSettings().map_inf_life_to_no_continue && lives>0)) --lives;
      resources.addRank(-1024);
      playerState = 2;
      playerStateTicks = 0;
      playerStateTime = 0;
      deathWindow = 0;
      invuln = 180;
      playerMotion.count = 0;
      playerMotion.bodyAnimation = 0;
      initializePlayerAnimation(); // Native4381e0 resets the embedded body VM.
    }
  }
  if (playerState == 2) {
    if (playerStateTicks == 3) {
      const int powerBeforeLoss = power;
      const auto plan = item_system::deathPowerDrops(resources, {px / 128.f, py / 128.f});
      if (practice::tool(practice::InfinitePower)) power = powerBeforeLoss;
      for (const auto& request : plan.spawns)
        itemManager.spawn(request);
      playerMotion.updateOptions(s, power, px, py, focus, true);
    }
    if (playerStateTicks >= 30) {
      if (lives < 0)
        ended = 2;
      else {
        // 436ec8..436ed7 restores the global rate before the respawn source
        // and common tail. This also changes Enemy18/Spell23 in this frame,
        // including a BossDead half/quarter-speed sequence still in progress.
        gameSpeed = animationScene.runtime.delta = 1;
        // 436efc creates the expanding damage source before saving+a08 and
        // resetting the player position. Player16's common source pass below
        // advances the new source in this same callback.
        respawnClearOrigin = {px / 128.f, py / 128.f, 0};
        playerDamageSources.circle(respawnClearOrigin[0], respawnClearOrigin[1], 32, 16, 30, 150);
        playerState = 0;
        playerStateTicks = 0;
        playerStateTime = 0;
        px = 0;
        py = 480 * 128;
        invuln = 280;
        if (bombs < 2) {
          bombs = 2;
          // 436f0f -> 422f60 restores stock only; HUD receives the retained
          // fragment count at4b0ca4, including a partially earned next Bomb.
        }
      }
    }
  }
  playerPreviousStateTicks = playerStateTicks;
  playerStateTime = float(double(playerStateTime) + gameSpeed);
  playerStateTicks = int(playerStateTime);
  playerTotalTime = gameSpeed == 1 ? float(frame + 1) : float(double(playerTotalTime) + gameSpeed);
  if (int(playerInvulnerabilityTime) != invuln) playerInvulnerabilityTime = float(invuln);
  if (invuln > 0) {
    playerInvulnerabilityTime = float(double(playerInvulnerabilityTime) - gameSpeed);
    invuln = int(playerInvulnerabilityTime);
  } // Native common tail follows state-transition writes.
  // Native state0 reaches4364f0 on its transition back to state1. States2,
  // 3 and4 leave C598/its handle alone until movement actually resumes.
  if (!beganMoving && !beganInDeathWindow && playerState == 1)
    playerFocused = playerMovementFocus(held);
  if (!beganInDeathWindow)
    updatePlayerFocusAnimation(playerFocused);
  tickPlayerAnimation();
  // 4374dd..437533 uses the current Player state timer, after it advances.
  if (!dialogue && playerStateTicks % 60 == 0)
    resources.addRank(1);
  previousHeld = held;
  // Successful436da8 restores state1 before the common tail.439a40 checks
  // that current state, so firing resumes on the deathbomb frame even though
  // movement/focus remained skipped in its earlier state4 branch.
  if (playerState == 1) {
    fireFrame = shotSchedule.begin(held & 1, !dialogue);
    if (fireFrame >= 0 && shotSchedule.emitThisTick)
      fire();
    shotSchedule.finish(held & 1, gameSpeed);
  }
  tickPlayerDamageSources();
  // Native436fxx updates the physical shot array in slot order. New shots
  // in a slot already visited wait until the next effective Player16 update.
  for (int slot = 0; slot < FriendlyCapacity; ++slot) {
    auto* pointer = friendlySlots[slot];
    if (!pointer || !pointer->active)
      continue;
    auto& b = *pointer;
    updateFriendlyProjectile(b);
    if (!b.active)
      continue;
    if (b.type != 2) {
      b.x = float(double(b.x) + float(std::cos(double(b.angle)) * b.speed));
      b.y = float(double(b.y) + float(std::sin(double(b.angle)) * b.speed));
    }
    b.x = enemy_motion::quantize(b.x);
    b.y = enemy_motion::quantize(b.y);
    if (auto* node = animationScene.find(0x800000000ull + b.entityId)) {
      const auto& pose = node->vm.state();
      const auto bank = animationScene.registry.bank(pose.spriteBank);
      if (bank && pose.sprite >= 0 && pose.sprite < int(bank->sprites.size()) &&
          (pose.mode == 0 || pose.mode == 1)) {
        const auto& sprite = bank->sprites[pose.sprite];
        const auto transform = resolveAnimationTransform(*node);
        player_bounds::Quad quad{transform.x + node->viewportX,
                                 transform.y + node->viewportY,
                                 transform.z,
                                 float(double(sprite.width) * transform.sx),
                                 float(double(sprite.height) * transform.sy),
                                 transform.rotation,
                                 pose.mode,
                                 pose.anchorX,
                                 pose.anchorY};
        if (!player_bounds::survives(b.type, b.age, b.x, b.y, player_bounds::vertices(quad)))
          b.active = false;
      }
    } else if (b.x < -280 || b.x > 280 || b.y < -100 || b.y > 520)
      b.active = false;
    if (int(b.weapon.ageTime) != b.age) b.weapon.ageTime = float(b.age);
    b.weapon.ageTime = float(double(b.weapon.ageTime) + gameSpeed);
    b.age = int(b.weapon.ageTime);
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
  for (const auto& area : playerBomb.rectangles) {
    if (!area.enabled)
      continue;
    cancelHostileRectangle(area.x, area.y, area.width, area.height, !spell);
    laserManager.clearRectangle(laserWorld, {area.x, area.y, 0}, {area.width, area.height, 0},
                                !spell, true);
  }
  for (const auto& area : playerBomb.circles) {
    cancelHostileCircle(area.x, area.y, area.radius, !spell, true);
    laserManager.clearCircle(laserWorld, {area.x, area.y, 0}, area.radius,
                             uint32_t(!spell) | area.laserCancelFlags, true);
  }
}

} // namespace th12
