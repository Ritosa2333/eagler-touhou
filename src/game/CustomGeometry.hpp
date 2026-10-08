// TH12 1.00b CPU geometry, separated from logic and GPU submission.
#pragma once
#include "AnmTypes.hpp"
#include "Laser.hpp"

namespace th12::custom_geometry {
struct Position {
  float x = 0, y = 0, z = 0;
};
struct SpriteRegion {
  int bank = -1, sprite = -1, textureEntry = 0;
  float width = 0, height = 0, u0 = 0, v0 = 0, u1 = 1, v1 = 1;
};
// Native FVF 0x144, sizeof 28: D3DFVF_XYZRHW | DIFFUSE | TEX1.
struct Vertex {
  float x = 0, y = 0, z = 0, rhw = 1;
  uint32_t argb = 0xffffffffu;
  float u = 0, v = 0;
};
static_assert(sizeof(Vertex) == 28);
enum class Primitive : uint32_t { LineStrip = 3, TriangleList = 4, TriangleStrip = 5 };
enum class DrawPass : uint32_t { AnmLayer = 0, LaserManager = 1, AsciiOverlay = 2 };
enum class Filter : uint32_t { Point = 1, Linear = 2 };
struct Mesh {
  Primitive primitive = Primitive::TriangleStrip;
  DrawPass pass = DrawPass::AnmLayer;
  int drawPriority = 0; // LaserManager's native scheduler draw priority is 29.
  Filter filter = Filter::Linear;
  bool wrapU = true, wrapV = true;
  int bank = -1, sprite = -1, textureEntry = 0, layer = 0, blend = 0;
  uint32_t owner = 0, nodeId = 0;
  std::vector<Vertex> vertices;
};
SpriteRegion spriteRegion(const anm_logic::State&, const anm_logic::SpriteInfo&);

// `origin` is the final native script+engine+offset+immediate-parent position,
// including the host viewport once. Mode13 does not inherit parent scale or
// rotation. Neither function updates a VM, timer, RNG, interpolation or Scene.
Mesh ufoFillRing(const anm_logic::State&, const SpriteRegion&, Position origin);
// 4581a3..458427 mode9: count-1 pairs around a full ring, followed by an
// exact copy of the first pair with the final repeated V coordinate.
Mesh spellRing(const anm_logic::State&, const SpriteRegion&, Position origin);
Mesh flatQuad(const anm_logic::State&, const SpriteRegion&, Position origin, float rotation,
              float scaleX, float scaleY);

// 429af0/42b100 writes its absolute script position at draw time and adds pi/2
// to angle. Main width/length scale must already contain the per-Laser20
// native external writes before the embedded ANM tick.
Mesh straightLaser(const laser::State&, const anm_logic::State&, const SpriteRegion&,
                   Position viewport = {224, 16, 0});

// 42cca0 uses Parameters.width (+464), not the potentially modified collision
// State.width (+70). UV is 0..1 along node ordinal; native vertex color is white.
Mesh curveLaser(const laser::State&, const anm_logic::State&, const SpriteRegion&,
                Position viewport = {224, 16, 0});
bool straightCapVisible(const laser::State&);
bool curveCapVisible(const laser::State&);
} // namespace th12::custom_geometry
