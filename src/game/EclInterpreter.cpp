// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "EclInterpreter.hpp"
#include "EnemyCallbacks.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "LaserWorld.hpp"

namespace th12 {
void referenceFault(const Context& c, int operand, bool floating, i32 value) {
  const uint64_t key = (uint64_t(rd<u16>(c.pc + 4)) << 48) | (uint64_t(uint8_t(operand)) << 40) |
                       (uint64_t(floating) << 32) | u32(value);
  if (referenceFaults.size() < 256 || referenceFaults.contains(key))
    ++referenceFaults[key];
  ++unsupported[1000];
}
float global(Enemy& e, int id, bool floating) {
  if (id >= -9985 && id <= -9982)
    return float(i32(e.variables[id + 9985]));
  if (id >= -9981 && id <= -9978)
    return asfloat(e.variables[id + 9985]);
  if (id >= -9935 && id <= -9932)
    return asfloat(e.variables[id + 9943]);
  switch (id) {
  case -10000:
    return float(random.next32());
  case -9999:
    return random.unit();
  case -9998:
    return random.signedUnit() * pi;
  case -9987:
    return random.signedUnit();
  case -9997:
  case -9977:
    return e.x;
  case -9996:
  case -9976:
    return e.y;
  case -9995:
  case -9975:
    return e.ax;
  case -9994:
  case -9974:
    return e.ay;
  case -9993:
  case -9973:
    return e.rx;
  case -9992:
  case -9972:
    return e.ry;
  case -9991:
  case -9965:
    return px / 128.f;
  case -9990:
  case -9964:
    return py / 128.f;
  case -9989:
    return aim(e.x, e.y);
  case -9988:
    return e.age;
  case -9986:
    return e.timedOut;
  case -9971:
    return e.angle;
  case -9969:
    return e.speed;
  case -9970:
    return e.relativeAngle;
  case -9968:
    return e.relativeSpeed;
  case -9963: {
    auto* b = primaryBoss();
    return b ? b->x : 0;
  }
  case -9962: {
    auto* b = primaryBoss();
    return b ? b->y : 224;
  }
  case -9960:
    return 0;
  case -9959:
    return difficulty;
  case -9954:
    return e.life;
  case -9957:
    return 1;
  case -9944: {
    const double dx = e.x - px / 128.f, dy = e.y - py / 128.f;
    return float(std::sqrt(dx * dx + dy * dy));
  }
  case -9953:
  case -9952:
  case -9951:
  case -9950:
    return difficulty == id + 9953;
  case -9946: {
    int n = 0;
    for (auto& p : enemies)
      n += p->active && !p->hidden;
    return n;
  }
  case -9945:
    return character * 2 + shot;
  default: {
    auto found = globals.find(id);
    return found == globals.end() ? 0 : found->second.floating();
  }
  }
}
i32 globalInt(Enemy& e, int id) {
  if (id == -10000)
    return i32(random.next32());
  if (id >= -9985 && id <= -9982)
    return i32(e.variables[id + 9985]);
  if (auto found = globals.find(id); found != globals.end())
    return found->second.integral();
  return nativeInt(global(e, id, false));
}
i32 argInt(Context& c, Enemy& e, int index, int referenceIndex) {
  const i32 raw = rd<i32>(c.pc + 16 + index * 4);
  if (referenceIndex < 0)
    referenceIndex = index;
  if (referenceIndex >= 16 || !(rd<u16>(c.pc + 8) & (1 << referenceIndex)))
    return raw;
  if (raw == -1)
    return c.popInt();
  if (raw < 0)
    return globalInt(e, raw);
  if (raw >= 1024) {
    referenceFault(c, index, false, raw);
    return 0;
  }
  return i32(c.locals[raw / 4]);
}
float arg(Context& c, Enemy& e, int index, bool isFloat, int referenceIndex) {
  const u32 raw = rd<u32>(c.pc + 16 + index * 4);
  if (referenceIndex < 0)
    referenceIndex = index;
  const bool ref = referenceIndex < 16 && (rd<u16>(c.pc + 8) & (1 << referenceIndex));
  if (!isFloat)
    return float(argInt(c, e, index, referenceIndex));
  const float f = asfloat(raw);
  if (!ref)
    return f;
  const int id = int(f);
  if (id == -1)
    return c.pop();
  if (id < 0) {
    const float v = global(e, id, isFloat);
    return isFloat ? v : float(i32(v));
  }
  if (id >= 1024) {
    referenceFault(c, index, true, id);
    return 0;
  }
  return isFloat ? asfloat(c.locals[id / 4]) : float(i32(c.locals[id / 4]));
}
void store(Context& c, Enemy& e, int id, float value, bool isFloat) {
  const u32 raw = isFloat ? asbits(value) : u32(i32(value));
  if (id >= 0 && id < 1024)
    c.locals[id / 4] = raw;
  else if (id >= -9985 && id <= -9978)
    e.variables[id + 9985] = raw;
  else if (id >= -9935 && id <= -9932)
    e.variables[id + 9943] = raw;
  else
    globals[id] = {raw, !isFloat};
}
void storeInt(Context& c, Enemy& e, int id, i32 value) {
  if (id >= 0 && id < 1024)
    c.locals[id / 4] = u32(value);
  else if (id >= -9985 && id <= -9978)
    e.variables[id + 9985] = u32(value);
  else
    globals[id] = {u32(value), true};
}
std::string stringArg(Context& c, int at) {
  const auto* p = c.pc + 16 + at * 4;
  const u32 n = rd<u32>(p);
  if (n > 4096)
    return {};
  return std::string(reinterpret_cast<const char*>(p + 4),
                     strnlen(reinterpret_cast<const char*>(p + 4), n));
}
std::array<u32, 256> callArguments(Context& c, Enemy& e, int skipped) {
  std::array<u32, 256> out{};
  const u32 length = rd<u32>(c.pc + 16);
  const u8* descriptors = c.pc + 16 + length + 4 + skipped * 4;
  const u32 count = c.pc[11];
  for (u32 i = skipped + 1, slot = 0; i < count && slot < 256; i++, slot++, descriptors += 8) {
    const bool f = descriptors[0] == 'f' || descriptors[0] == 'g';
    u32 raw = rd<u32>(descriptors + 4);
    Value value{raw, !f};
    if (i < 16 && (rd<u16>(c.pc + 8) & (1 << i))) {
      int id = f ? nativeInt(asfloat(raw)) : i32(raw);
      if (id == -1)
        value = c.popValue();
      else if (id < 0)
        value = f ? Value{asbits(global(e, id, true)), false} : Value{u32(globalInt(e, id)), true};
      else if (id < 1024)
        value = {c.locals[id / 4], !f};
      else
        value = {};
    }
    out[slot] = descriptors[1] == 'f' ? asbits(value.floating()) : u32(value.integral());
  }
  return out;
}

void command(Enemy& e, Context& c, int op) {
  auto I = [&](int i) { return argInt(c, e, i); };
  auto F = [&](int i) { return arg(c, e, i, true); };
  const int sid = (op >= 500 && op <= 528 && op != 512 && op != 513 && op != 520) ||
                          (op >= 600 && op <= 603) || op == 611
                      ? std::clamp(I(0), 0, 15)
                      : 0;
  auto& s = e.shooters[sid];
  switch (op) {
  case 256:
  case 257:
  case 260:
  case 261:
  case 265:
  case 266:
  case 267:
  case 268:
  case 280: {
    if (op >= 265 && op <= 268 && primaryBoss())
      break;
    const std::string name = stringArg(c);
    const int at = (rd<u32>(c.pc + 16) + 4) / 4;
    float x = arg(c, e, at, true, 1), y = arg(c, e, at + 1, true, 2);
    const int life = argInt(c, e, at + 2, 3), points = argInt(c, e, at + 3, 4),
              drop = argInt(c, e, at + 4, 5);
    if (op == 256 || op == 260 || op == 265 || op == 267 || op == 280) {
      x = float(double(x) + e.x);
      y = float(double(y) + e.y);
    }
    spawn(name, x, y, life, points, drop, op == 260 || op == 261 || op == 267 || op == 268, &e);
    break;
  }
  case 258:
    e.bank = I(0);
    break;
  case 259:
  case 262: {
    const int slot = I(0), script = I(1);
    if (slot == 0) {
      e.baseAnimation = e.animation = script;
      e.boundBank = e.bank;
      e.animationAge = 0;
    }
    bindEnemyAnimation(e, slot, script);
    break;
  }
  case 269: {
    const int slot = I(0), script = e.baseAnimation + 5;
    if (slot == 0) {
      e.animation = script;
      e.boundBank = e.bank;
      e.animationAge = 0;
    }
    bindEnemyAnimation(e, slot, script);
    break;
  }
  case 270:
  case 271: {
    // 4157b4/4157c3: world/background birth, camera0 offset, not enemyXY.
    if (op == 271 && primaryBoss())
      break;
    const std::string name = stringArg(c);
    const int at = (rd<u32>(c.pc + 16) + 4) / 4;
    const float x = float(double(arg(c, e, at, true, 1)) + backgroundEnemyOrigin[0]),
                y = float(double(arg(c, e, at + 1, true, 2)) + backgroundEnemyOrigin[1]),
                z = arg(c, e, at + 2, true, 3);
    const int life = argInt(c, e, at + 3, 4), points = argInt(c, e, at + 4, 5),
              drop = argInt(c, e, at + 5, 6);
    spawn(name, x, y, life, points, drop, false, &e, 0x2000000u, z);
    break;
  }
  case 274: {
    const int slot = I(0), script = e.baseAnimation + I(1) + 5;
    if (slot == 0) {
      e.animation = script;
      e.boundBank = e.bank;
      e.animationAge = 0;
    }
    bindEnemyAnimation(e, slot, script);
    break;
  }
  case 276:
    e.animation = e.baseAnimation;
    e.boundBank = e.bank;
    e.animationAge = 0;
    bindEnemyAnimation(e, 0, e.baseAnimation);
    break;
  case 263:
  case 264:
    detachedAnimation(I(0), I(1), e, op == 263);
    break;
  case 275:
    break;
  case 300:
  case 302: {
    const float x = F(0), y = F(1);
    float& xx = op == 300 ? e.ax : e.rx;
    float& yy = op == 300 ? e.ay : e.ry;
    if (x > -999999)
      xx = x;
    if (y > -999999)
      yy = y;
    combinePosition(e);
    break;
  }
  case 301:
  case 303: {
    const float x = F(2), y = F(3);
    const int duration = I(0), mode = I(1);
    const bool rel = op == 303;
    (rel ? e.relativeCircle : e.absoluteCircle).enabled = false;
    int& ticks = rel ? e.relativePositionTicks : e.positionTicks;
    ticks = std::max(0, duration);
    (rel ? e.relativePositionTime : e.positionTime) = 0;
    (rel ? e.relativePositionMode : e.positionMode) = mode;
    (rel ? e.relativeFromX : e.fromX) = rel ? e.rx : e.ax;
    (rel ? e.relativeFromY : e.fromY) = rel ? e.ry : e.ay;
    (rel ? e.relativeToX : e.toX) = x <= -999999 ? (rel ? e.rx : e.ax) : x;
    (rel ? e.relativeToY : e.toY) = y <= -999999 ? (rel ? e.ry : e.ay) : y;
    break;
  }
  case 325:
  case 326: {
    const bool rel = op == 326;
    const int duration = I(0);
    const float from[3] = {rel ? e.rx : e.ax, rel ? e.ry : e.ay, 0};
    const float tangentA[3] = {F(1), F(2), 0}, targetX = F(3), targetY = F(4);
    const float target[3] = {targetX <= -999999 ? from[0] : targetX,
                             targetY <= -999999 ? from[1] : targetY, 0};
    const float tangentB[3] = {F(5), F(6), 0};
    (rel ? e.relativeCircle : e.absoluteCircle).enabled = false;
    (rel ? e.relativePositionTicks : e.positionTicks) = std::max(0, duration);
    (rel ? e.relativePositionTime : e.positionTime) = 0;
    (rel ? e.relativePositionMode : e.positionMode) = 8;
    (rel ? e.relativePositionHermite : e.positionHermite)
        .begin(from, target, 3, duration, 8, tangentA, tangentB);
    break;
  }
  case 304:
  case 306:
  case 328: {
    float angle = F(0);
    const float speed = F(1);
    const bool rel = op == 306;
    (rel ? e.relativeCircle : e.absoluteCircle).enabled = false;
    if (angle > -999999)
      (rel ? e.relativeAngle : e.angle) =
          normalize(e.mirror && op != 328 ? mirrorAngle(angle) : angle);
    if (speed > -999999)
      (rel ? e.relativeSpeed : e.speed) = speed;
    break;
  }
  case 305:
  case 307:
  case 329: {
    float angle = F(2);
    const float speed = F(3);
    const int duration = I(0), mode = I(1);
    const bool rel = op == 307;
    (rel ? e.relativeCircle : e.absoluteCircle).enabled = false;
    int& ticks = rel ? e.relativePolarTicks : e.polarTicks;
    ticks = std::max(0, duration);
    (rel ? e.relativePolarTime : e.polarTime) = 0;
    (rel ? e.relativePolarMode : e.polarMode) = mode;
    float& fromA = rel ? e.relativeFromA : e.fromA;
    float& toA = rel ? e.relativeToA : e.toA;
    float& fromS = rel ? e.relativeFromS : e.fromS;
    float& toS = rel ? e.relativeToS : e.toS;
    fromA = rel ? e.relativeAngle : e.angle;
    fromS = rel ? e.relativeSpeed : e.speed;
    toA = angle <= -999999 ? fromA : e.mirror && op != 329 ? mirrorAngle(angle) : angle;
    toS = speed <= -999999 ? fromS : speed;
    if (std::fabs(fromA - toA) >= pi) {
      if (toA <= fromA)
        toA += tau;
      else
        fromA += tau;
    }
    break;
  }
  case 308:
  case 310: {
    const bool rel = op == 310;
    auto& m = rel ? e.relativeCircle : e.absoluteCircle;
    if (!m.enabled) {
      m.centerX = rel ? e.rx : e.ax;
      m.centerY = rel ? e.ry : e.ay;
    }
    m.enabled = true;
    const float angle = F(0), omega = F(1), radius = F(2), radial = F(3);
    if (angle > -999999)
      (rel ? e.relativeAngle : e.angle) = normalize(angle);
    if (omega > -999999)
      (rel ? e.relativeSpeed : e.speed) = omega;
    if (radius > -999999)
      m.radius = radius;
    if (radial > -999999)
      m.radialSpeed = radial;
    circularPosition(e, rel);
    combinePosition(e);
    break;
  }
  case 309:
  case 311: {
    const bool rel = op == 311;
    auto& m = rel ? e.relativeCircle : e.absoluteCircle;
    m.ticks = std::max(0, I(0));
    m.time = 0;
    m.mode = I(1);
    m.fromOmega = rel ? e.relativeSpeed : e.speed;
    m.fromRadius = m.radius;
    m.fromRadial = m.radialSpeed;
    const float omega = F(2), radius = F(3), radial = F(4);
    m.toOmega = omega <= -999999 ? m.fromOmega : omega;
    m.toRadius = radius <= -999999 ? m.fromRadius : radius;
    m.toRadial = radial <= -999999 ? m.fromRadial : radial;
    break;
  }
  case 312:
  case 313: {
    const float randomAngle = random.signedUnit() * pi;
    float angle;
    if (e.x < e.clampX - e.clampWidth * .25f)
      angle = randomAngle / 3;
    else if (e.x > e.clampX + e.clampWidth * .25f)
      angle = normalize(randomAngle / 3 + pi);
    else if (e.x < px / 128.f)
      angle = randomAngle * .5f;
    else
      angle = normalize(randomAngle * .5f + pi);
    if (e.y < e.clampY - e.clampHeight * .25f)
      angle = std::fabs(angle);
    else if (e.y > e.clampY + e.clampHeight * .25f)
      angle = -std::fabs(angle);
    const int mode = I(1);
    const float speed = F(2);
    const int duration = I(0);
    const bool rel = op == 313;
    (rel ? e.relativePolarTicks : e.polarTicks) = std::max(0, duration);
    (rel ? e.relativePolarTime : e.polarTime) = 0;
    (rel ? e.relativePolarMode : e.polarMode) = mode;
    (rel ? e.relativeFromA : e.fromA) = (rel ? e.relativeToA : e.toA) = angle;
    (rel ? e.relativeFromS : e.fromS) = speed;
    (rel ? e.relativeToS : e.toS) = 0;
    break;
  }
  case 314:
  case 315: {
    if (auto* boss = primaryBoss()) {
      if (op == 314) {
        e.ax = boss->x;
        e.ay = boss->y;
      } else {
        e.rx = boss->x;
        e.ry = boss->y;
      }
      combinePosition(e);
    }
    break;
  }
  case 324:
    e.mirror = I(0) & 1;
    break;
  case 400:
    e.hitWidth = F(0);
    e.hitHeight = F(1);
    e.hit = std::max(e.hitWidth, e.hitHeight) * .5f;
    break;
  case 404:
    e.clampX = F(0);
    e.clampY = F(1);
    e.clampWidth = F(2);
    e.clampHeight = F(3);
    e.flags |= 0x10000u;
    break;
  case 402:
    e.flags |= u32(I(0));
    e.hidden = e.controller || bool(e.flags & 32);
    if (e.flags & 32u)
      for (int slot = 0; slot < 16; ++slot)
        animationScene.drawEnabled(enemyAnimationKey(e.id, slot), false);
    e.invincible = bool(e.flags & 17) || e.immunityTicks > 0;
    break;
  case 403:
    e.flags &= ~u32(I(0));
    e.hidden = e.controller || bool(e.flags & 32);
    if (!(e.flags & 32u))
      for (int slot = 0; slot < 16; ++slot)
        animationScene.drawEnabled(enemyAnimationKey(e.id, slot), true);
    e.invincible = bool(e.flags & 17) || e.immunityTicks > 0;
    break;
  case 401:
    e.bodyWidth = F(0);
    e.bodyHeight = F(1);
    break;
  case 405:
    e.flags &= ~0x10000u;
    break;
  case 406:
    e.dropCounts.fill(0);
    break;
  case 407: {
    const int type = I(0), count = I(1);
    if (type == 0)
      e.drop = count;
    else if (type >= 1 && type <= 19)
      e.dropCounts[type - 1] = count;
    else
      unsupported[op]++;
    break;
  }
  case 408:
    e.dropSpreadX = F(0);
    e.dropSpreadY = F(1);
    break;
  case 409:
    dropItems(e);
    break;
  case 410:
    e.drop = I(0);
    break;
  case 411:
    e.life = e.maxLife = I(0);
    e.lifeRaw = enemy_life::expanded(e.life);
    if (e.boss) {
      clearBossMarkers();
      e.flags |= 0x20000000u;
    }
    break;
  case 412: {
    const int slot = I(0);
    if (slot >= 0 && slot < 8) {
      for (auto& other : enemies)
        if (other.get() != &e && other->bossSlot == slot) {
          other->boss = false;
          other->bossSlot = -1;
          other->flags &= ~0x400000u;
        }
      e.boss = true;
      e.bossSlot = slot;
      e.flags |= 0x400000u;
    } else {
      e.boss = false;
      e.bossSlot = -1;
      e.flags &= ~0x400000u;
    }
    break;
  }
  case 413:
    e.phaseAge = e.age = 0;
    break;
  case 414: {
    const int timeout = I(2), health = I(1), slot = I(0);
    if (slot >= 0 && slot < 8) {
      auto& interrupt = e.interrupts[slot];
      interrupt.health = health;
      if (health >= 0) {
        interrupt.time = timeout;
        interrupt.healthScript = interrupt.timeoutScript = stringArg(c, 3);
      }
    }
    break;
  }
  case 415:
    e.immunityTicks = std::max(0, I(0));
    e.invincible = bool(e.flags & 17) || e.immunityTicks > 0;
    break;
  case 416:
    sound_system::queue.play(I(0), e.x);
    break;
  case 417:
    ++unsupported[417]; // Native4529a0 screen effect is not a sound alias.
    break;
  case 418: {
    if (messageVM.start(messageFiles[character * 2 + shot], I(0))) {
      dialogue = I(0) + 1;
      messageEvents.clear();
      startMessageAnimations();
      bossHud.checkpoint = I(0) + 1; // Native41fc68.
      for (auto* b : hostileSlots)
        if (b && b->active)
          eb::cancel(b->enemy, bulletWorld);
      laserManager.clearAll(laserWorld, false, false);
      requestEnemyClear();
    } else
      unsupported[418]++;
    break;
  }
  case 419:
    if (dialogue > 0) {
      c.time -= 1;
      c.pc -= rd<u16>(c.pc + 6);
    }
    break;
  case 420:
    if (primaryBoss()) {
      c.time -= 1;
      c.pc -= rd<u16>(c.pc + 6);
    }
    break;
  case 421: {
    const int slot = I(0);
    if (slot >= 0 && slot < 8)
      e.interrupts[slot].timeoutScript = stringArg(c, 1);
    break;
  }
  case 422:
  case 428:
  case 437:
  case 438:
  case 439: {
    spell_system::Start start;
    start.cardId = spell_system::resolveCardId(op, I(0), difficulty);
    start.timeLimit = I(1);
    (void)I(2);
    start.difficulty = difficulty;
    start.stage = stageNumber;
    start.bombActive = playerBomb.active;
    start.bossX = e.x;
    start.bossY = e.y;
    const int length = rd<i32>(c.pc + 28);
    if (length >= 0 && length <= int(rd<u16>(c.pc + 6)) - 32)
      start.title = spell_system::decryptTitle(c.pc + 32, length);
    if (!spellState.start(start))
      ++unsupported[op];
    else
      startSpellAnimations();
    spell = spellState.active();
    e.lifeFlags |= 1u; // Native418947..41895d, independent of capture eligibility.
    e.lifeRaw = enemy_life::expanded(e.life);
    break;
  }
  case 423:
    endSpellAnimations(spellState.end(score));
    backgroundVisible = true;
    stageState.flags |= 1u;
    spell = spellState.active();
    e.lifeFlags &= ~1u;
    break;
  case 424:
    bossHud.checkpoint = I(0); // Native41c550 writes global4b0cb8.
    break;
  case 425:
    if (op == 425)
      requestEnemyClear();
    break;
  case 426:
    break;
  case 427: {
    const float life = F(1);
    const uint32_t color = uint32_t(I(2));
    const int index = I(0);
    if (index >= 0 && index < 4 && e.maxLife > 0) {
      bossHud.markers[index] = float(double(life) / e.maxLife);
      bossHud.colors[index] = color;
    }
    break;
  }
  case 435: {
    int id = rd<i32>(c.pc + 16);
    storeInt(c, e, id, argInt(c, e, std::min(difficulty, 3) + 1));
    break;
  }
  case 436: {
    int id = int(rd<float>(c.pc + 16));
    store(c, e, id, arg(c, e, std::min(difficulty, 3) + 1, true), true);
    break;
  }
  case 434: {
    const int low = I(1), high = I(2), rank = std::clamp(resources.rank, -1024, 1024);
    storeInt(c, e, rd<i32>(c.pc + 16),
             i32(u32(low) + u32(i32(i32(u32(high) - u32(low)) * u32(rank + 1024)) / 2048)));
    break;
  }
  case 448:
    c.time -= I(std::min(difficulty, 3));
    break;
  case 442:
    spellState.setSurvival();
    break;
  case 443:
    spellState.markTimeoutRing();
    animationScene.remove(spellAnimationKeys[3]);
    spellAnimationKeys[3] = 0;
    break;
  case 440:
    bossHud.remaining = std::clamp(I(0), 0, 10);
    break;
  case 445:
  case 449:
    break;
  case 455: {
    const auto id = I(1);
    const bool found =
        id != 0 && std::any_of(enemies.begin(), enemies.end(), [id](const auto& enemy) {
          return enemy->active && enemy->id == u32(id);
        });
    storeInt(c, e, rd<i32>(c.pc + 16), found ? 1 : 0);
    break;
  }
  case 454:
    // Native421740 uses HUD+6ce4, loaded from StageInfo+30 by41d3b0.
    bindScreenAnimation(9, 0, 23);
    break;
  case 500:
    s = Shooter();
    break;
  case 501:
    emit(e, s);
    break;
  case 502:
    s.type = I(1);
    s.color = I(2);
    break;
  case 503:
    s.x = F(1);
    s.y = F(2);
    break;
  case 504:
    s.angle = F(1);
    s.spacing = F(2);
    break;
  case 505:
    s.speed = F(1);
    s.slow = F(2);
    break;
  case 506:
    s.count = I(1);
    s.layers = I(2);
    break;
  case 507:
    s.mode = I(1);
    break;
  case 511:
    e.shooters[sid] = e.shooters[std::clamp(I(1), 0, 15)];
    break;
  case 508:
    s.fireSound = I(1);
    s.extensions.transformSound = I(2);
    break;
  case 529:
    e.customUpdate = I(0);
    break;
  case 530:
    e.customDamage = I(0);
    break;
  case 531:
    e.customCollision = I(0);
    break;
  case 534:
    runEnemyCallback(e, I(0));
    break;
  case 509:
    if (!eb::set(s.extensions, I(1), I(2) != 0, u32(I(3)), I(4), I(5), F(6), F(7)))
      unsupported[op]++;
    break;
  case 510:
    for (auto& b : bullets)
      if (!b.friendly && b.active)
        eb::cancel(b.enemy, bulletWorld);
    laserManager.clearAll(laserWorld);
    break;
  case 514:
  case 517: {
    const int at = resources.rank < -512 ? 1 : resources.rank < 512 ? 3 : 5;
    if (op == 514) {
      s.speed = F(at);
      s.slow = F(at + 1);
    } else {
      s.count = int16_t(I(at));
      s.layers = int16_t(I(at + 1));
    }
    break;
  }
  case 515:
  case 518: {
    const int at = resources.rank < -600   ? 1
                   : resources.rank < -200 ? 3
                   : resources.rank < 200  ? 5
                   : resources.rank < 600  ? 7
                                           : 9;
    if (op == 515) {
      s.speed = F(at);
      s.slow = F(at + 1);
    } else {
      s.count = int16_t(I(at));
      s.layers = int16_t(I(at + 1));
    }
    break;
  }
  case 516: {
    const double t = double(resources.rank + 1024) / 2048;
    s.speed = float(double(F(1)) + (double(F(3)) - F(1)) * t);
    s.slow = float(double(F(2)) + (double(F(4)) - F(2)) * t);
    break;
  }
  case 519: {
    const auto interpolate = [&](i32 low, i32 high) {
      return int16_t(low + i32(i32(u32(high - low) * u32(resources.rank + 1024)) / 2048));
    };
    s.count = interpolate(I(1), I(3));
    s.layers = interpolate(I(2), I(4));
    break;
  }
  case 512:
  case 513: {
    const float radius = F(0);
    for (auto& b : bullets)
      if (!b.friendly && b.active &&
          (b.enemy.phase == eb::Phase::Alive || b.enemy.phase == eb::Phase::Spawning)) {
        const double dx = double(b.x) - e.x, dy = double(b.y) - e.y,
                     reach = double(radius) + eb::appearances[b.enemy.type].hitbox * .5;
        if (dx * dx + dy * dy <= reach * reach) {
          eb::cancel(b.enemy, bulletWorld);
          if (op == 512 && b.x >= -192 && b.x <= 192 && b.y >= 0 && b.y <= 448)
            itemManager.spawn(9, {b.x, b.y}, -pi / 2, .6000000238418579f);
        }
      }
    laserManager.clearCircle(laserWorld, {e.x, e.y, 0}, radius, op == 512, false);
    break;
  }
  case 520: {
    const float x = F(1), y = F(2);
    store(c, e, int(rd<float>(c.pc + 16)), aim(x, y), true);
    break;
  }
  case 521:
    s.speed = F(1 + std::min(difficulty, 3));
    s.slow = F(5 + std::min(difficulty, 3));
    break;
  case 522:
    s.count = I(1 + std::min(difficulty, 3));
    s.layers = I(5 + std::min(difficulty, 3));
    break;
  case 523: {
    const float angle = F(1), radius = F(2);
    s.x = float(std::cos(double(angle)) * radius);
    s.y = float(std::sin(double(angle)) * radius);
    break;
  }
  case 524:
    s.radius = F(1);
    break;
  case 525:
    s.originX = F(1);
    s.originY = F(2);
    s.fixedOrigin = s.originX >= -990;
    break;
  case 526:
    break;
  case 527:
  case 528:
    break;
  case 600:
    s.laserConfig.initialLength = F(1);
    s.laserConfig.maxLength = F(2);
    s.laserConfig.travelDistance = F(3);
    s.laserConfig.width = F(4);
    break;
  case 601:
    s.laserConfig.delay = I(1);
    s.laserConfig.grow = I(2);
    s.laserConfig.hold = I(3);
    s.laserConfig.shrink = I(4);
    s.laserConfig.flags = u32(I(5));
    break;
  case 602:
    spawnLaser(e, s, laser::Kind::Moving);
    break;
  case 603:
    spawnLaser(e, s, laser::Kind::Timed, I(1));
    break;
  case 611:
    spawnLaser(e, s, laser::Kind::Curve);
    break;
  case 604:
    if (auto* object = laserManager.find(I(0)))
      object->state.position = {F(1), F(2), 0};
    break;
  case 605:
    if (auto* object = laserManager.find(I(0))) {
      if (object->state.kind == laser::Kind::Timed)
        object->state.velocity = {F(1), F(2), 0};
      else
        ++unsupported[op];
    }
    break;
  case 606:
    if (auto* object = laserManager.find(I(0)))
      object->state.speed = F(1);
    break;
  case 607:
    if (auto* object = laserManager.find(I(0)))
      object->state.width = F(1);
    break;
  case 608:
    if (auto* object = laserManager.find(I(0)))
      object->state.angle = F(1);
    break;
  case 609:
    if (auto* object = laserManager.find(I(0))) {
      if (object->state.kind == laser::Kind::Timed)
        object->state.parameters.angularVelocity = F(1);
      else
        ++unsupported[op];
    }
    break;
  case 610:
    laserManager.clearId(laserWorld, I(0));
    break;
  case 700:
    break; // Original debug printf instruction has no game-state mutation.
  default:
    if (op >= 0 && op < 1024)
      unsupported[op]++;
    break;
  }
}
void runContext(Enemy& e, Context& c) {
  for (int budget = 0; c.pc && rd<i32>(c.pc) <= c.time && budget < 10000; budget++) {
    const u8* instruction = c.pc;
    const int op = rd<u16>(c.pc + 4), length = rd<u16>(c.pc + 6);
    if (length < 16) {
      unsupported[998]++;
      c.pc = nullptr;
      break;
    }
    if (c.pc[10] & (1 << difficulty))
      switch (op) {
      case 0:
        break;
      case 1:
        e.active = false;
        c.pc = nullptr;
        return;
      case 10:
        if (c.calls.empty()) {
          c.pc = nullptr;
          return;
        } else {
          auto f = c.calls.back();
          c.calls.pop_back();
          c.pc = f.pc;
          c.time = f.time;
          c.locals = f.locals;
          continue;
        }
      case 11: {
        auto locals = callArguments(c, e);
        auto sub = stringArg(c);
        c.calls.push_back({c.pc + length, c.time, c.locals});
        c.locals = locals;
        c.pc = program.find(sub);
        c.time = 0;
        continue;
      }
      case 12:
      case 13:
      case 14: {
        bool go = op == 12;
        if (op != 12) {
          const int v = int(c.pop());
          go = op == 13 ? v == 0 : v != 0;
        }
        if (go) {
          int delta = rd<i32>(c.pc + 16);
          c.time = float(rd<i32>(c.pc + 20));
          c.pc += delta;
          continue;
        }
        break;
      }
      case 15:
      case 16: {
        Context next(program.find(stringArg(c)));
        next.locals = callArguments(c, e, op == 16 ? 1 : 0);
        next.id = op == 16 ? int(arg(c, e, (rd<u32>(c.pc + 16) + 4) / 4)) : -1;
        e.contexts.insert(std::next(e.contexts.begin()), std::move(next));
        break;
      }
      case 17: {
        int id = int(arg(c, e, 0));
        for (auto& t : e.contexts)
          if (t.id == id)
            t.pc = nullptr;
        break;
      }
      case 21: {
        auto i = e.contexts.begin();
        if (i != e.contexts.end())
          ++i;
        for (; i != e.contexts.end(); ++i)
          i->pc = nullptr;
        break;
      }
      case 40:
      case 41:
        break;
      case 42:
        c.pushInt(argInt(c, e, 0));
        break;
      case 44:
        c.push(arg(c, e, 0, true));
        break;
      case 43:
        storeInt(c, e, rd<i32>(c.pc + 16), c.popInt());
        break;
      case 45: {
        int id = int(rd<float>(c.pc + 16));
        float v = c.pop();
        store(c, e, id, v, true);
        break;
      }
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
      case 65:
      case 66:
      case 67:
      case 68:
      case 69:
      case 70:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77: {
        const Value vb = c.popValue(), va = c.popValue();
        const float b = vb.floating(), a = va.floating();
        const i32 ib = vb.integral(), ia = va.integral();
        float v = 0;
        bool integer = true;
        i32 iv = 0;
        switch (op) {
        case 50:
          iv = i32(u32(ia) + u32(ib));
          break;
        case 51:
          v = float(double(a) + b);
          integer = false;
          break;
        case 52:
          iv = i32(u32(ia) - u32(ib));
          break;
        case 53:
          v = float(double(a) - b);
          integer = false;
          break;
        case 54:
          iv = i32(u32(ia) * u32(ib));
          break;
        case 55:
          v = float(double(a) * b);
          integer = false;
          break;
        case 56:
          iv = ib ? (ia == i32(0x80000000u) && ib == -1 ? ia : ia / ib) : 0;
          break;
        case 57:
          v = b ? float(double(a) / b) : 0;
          integer = false;
          break;
        case 58:
          iv = ib ? (ia == i32(0x80000000u) && ib == -1 ? 0 : ia % ib) : 0;
          break;
        case 59:
          iv = ia == ib;
          break;
        case 60:
          iv = a == b;
          break;
        case 61:
          iv = ia != ib;
          break;
        case 62:
          iv = a != b;
          break;
        case 63:
          iv = ia < ib;
          break;
        case 64:
          iv = a < b;
          break;
        case 65:
          iv = ia <= ib;
          break;
        case 66:
          iv = a <= b;
          break;
        case 67:
          iv = ia > ib;
          break;
        case 68:
          iv = a > b;
          break;
        case 69:
          iv = ia >= ib;
          break;
        case 70:
          iv = a >= b;
          break;
        case 73:
          iv = bool(ia) || bool(ib);
          break;
        case 74:
          iv = bool(ia) && bool(ib);
          break;
        case 75:
          iv = ia ^ ib;
          break;
        case 76:
          iv = ia | ib;
          break;
        case 77:
          iv = ia & ib;
          break;
        }
        if (integer)
          c.pushInt(iv);
        else
          c.push(v);
        break;
      }
      case 71:
        c.pushInt(!c.popInt());
        break;
      case 72:
        c.pushInt(!c.pop());
        break;
      case 78: {
        int id = rd<i32>(c.pc + 16);
        i32 v = argInt(c, e, 0);
        storeInt(c, e, id, i32(u32(v) - 1));
        c.pushInt(v);
        break;
      }
      case 79:
        c.push(float(std::sin(double(c.pop()))));
        break;
      case 80:
        c.push(float(std::cos(double(c.pop()))));
        break;
      case 81: {
        float angle = arg(c, e, 2, true), length = arg(c, e, 3, true);
        store(c, e, int(rd<float>(c.pc + 16)), float(std::cos(double(angle)) * length), true);
        store(c, e, int(rd<float>(c.pc + 20)), float(std::sin(double(angle)) * length), true);
        break;
      }
      case 82: {
        float v = arg(c, e, 0, true);
        while (v > pi)
          v -= tau;
        while (v < -pi)
          v += tau;
        store(c, e, int(rd<float>(c.pc + 16)), v, true);
        break;
      }
      case 83:
        c.time -= arg(c, e, 0);
        break;
      case 84:
        c.pushInt(i32(0u - u32(c.popInt())));
        break;
      // Retail op85 negates the converted float's raw word (not fchs).
      // Verified with the native VM: 3.5 becomes -1.25 in this title.
      case 85: {
        u32 bits = asbits(c.pop());
        c.stack.push_back({0u - bits, false});
        break;
      }
      case 86: {
        const double x = arg(c, e, 1, true), y = arg(c, e, 2, true);
        store(c, e, int(rd<float>(c.pc + 16)), float(x * x + y * y), true);
        break;
      }
      case 87: {
        const float dx = float(double(arg(c, e, 3, true)) - arg(c, e, 1, true)),
                    dy = float(double(arg(c, e, 4, true)) - arg(c, e, 2, true));
        store(c, e, int(rd<float>(c.pc + 16)), float(std::atan2(double(dy), double(dx))), true);
        break;
      }
      case 89: {
        const double a = arg(c, e, 1, true), b = arg(c, e, 2, true);
        store(c, e, int(rd<float>(c.pc + 16)), normalize(float(b - a)), true);
        break;
      }
      case 88:
        c.push(float(std::sqrt(double(c.pop()))));
        break;
      default:
        command(e, c, op);
        break;
      }
    if (c.pc == instruction)
      c.pc += length;
    else if (c.pc == instruction - length)
      c.pc = instruction;
  }
  if (c.pc)
    c.time += 1;
}

} // namespace th12
