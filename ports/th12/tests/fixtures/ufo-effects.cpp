#include "../../src/game/UfoEffects.hpp"
using namespace th12;
anm_logic::Scene scene;
uint16_t seed;
uint32_t rngCalls;
float values[64];
uint32_t words[24];
std::vector<const anm_logic::Node*> effects() {
  std::vector<const anm_logic::Node*> result;
  for (auto* node : scene.chain(anm_logic::Membership::Primary))
    if (ufoEffects.owns(node->id))
      result.push_back(node);
  return result;
}
void prepare(int initialSeed) {
  scene.reset();
  ufoEffects.reset();
  seed = uint16_t(initialSeed);
  rngCalls = 0;
  scene.runtime.delta = 1;
  scene.runtime.gameRandom32 = [] {
    const uint16_t a = uint16_t((seed ^ 0x9630) - 0x6553);
    const uint16_t b = uint16_t((a << 2) | (a >> 14));
    const uint16_t c = uint16_t((b ^ 0x9630) - 0x6553);
    seed = uint16_t((c << 2) | (c >> 14));
    rngCalls += 2;
    return uint32_t(a) << 16 | c;
  };
}
extern "C" {
int load(int bank, const uint8_t* bytes, int length) {
  std::string error;
  return scene.registry.loadBank(bank, bytes, length, error);
}
void begin(int initialSeed, float x, float y, float z, int following) {
  prepare(initialSeed);
  ufoEffects.summon(scene, x, y, z);
  if (following)
    scene.bind(0x7a00000000000001ull, 0, 0, 0, anm_logic::Membership::Primary,
                0, 0, {}, false, nullptr, 0, 0, 23);
}
void defeat_begin(int initialSeed, float x, float y, float z) {
  prepare(initialSeed);
  scene.bind(0x7a00000000000001ull, 0, 0, 208, anm_logic::Membership::Primary,
              x, y, {}, false, nullptr, 224, 16, 23, false, z);
  ufoEffects.defeat(scene, x, y, z);
}
void tick() {
  scene.tickPrimary();
  scene.collect();
}
uint32_t rng_seed() { return seed; }
uint32_t rng_calls() { return rngCalls; }
int count() { return int(effects().size()); }
const float* effect_values(int index) {
  const auto* node = effects().at(index);
  const auto& e = *ufoEffects.state(node->id);
  values[0] = e.position[0];values[1] = e.position[1];values[2] = e.position[2];
  values[3] = e.timer.previous;values[4] = e.timer.current;values[5] = e.timer.value;
  values[6] = e.radius;values[7] = e.heading;
  for (int i = 0; i < 16; ++i) {
    values[8 + i * 2] = e.points[i][0];values[9 + i * 2] = e.points[i][1];
  }
  const auto& vm = node->vm.state();
  values[40] = vm.engineX;values[41] = vm.engineY;values[42] = vm.engineZ;
  values[43] = vm.clock;values[44] = vm.ticks;
  return values;
}
const uint32_t* effect_words(int index) {
  const auto* node = effects().at(index);
  const auto& e = *ufoEffects.state(node->id);
  words[0] = uint32_t(e.kind);words[1] = e.nodeId;
  words[2] = e.centerColor;words[3] = e.rimColor;
  std::copy(e.colors.begin(), e.colors.end(), words + 4);
  words[20] = node->vm.state().layer;words[21] = node->vm.state().flags;
  return words;
}
custom_geometry::Mesh mesh;
const custom_geometry::Vertex* draw(int index) {
  mesh = ufoEffects.draw(*effects().at(index));
  return mesh.vertices.data();
}
uint32_t mesh_count() { return uint32_t(mesh.vertices.size()); }
uint32_t mesh_primitive() { return uint32_t(mesh.primitive); }
int mesh_bank() { return mesh.bank; }
int mesh_priority() { return mesh.drawPriority; }
}
