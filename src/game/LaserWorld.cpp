// Bridge the recovered native laser objects to the shared RNG, ANM and item world.
#include "LaserWorld.hpp"
#include "GameState.hpp"
#include "LaserCollision.hpp"

namespace th12 {
laser::Manager laserManager;
uint64_t laserAnimationKey(u32 serial, laser::Role role) {
  return 0xe00000000ull + uint64_t(serial) * 2 + uint32_t(role);
}

class GameLaserWorld final : public laser::World {
public:
  laser::Vec3 playerPosition() const override { return {px / 128.f, py / 128.f, 0}; }
  bool bossPosition(laser::Vec3& p) const override {
    if (auto* boss = primaryBoss()) {
      p = {boss->x, boss->y, 0};
      return true;
    }
    return false;
  }
  laser::Collision lineCollision(const laser::Vec3& p, float angle, float halfWidth,
                                 float length) override;
  void graze(const laser::Vec3& position) override {
    awardPlayerGraze(position.x, position.y, position.z);
  }
  void bindAnimation(laser::Laser& object, laser::Role role,
                     const laser::AnimationBinding& b) override {
    const auto key = laserAnimationKey(object.state.serialId, role);
    std::function<int(int)> remap;
    if (b.typeColorAppearance)
      remap = [type = b.type, color = b.color](int sprite) {
        int resolved = 0;
        if (bullet_visuals::resolve(type, color, sprite, resolved))
          return resolved;
        ++th12::unsupported[984];
        return -1;
      };
    if (auto* node = animationScene.bind(key, 0xb0000000u + object.state.serialId, 0, b.script,
                                         anm_logic::Membership::Embedded, 0, 0, remap, false,
                                         nullptr, 224, 16))
      node->customGeometry = true;
  }
  void finishAnimationBirth(laser::Laser& object, laser::Role role) override {
    if (object.state.kind == laser::Kind::Curve)
      return;
    if (auto* node = animationScene.find(laserAnimationKey(object.state.serialId, role))) {
      auto& state = node->vm.control();
      state.blend = 1;
      state.mode = 1;
      if (role == laser::Role::Main) {
        state.anchorX = 0;
        state.anchorY = 2;
      }
    }
  }
  void interruptAnimation(laser::Laser& object, laser::Role role, int code) override {
    animationScene.interrupt(laserAnimationKey(object.state.serialId, role), code);
  }
  void updateAnimationGeometry(laser::Laser& object, laser::Role role,
                               const laser::AnimationGeometry& g) override {
    auto p = g.position;
    if (role == laser::Role::Cap) {
      p.x = float(double(p.x) + std::cos(double(g.angle)) * g.headOffset);
      p.y = float(double(p.y) + std::sin(double(g.angle)) * g.headOffset);
    }
    animationScene.position(laserAnimationKey(object.state.serialId, role), p.x, p.y);
    if (auto* node = animationScene.find(laserAnimationKey(object.state.serialId, role))) {
      node->vm.control().engineZ = p.z;
      if (role == laser::Role::Main)
        node->vm.control().rotation = g.angle + pi / 2;
      if (role == laser::Role::Main && object.state.kind != laser::Kind::Curve) {
        auto& state = node->vm.control();
        const auto bank = animationScene.registry.bank(state.spriteBank);
        if (bank && state.sprite >= 0 && state.sprite < int(bank->sprites.size())) {
          const auto& sprite = bank->sprites[state.sprite];
          state.sx = g.width / sprite.width;
          state.sy = g.length / sprite.height;
          state.flags |= 8u;
        }
      }
    }
  }
  bool tickAnimation(laser::Laser& object, laser::Role role) override {
    const auto key = laserAnimationKey(object.state.serialId, role);
    animationScene.tickEmbedded(key);
    const auto* node = animationScene.find(key);
    return !node || node->vm.state().ended;
  }
  void retireAnimation(laser::Laser& object, laser::Role role) override {
    animationScene.remove(laserAnimationKey(object.state.serialId, role));
  }
  void sound(int id, float x) override {
    sound_system::queue.play(id, x);
    if (id >= 0)
      eventBits |= 1;
  }
  // Recovered callback3/5 owns the emitter's single sound call after allocation.
  void emitBullet(const eb::Emission& emission) override { emitHostileEmission(emission, false); }
  uint32_t next32() override { return random.next32(); }
  float unit() override { return random.unit(); }
  void spawnCancelEffect(const laser::Vec3& p, int script, float angle, float) override {
    Enemy origin;
    origin.x = p.x;
    origin.y = p.y;
    const auto serial = nextDetachedAnimation;
    detachedAnimation(0, script, origin, false);
    if (auto* node = animationScene.find(0x100000000ull + serial))
      node->vm.control().rotation = angle;
  }
  void pointItem(const laser::Vec3& p, int type, float angle, float speed) override {
    itemManager.spawn(type, {p.x, p.y}, angle, speed);
  }
  void unsupported(uint32_t code) override {
    ++th12::unsupported[985];
    recordRuntimeFault(1, code);
  }
};
GameLaserWorld gameLaserWorld;
laser::World& laserWorld = gameLaserWorld;
laser::Collision GameLaserWorld::lineCollision(const laser::Vec3& p, float angle, float halfWidth,
                                               float length) {
  if (oracleFixtureFlags & 2)
    return laser::Collision::None;
  laser::PlayerCollision player;
  player.position = playerPosition();
  player.hitWidth = player.hitHeight = eb::playerHitboxes[character] * .5f;
  player.state = playerState;
  player.invulnerability = invuln;
  player.dialogue = dialogue > 0;
  player.flags = playerFlags;
  const auto result = laser::collidePlayerLine(p, angle, halfWidth, length, player);
  if (result.beginMiss)
    playerMissCollision();
  return result.code;
}

int spawnLaser(Enemy& e, Shooter& shooter, laser::Kind kind, int lookupId) {
  laser::FactoryInput input;
  input.config = shooter.laserConfig;
  input.offset = {shooter.x, shooter.y, 0};
  input.origin = {shooter.originX, shooter.originY, 0};
  input.fixedOrigin = shooter.fixedOrigin;
  input.angle = shooter.angle;
  input.speed = shooter.speed;
  input.initialOffset = shooter.radius;
  input.type = shooter.type;
  input.color = shooter.color;
  input.sound = shooter.fireSound;
  input.transformSound = shooter.extensions.transformSound;
  input.program = shooter.extensions;
  const auto p = laser::makeParameters(kind, input, {e.x, e.y, 0}, lookupId);
  return laserManager.spawn(kind, p, laserWorld);
}
void tickLasers() {
  laserManager.tick(laserWorld);
}
void clearBombLasers() {
  for (const auto& a : playerBomb.rectangles)
    if (a.enabled)
      laserManager.clearRectangle(laserWorld, {a.x, a.y, 0}, {a.width, a.height, 0}, !spell, true);
  for (const auto& a : playerBomb.circles)
    laserManager.clearCircle(laserWorld, {a.x, a.y, 0}, a.radius, !spell, true);
}
} // namespace th12
