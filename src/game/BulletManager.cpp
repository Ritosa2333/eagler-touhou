// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
struct BulletWorld : eb::World {
  void emit(const eb::Emission& event) override {
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
    th12::emit(origin, s);
  }
  void unsupported(uint32_t) override { ++th12::unsupported[509]; }
  void sound(int sound, float) override {
    if (sound >= 0)
      eventBits |= 1;
  }
  static uint64_t key(u32 id) { return 0x500000000ull + id; }
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
      return false;
    case eb::AnimationEvent::CancelEffect: {
      const u32 serial = nextDetachedAnimation++;
      animationScene.bind(0x100000000ull + serial, 0, 0, argument, anm_logic::Membership::Primary,
                          state.x, state.y);
      return false;
    }
    case eb::AnimationEvent::BirthTick:
    case eb::AnimationEvent::BulletTick:
      animationScene.position(root, state.x, state.y);
      if (auto* node = animationScene.find(root)) {
        auto& pose = node->vm.control();
        if (state.rotateToMotion)
          pose.rotation = state.angle + pi / 2;
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
} bulletBridge;
eb::World& bulletWorld = bulletBridge;
void emit(Enemy& e, Shooter& s) {
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
      if (bullets.size() > 16000)
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
      const float x = float(double(originX) + std::cos(double(a)) * s.radius),
                  y = float(double(originY) + std::sin(double(a)) * s.radius);
      Projectile projectile{x, y, normalize(a), speed, s.type == 11 ? 4.f : 3.f, s.type, s.color};
      projectile.entityId = nextProjectileId++;
      eb::initialize(projectile.enemy, projectile.entityId, s.extensions, s.type, s.color, x, y,
                     normalize(a), speed, bulletWorld);
      projectile.x = projectile.enemy.x;
      projectile.y = projectile.enemy.y;
      bullets.push_back(projectile);
    }
  eventBits |= 1;
}

void tickBullets() {
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  for (size_t i = 0; i < bullets.size(); i++) {
    auto& b = bullets[i];
    if (!b.active || b.friendly)
      continue;
    eb::tick(b.enemy, bulletWorld, true);
    b.x = b.enemy.x;
    b.y = b.enemy.y;
    b.angle = b.enemy.angle;
    b.speed = b.enemy.speed;
    b.age = b.enemy.age;
    b.active = b.enemy.phase != eb::Phase::Inactive;
    if (!(oracleFixtureFlags & 2) && playerState != 2 && b.active) {
      const auto collision =
          eb::collide(b.enemy, px / 128.f, py / 128.f, eb::playerHitboxes[character]);
      if (collision == eb::Collision::Graze && !b.enemy.grazed) {
        ++graze;
        b.enemy.grazed = true;
        eventBits |= 16;
      }
      if (collision == eb::Collision::Hit) {
        eb::cancel(b.enemy, bulletWorld);
        playerMissCollision();
      }
    }
    if (b.enemy.phase != eb::Phase::Inactive)
      eb::finishAnimation(b.enemy, bulletWorld);
    b.active = b.enemy.phase != eb::Phase::Inactive;
  }
}

} // namespace th12
