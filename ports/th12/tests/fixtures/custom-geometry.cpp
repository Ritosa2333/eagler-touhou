#include "../../src/game/AnmLogic.hpp"
#include "../../src/game/CustomGeometry.hpp"
using namespace th12;
custom_geometry::Mesh mesh;
anm_logic::Registry registry;
float regionValues[6];
extern "C" {
int load_bank(const uint8_t* bytes, int length) {
  std::string error;
  return registry.loadBank(0, bytes, size_t(length), error);
}
const float* sprite_region(int sprite) {
  const auto& s = registry.bank(0)->sprites.at(size_t(sprite));
  const float values[6] = {s.width, s.height, s.u0, s.v0, s.u1, s.v1};
  std::copy(values, values + 6, regionValues);
  return regionValues;
}
int generate_geometry(int kind, const float* f, const uint32_t* w, const float* nodes, int count) {
  anm_logic::State s;
  custom_geometry::Position origin{f[0], f[1], f[2]};
  s.rx = f[3];
  s.ry = f[4];
  s.rotation = f[5];
  s.sx = f[6];
  s.sy = f[7];
  s.u = f[8];
  s.v = f[9];
  s.spriteWidth = f[12];
  s.spriteHeight = f[13];
  s.uvScaleX = f[10];
  s.uvScaleY = f[11];
  s.flags = w[0];
  s.primary = w[1];
  s.secondary = w[2];
  s.geometryCount = int(w[3]);
  s.integers[1] = int(w[4]);
  s.mode = int(w[5]);
  s.layer = int(w[6]);
  s.blend = int(w[7]);
  s.anchorX = int(w[8]);
  s.anchorY = int(w[9]);
  s.sprite = int(w[10]);
  s.spriteBank = int(w[11]);
  custom_geometry::SpriteRegion region{int(w[11]), int(w[10]), int(w[12]), f[28], f[29],
                                       f[14],      f[15],      f[16],      f[19]};
  laser::State l;
  l.position = {f[22], f[23], f[24]};
  l.angle = f[25];
  l.parameters.width = f[26];
  l.headOffset = f[27];
  l.age.current = int(w[13]);
  for (int i = 0; i < count; i++)
    l.nodes.push_back(
        {{nodes[i * 5], nodes[i * 5 + 1], nodes[i * 5 + 2]}, nodes[i * 5 + 3], nodes[i * 5 + 4]});
  if (kind == 0)
    mesh = custom_geometry::ufoFillRing(s, region, origin);
  else if (kind == 1)
    mesh = custom_geometry::flatQuad(s, region, origin, s.rotation, s.sx, s.sy);
  else if (kind == 2)
    mesh = custom_geometry::curveLaser(l, s, region);
  else if (kind == 4)
    mesh = custom_geometry::spellRing(s, region, origin);
  else
    mesh = custom_geometry::straightLaser(l, s, region);
  return int(mesh.vertices.size());
}
const custom_geometry::Vertex* vertices() {
  return mesh.vertices.data();
}
int metadata(int field) {
  return field == 0   ? int(mesh.primitive)
         : field == 1 ? mesh.bank
         : field == 2 ? mesh.sprite
         : field == 3 ? mesh.textureEntry
         : field == 4 ? mesh.layer
                      : mesh.blend;
}
}
