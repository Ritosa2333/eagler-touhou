#include "../../src/game/CustomGeometry.hpp"
using namespace th12;
pw::Leaf leaf;
anm_logic::State state;
custom_geometry::Mesh mesh;
struct Random {
  uint16_t seed = 0;
  uint32_t calls = 0;
  uint32_t next32() {
    const uint16_t a = uint16_t((seed ^ 0x9630) - 0x6553);
    const uint16_t b = uint16_t((a << 2) | (a >> 14));
    const uint16_t c = uint16_t((b ^ 0x9630) - 0x6553);
    seed = uint16_t((c << 2) | (c >> 14));
    calls += 2;
    return uint32_t(a) << 16 | c;
  }
} randomSource;
extern "C" {
void begin(int seed, float x, float y) {
  randomSource = {uint16_t(seed), 0};
  state = {};
  state.flags = 8198;
  state.primary = 0x00ffffffu;
  state.layer = 13;
  leaf.begin(1, x, y, randomSource);
}
void tick() {
  leaf.tick(randomSource);
}
int draw() {
  mesh = custom_geometry::sanaeBLeaf(leaf, state);
  return int(mesh.vertices.size());
}
const custom_geometry::Vertex* vertices() {
  return mesh.vertices.data();
}
int metadata(int field) {
  return field == 0   ? int(mesh.primitive)
         : field == 1 ? mesh.bank
         : field == 2 ? mesh.layer
         : field == 3 ? mesh.blend
                      : mesh.drawPriority;
}
uint32_t seed() {
  return randomSource.seed;
}
uint32_t calls() {
  return randomSource.calls;
}
float heading() {
  return leaf.heading;
}
}
