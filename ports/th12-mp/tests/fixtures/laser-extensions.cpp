#include "../../src/game/Laser.hpp"
#include <array>
#include <unordered_map>
using namespace th12::laser;
namespace {
Manager manager;
struct ExtensionWorld final : World {
  std::unordered_map<uint32_t, std::array<int, 2>> blends;
  int missing = 0;
  void bindAnimation(Laser& object, Role role, const AnimationBinding&) override {
    blends[object.state.serialId][int(role)] = 1;
  }
  void blendControl(Laser& object, int blend) override {
    blends[object.state.serialId][0] = blend;
  }
  void unsupported(uint32_t) override { ++missing; }
} world;
std::array<float, 256 * 26 + 1> output;
}
extern "C" {
void extensions_begin(int kind, float x, float y, float angle, float length, float speed,
                      int c, int flags, float changedSpeed, int extension) {
  manager.reset();
  world.blends.clear();
  world.missing = 0;
  Parameters p;
  p.position = {x, y, 0};
  p.angle = angle;
  p.initialLength = p.maxLength = length;
  p.width = 8;
  p.speed = speed;
  p.type = kind == 2 ? 0 : 17;
  p.color = 4;
  p.delay = 8;
  p.grow = 3;
  p.hold = 30;
  p.shrink = 5;
  p.flags = kind == 1 ? 2 : 0;
  p.nodeCount = 8;
  p.program.records[0] = {changedSpeed, 0, c, flags, uint32_t(extension), false};
  manager.spawn(Kind(kind), p, world);
}
void extensions_tick() { manager.tick(world); }
int extensions_missing() { return world.missing; }
const float* extensions_snapshot() {
  const auto list = manager.ordered();
  output[0] = float(list.size());
  for (size_t i = 0; i < list.size(); ++i) {
    const auto& s = list[i]->state;
    auto* p = output.data() + 1 + i * 26;
    p[0] = float(s.lookupId);
    p[1] = float(int(s.kind));
    p[2] = float(int(s.phase));
    p[3] = float(s.age.current);
    p[4] = s.position.x;
    p[5] = s.position.y;
    p[6] = s.position.z;
    p[7] = s.velocity.x;
    p[8] = s.velocity.y;
    p[9] = s.velocity.z;
    p[10] = s.angle;
    p[11] = s.length;
    p[12] = s.width;
    p[13] = s.speed;
    p[14] = s.headOffset;
    p[15] = float(s.activeExtensions);
    p[16] = float(s.cursor);
    p[17] = s.mirrorSpeed;
    p[18] = float(s.mirrorRemaining);
    p[19] = float(s.mirrorFlags);
    p[20] = float(s.parameters.program.records[0].c);
    p[21] = float(world.blends[s.serialId][0]);
    p[22] = float(world.blends[s.serialId][1]);
    p[23] = s.initialPosition.x;
    p[24] = s.initialPosition.y;
    p[25] = s.initialPosition.z;
  }
  return output.data();
}
}
