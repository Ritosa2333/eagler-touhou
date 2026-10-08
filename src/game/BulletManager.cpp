// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "BulletCancellation.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"

namespace th12 {
void emitHostileEmission(const eb::Emission& event, bool audible) {
  Enemy origin;
  origin.x = event.x;
  origin.y = event.y;
  Shooter s;
  s.angle = event.angle;
  s.spacing = event.spacing;
  s.speed = event.speed;
  s.slow = event.slow;
  s.type = event.type;
  s.color = event.color;
  s.count = event.count;
  s.layers = event.layers;
  s.mode = event.mode;
  s.extensions = event.program;
  th12::emit(origin, s, audible);
}
struct BulletWorld : eb::World {
  void emit(const eb::Emission& event) override { emitHostileEmission(event); }
  void unsupported(uint32_t code) override {
    ++th12::unsupported[509];
    recordRuntimeFault(0, code);
  }
  void sound(int sound, float x) override {
    sound_system::queue.play(sound, x);
    if (sound >= 0)
      eventBits |= 1;
  }
  static uint64_t key(u32 id) { return 0x500000000ull + id; }
  static const anm_logic::SpriteInfo* currentSprite(const eb::State& state) {
    if (auto* node = animationScene.find(key(state.id))) {
      const auto& pose = node->vm.state();
      if (const auto bank = animationScene.registry.bank(pose.spriteBank);
          bank && pose.sprite >= 0 && pose.sprite < int(bank->sprites.size())) {
        return &bank->sprites[pose.sprite];
      }
    }
    return nullptr;
  }
  std::array<float, 2> spriteDimensions(const eb::State& state) const override {
    if (const auto* sprite = currentSprite(state))
      return {sprite->width, sprite->height};
    return {0, 0};
  }
  bool spriteDimensionsAvailable(const eb::State& state) const override {
    return currentSprite(state) != nullptr;
  }
  void blendControl(eb::State& state, int blend) override {
    if (auto* node = animationScene.find(key(state.id))) {
      auto& pose = node->vm.control();
      pose.flags = (pose.flags & ~0xe0u) | (u32(blend & 1) << 5);
      pose.blend = blend & 1;
    }
  }
  void appearance(eb::State& state, bool initial) override {
    const u32 id = state.id;
    const auto remap = [id](int sprite) {
      for (const auto& b : bullets)
        if (!b.friendly && b.entityId == id) {
          int result = 0;
          if (bullet_visuals::resolve(b.enemy.type, b.enemy.color, sprite, result))
            return result;
          ++th12::unsupported[984];
          return -1;
        }
      return sprite;
    };
    // During synchronous birth the projectile is not yet in its manager. The
    // binding callback therefore captures the resolved birth appearance.
    const int type = state.type, color = state.color;
    const auto birthRemap = [type, color](int sprite) {
      int result = 0;
      if (bullet_visuals::resolve(type, color, sprite, result))
        return result;
      ++th12::unsupported[984];
      return -1;
    };
    if (initial)
      animationScene.bind(key(id), 0x10000000u + id, 0, state.animation,
                          anm_logic::Membership::Embedded, state.x, state.y, birthRemap, false,
                          nullptr, 224, 16, 0, true);
    else if (auto* node = animationScene.find(key(id))) {
      node->spriteRemap = birthRemap;
      animationScene.rebind(key(id), 0, state.animation);
    }
    (void)remap;
  }
  bool stateAnimation(eb::State& state, eb::AnimationEvent event, int argument) override {
    const auto root = key(state.id);
    switch (event) {
    case eb::AnimationEvent::Interrupt:
      animationScene.interrupt(root, argument);
      return false;
    case eb::AnimationEvent::Retire:
      animationScene.remove(root);
      releaseHostileBullet(state.id);
      return false;
    case eb::AnimationEvent::CancelEffect: {
      const u32 serial = nextDetachedAnimation++;
      animationScene.bind(0x100000000ull + serial, 0, 0, argument, anm_logic::Membership::Primary,
                          state.x, state.y, {}, false, nullptr, 224, 16, 23);
      return false;
    }
    case eb::AnimationEvent::BirthTick:
    case eb::AnimationEvent::BulletTick:
      animationScene.position(root, state.x, state.y);
      if (auto* node = animationScene.find(root)) {
        animationScene.tickEmbedded(root);
        return node->vm.state().ended;
      }
    }
    return false;
  }
  int animationInteger(const eb::State& state, int index, int fallback) override {
    if (auto* node = animationScene.find(key(state.id)); node && index >= 0 && index < 4)
      return node->vm.state().integers[index];
    return fallback;
  }
  bool ownsStateAnimation(const eb::State& state) const override {
    return const_cast<anm_logic::Scene&>(animationScene).find(key(state.id)) != nullptr;
  }
  eb::Collision playerCollision(eb::State& state) override {
    if (oracleFixtureFlags & 2)
      return eb::Collision::None;
    const auto result = eb::collide(state, px / 128.f, py / 128.f, eb::playerHitboxes[character]);
    // Native437810's outer graze branch precedes every state/dialogue gate.
    if (result == eb::Collision::Graze && !state.grazed) {
      awardPlayerGraze(px / 128.f, py / 128.f);
      state.grazed = true;
    }
    if (result != eb::Collision::Hit)
      return result;
    if (dialogue || playerState == 2 || playerState == 3 || playerState == 4 || (playerFlags & 2u))
      return eb::Collision::None;
    // Invulnerability suppresses438370, while437810 still returnsHit and
    // 4099e0 consumes the projectile. The miss effects precede its IRQ1.
    playerMissCollision();
    return eb::Collision::Hit;
  }
} bulletBridge;
eb::World& bulletWorld = bulletBridge;
void emit(Enemy& e, Shooter& s, bool audible) {
  const int count = std::clamp(s.count, 0, 120), layers = std::clamp(s.layers, 0, 20);
  const float originX = float(double(s.fixedOrigin ? s.originX : e.x) + s.x),
              originY = float(double(s.fixedOrigin ? s.originY : e.y) + s.y),
              aimed = aim(originX, originY);
  const auto between = [](float high, float low) {
    const float delta = float(double(high) - low), unit = random.unit(),
                product = float(double(unit) * delta);
    return float(double(low) + product);
  };
  for (int layer = 0; layer < layers; layer++)
    for (int j = 0; j < count; j++) {
      const int physicalSlot = hostilePool.reserve();
      if (physicalSlot < 0)
        return;
      float a = 0,
            speed = layers > 1
                        ? float(double(s.speed) - (double(s.speed) - s.slow) * layer / layers)
                        : s.speed;
      switch (s.mode) {
      case 0:
      case 1: {
        a = (count & 1) ? float(double((j + 1) / 2) * s.spacing)
                        : float(double(j / 2) * s.spacing + double(s.spacing) * .5);
        if (j & 1)
          a = -a;
        if (s.mode == 0)
          a = float(double(a) + aimed);
        a = float(double(s.angle) + a);
        break;
      }
      case 2:
      case 3: {
        const float center = s.mode == 2 ? aimed : 0,
                    ring = float(double(j) * tau / count + center);
        a = float(double(s.spacing) * layer + s.angle + ring);
        break;
      }
      case 4:
      case 5: {
        const float center = s.mode == 4 ? aimed : 0, half = float(double(pi) / count + center),
                    ring = float(double(j) * tau / count + half);
        a = float(double(s.spacing) * layer + s.angle + ring);
        break;
      }
      case 6:
        a = between(s.angle, s.spacing);
        break;
      case 7: {
        speed = between(s.speed, s.slow);
        const float ring = float(double(j) * tau / count);
        a = float(double(s.spacing) * layer + s.angle + ring);
        break;
      }
      case 8:
        a = between(s.angle, s.spacing);
        speed = between(s.speed, s.slow);
        break;
      default:
        unsupported[507]++;
        break;
      }
      // Native40a671 uses the normalized heading for radius placement, then
      // stores40d640's polar offsets before adding the shooter origin. The
      // subsequent velocity still uses the raw emitter angle in initialize.
      const float radiusAngle = normalize(a),
                  offsetX = float(std::cos(double(radiusAngle)) * s.radius),
                  offsetY = float(std::sin(double(radiusAngle)) * s.radius),
                  x = float(double(originX) + offsetX), y = float(double(originY) + offsetY);
      Projectile projectile{x, y, normalize(a), speed, s.type == 11 ? 4.f : 3.f, s.type, s.color};
      projectile.entityId = nextProjectileId++;
      projectile.physicalSlot = physicalSlot;
      bullets.push_back(projectile);
      auto& born = bullets.back();
      attachHostileSlot(born); // Parent occupies its slot before child births.
      eb::initialize(born.enemy, born.entityId, s.extensions, s.type, s.color, x, y, a, speed,
                     bulletWorld);
      born.x = born.enemy.x;
      born.y = born.enemy.y;
      born.active = born.enemy.phase != eb::Phase::Inactive;
      hostilePool.commit(physicalSlot); // After synchronous ANM/child births.
    }
  eventBits |= 1;
  if (audible && (s.extensions.flags & 0x80u))
    sound_system::queue.play(s.fireSound, originX);
}

namespace {
void cancelToPoint(Projectile& bullet, bool convert) {
  const float x = bullet.enemy.x, y = bullet.enemy.y;
  eb::cancel(bullet.enemy, bulletWorld);
  if (convert && bullet_cancellation::pointVisible(x, y))
    itemManager.spawn(9, {x, y}, -pi / 2, .6000000238418579f);
}
bool eligibleCancellation(const Projectile* bullet) {
  return bullet && bullet->active &&
         (bullet->enemy.phase == eb::Phase::Alive || bullet->enemy.phase == eb::Phase::Spawning);
}
bool fullClearTint(const Projectile& bullet, u32& color) {
  const auto* node = animationScene.find(BulletWorld::key(bullet.enemy.id));
  if (!node)
    return false;
  const auto& pose = node->vm.state();
  const auto bank = animationScene.registry.bank(pose.spriteBank);
  if (!bank || pose.sprite < 0 || pose.sprite >= int(bank->sprites.size()))
    return false;
  const float width = bank->sprites[pose.sprite].width;
  // Native40d1ad..40d1f4 reads the resolved sprite record width, independent
  // of scale/size overrides. Tables are4b0bd0,4b0c10,4b0c30.
  static constexpr u32 small[16] = {0xff808080, 0xffff1010, 0xffff1010, 0xff801080,
                                    0xff801080, 0xff1010ff, 0xff1010ff, 0xff108080,
                                    0xff108080, 0xff10ff10, 0xff10ff10, 0xff10ff10,
                                    0xff808010, 0xff808010, 0xff808010, 0xff808080};
  static constexpr u32 medium[8] = {0xff808080, 0xffff1010, 0xff801080, 0xff1010ff,
                                    0xff108080, 0xff10ff10, 0xff808010, 0xff808080};
  static constexpr u32 large[4] = {0xffff1010, 0xff1010ff, 0xff10ff10, 0xff808010};
  const int index = bullet.enemy.color;
  if (index < 0)
    return false;
  if (width <= 16 && index < 16)
    color = small[index];
  else if (width > 16 && width <= 32 && index < 8)
    color = medium[index];
  else if (width > 32 && index < 4)
    color = large[index];
  else
    return false;
  return true;
}
} // namespace
void cancelHostileBullets() {
  for (auto* bullet : hostileSlots)
    if (eligibleCancellation(bullet))
      cancelToPoint(*bullet, false);
}
void clearHostileViewport(bool convert) {
  if (spellState.active() && spellState.cardId >= 96 && spellState.cardId <= 99)
    return;
  for (auto* bullet : hostileSlots) {
    if (!eligibleCancellation(bullet))
      continue;
    const float hitbox = eb::appearances[bullet->enemy.type].hitbox;
    if (!bullet_cancellation::viewportOverlaps(bullet->enemy.x, bullet->enemy.y, hitbox, hitbox))
      continue;
    // Full clear marks flags8 directly. It preserves phase, lifetime and IRQ
    // until the next Bullet21 visit, which retires without movement/ANM tick.
    bullet->enemy.pendingRetire = true;
    if (convert)
      itemManager.spawn(9, {bullet->enemy.x, bullet->enemy.y}, -pi / 2, .6000000238418579f);
    u32 tint = 0;
    const bool tinted = fullClearTint(*bullet, tint);
    float z = 0;
    if (const auto* body = animationScene.find(BulletWorld::key(bullet->enemy.id)))
      z = body->vm.state().engineZ;
    const u32 serial = nextDetachedAnimation++;
    if (auto* effect = animationScene.bind(
            0x100000000ull + serial, 0, 0, 96, anm_logic::Membership::Primary, bullet->enemy.x,
            bullet->enemy.y, {}, false, nullptr, 224, 16, 23, false, z);
        effect && tinted)
      effect->vm.control().primary = tint;
  }
}
void cancelHostileCircle(float x, float y, float radius, bool convert, bool respectCountdown) {
  // Native40caa0 visits physical slots even when a low free slot was reused
  // after an older projectile; item9's delay ordinal follows these visits.
  if (spellState.active() && spellState.cardId >= 96 && spellState.cardId <= 99)
    radius = float(double(radius) / 3);
  for (auto* bullet : hostileSlots)
    if (eligibleCancellation(bullet) && (!respectCountdown || bullet->enemy.collisionDelay == 0) &&
        bullet_cancellation::circleContains(bullet->enemy.x, bullet->enemy.y, x, y, radius,
                                            eb::appearances[bullet->enemy.type].hitbox))
      cancelToPoint(*bullet, convert);
}
void cancelHostileRectangle(float x, float y, float width, float height, bool convert) {
  for (auto* bullet : hostileSlots)
    if (eligibleCancellation(bullet) && bullet->enemy.collisionDelay == 0 &&
        bullet_cancellation::rectangleContains(bullet->enemy.x, bullet->enemy.y, x, y, width,
                                               height))
      cancelToPoint(*bullet, convert);
}

void tickBullets() {
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  for (size_t slot = 0; slot < hostileSlots.size(); ++slot) {
    auto* pointer = hostileSlots[slot];
    if (!pointer || !pointer->active)
      continue;
    auto& b = *pointer;
    eb::tick(b.enemy, bulletWorld, true);
    b.x = b.enemy.x;
    b.y = b.enemy.y;
    b.angle = b.enemy.angle;
    b.speed = b.enemy.speed;
    b.age = b.enemy.age;
    b.active = b.enemy.phase != eb::Phase::Inactive;
    if (b.enemy.phase != eb::Phase::Inactive)
      eb::finishAnimation(b.enemy, bulletWorld);
    b.active = b.enemy.phase != eb::Phase::Inactive;
  }
}

} // namespace th12
