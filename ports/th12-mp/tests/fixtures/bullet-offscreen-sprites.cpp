#include "../../src/bullet.hpp"
#include "../../src/game/AnmLogic.hpp"
#include <array>
namespace {
using namespace th12;
anm_logic::Registry registry;
anm_logic::Runtime runtime;
anm_logic::VM machine;
eb::State bullet;
int tails, missing;
struct SpriteWorld : eb::World {
  void emit(const eb::Emission&) override {}
  void unsupported(uint32_t) override { ++missing; }
  const anm_logic::SpriteInfo* sprite() const {
    const auto& pose = machine.state();
    const auto bank = registry.bank(pose.spriteBank);
    return bank && pose.sprite >= 0 && pose.sprite < int(bank->sprites.size())
               ? &bank->sprites[size_t(pose.sprite)]
               : nullptr;
  }
  std::array<float, 2> spriteDimensions(const eb::State&) const override {
    if (const auto* info = sprite())
      return {info->width, info->height};
    return {0, 0};
  }
  bool spriteDimensionsAvailable(const eb::State&) const override { return sprite() != nullptr; }
  bool ownsStateAnimation(const eb::State&) const override { return true; }
  int animationInteger(const eb::State&, int index, int) override {
    return machine.state().integers[size_t(index)];
  }
  bool stateAnimation(eb::State&, eb::AnimationEvent event, int) override {
    if (event == eb::AnimationEvent::BulletTick) {
      ++tails;
      return machine.tick(runtime);
    }
    return false;
  }
} world;
float values[7];
}
extern "C" {
int bounds_query(float x, float y, float width, float height) {
  th12::eb::State state;
  state.x = x;
  state.y = y;
  return th12::eb::outside(state, {width, height});
}
int bounds_load(const uint8_t* bytes, int size) {
  std::string error;
  return registry.loadBank(0, bytes, size, error);
}
void bounds_begin(float x, float y, int type, int phase, int age) {
  machine = {};
  runtime = {};
  missing = tails = 0;
  runtime.unsupported = [](int, int, int, const char*) { ++missing; };
  machine.bind(registry, 0, 0, runtime);
  bullet = {};
  bullet.x = x;
  bullet.y = y;
  bullet.type = type;
  bullet.phase = th12::eb::Phase(phase);
  bullet.age = age;
  bullet.offscreenGrace = 0;
  bullet.cancelEffect = -1;
}
void bounds_tick() { th12::eb::tick(bullet, world); }
const float* bounds_snapshot() {
  const auto dimensions = world.spriteDimensions(bullet);
  values[0] = float(bullet.phase);
  values[1] = float(world.spriteDimensionsAvailable(bullet));
  values[2] = dimensions[0];
  values[3] = dimensions[1];
  values[4] = float(tails);
  values[5] = float(machine.state().clock);
  values[6] = float(missing);
  return values;
}
}
