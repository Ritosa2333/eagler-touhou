#include "CustomGeometry.hpp"

namespace th12::custom_geometry {
namespace {
Mesh metadata(const anm_logic::State& state, const SpriteRegion& region) {
  Mesh result;
  result.bank = region.bank;
  result.sprite = region.sprite;
  result.textureEntry = region.textureEntry;
  result.layer = state.layer;
  result.blend = state.blend;
  // 4514c5/4514db sets addressU/addressV=WRAP1. 459cf0 changes only
  // MAG/MIN filtering, to POINT1 when flags2.bit1 is set, LINEAR2 otherwise.
  result.filter = state.flags2 & 2 ? Filter::Point : Filter::Linear;
  return result;
}
bool drawable(const anm_logic::State& state) {
  // 45c900/45c3a0 checks the primary alpha even when selecting secondary color.
  return (state.flags & 3) == 3 && (state.primary >> 24) != 0;
}
uint32_t color(const anm_logic::State& state) {
  return state.flags & 0x10000 ? state.secondary : state.primary;
}
float add(float a, float b) {
  return float(double(a) + b);
}
float normalize(float angle) {
  // 4646e0 stores float after each wrap; preserve its finite loop bound.
  int count = 0;
  while (angle > anm_logic::Pi && count++ <= 33)
    angle = float(double(angle) - anm_logic::Tau);
  while (angle < -anm_logic::Pi && count++ <= 33)
    angle = float(double(angle) + anm_logic::Tau);
  return angle;
}
Vertex polar(float angle, float radius, Position origin, uint32_t argb, float u, float v) {
  // 4594b0/42e880 stores the fsincos product as float before adding position.
  const float dx = float(std::cos(double(angle)) * radius),
              dy = float(std::sin(double(angle)) * radius);
  return {add(origin.x, dx), add(origin.y, dy), origin.z, 1, argb, u, v};
}
float joinNormal(float current, float previous, bool first, float side) {
  float a = float(double(current) + side * double(anm_logic::Pi) / 2);
  if (first)
    return normalize(a);
  a = normalize(a);
  const float b = normalize(float(double(previous) + side * double(anm_logic::Pi) / 2));
  float difference;
  // 42cd45/42ce9f chooses the shortest difference before normalizing its half.
  if (double(b) - a > anm_logic::Pi)
    difference = float(double(a) + anm_logic::Tau - b);
  else if (double(a) - b > anm_logic::Pi)
    difference = float(double(a) - anm_logic::Tau - b);
  else
    difference = float(double(a) - b);
  difference = normalize(difference);
  return normalize(float(double(b) + normalize(float(double(difference) * .5))));
}
} // namespace

SpriteRegion spriteRegion(const anm_logic::State& state, const anm_logic::SpriteInfo& info) {
  return {state.spriteBank, state.sprite, info.textureEntry, info.width, info.height,
          info.u0,          info.v0,      info.u1,           info.v1};
}
Mesh ufoFillRing(const anm_logic::State& state, const SpriteRegion& region, Position origin) {
  auto result = metadata(state, region);
  const int count = state.geometryCount;
  if (state.mode != 13 || count < 2 || count > 65536 || !drawable(state))
    return result;
  // 458435..45869b: Rx is arc span, Rz its center, sx thickness, sy radius.
  float angle = normalize(float(double(state.rotation) - double(state.rx) * .5)), v = 0;
  const float step = float(double(state.rx) / (count - 1)),
              vstep = float(double(state.integers[1]) / (count - 1)),
              outer = float(double(state.sx) * .5 + state.sy),
              inner = float(double(state.sy) - double(state.sx) * .5),
              outerU = add(region.u0, state.u), innerU = add(region.u1, state.u);
  const auto argb = color(state);
  result.vertices.reserve(size_t(count) * 2);
  for (int i = 0; i < count; ++i) {
    const float textureV = add(v, state.v);
    result.vertices.push_back(polar(angle, outer, origin, argb, outerU, textureV));
    result.vertices.push_back(polar(angle, inner, origin, argb, innerU, textureV));
    v = add(v, vstep);
    angle = normalize(add(angle, step));
  }
  return result;
}

Mesh spellRing(const anm_logic::State& state, const SpriteRegion& region, Position origin) {
  auto result = metadata(state, region);
  const int count = state.geometryCount;
  if (state.mode != 9 || count < 2 || count > 65536 || !drawable(state))
    return result;
  float angle = state.rotation, v = 0;
  // Retail4a3cd0 is the double representation of the float tau constant.
  const float step = float(double(anm_logic::Tau) / (count - 1)),
              vstep = float(double(state.integers[1]) / (count - 1)),
              outer = float(double(state.sx) * .5 + state.sy),
              inner = float(double(state.sy) - double(state.sx) * .5),
              outerU = add(region.u0, state.u), innerU = add(region.u1, state.u);
  const auto argb = color(state);
  result.vertices.reserve(size_t(count) * 2);
  for (int i = 0; i < count - 1; ++i) {
    const float textureV = add(v, state.v);
    result.vertices.push_back(polar(angle, outer, origin, argb, outerU, textureV));
    result.vertices.push_back(polar(angle, inner, origin, argb, innerU, textureV));
    v = add(v, vstep);
    angle = normalize(add(angle, step));
  }
  auto outerEnd = result.vertices[0], innerEnd = result.vertices[1];
  outerEnd.v = innerEnd.v = add(v, state.v);
  result.vertices.push_back(outerEnd);
  result.vertices.push_back(innerEnd);
  return result;
}

Mesh flatQuad(const anm_logic::State& state, const SpriteRegion& region, Position origin,
              float rotation, float scaleX, float scaleY) {
  auto result = metadata(state, region);
  result.primitive = Primitive::TriangleList;
  if (!drawable(state))
    return result;
  const float spriteWidth = state.spriteWidth == -1 ? region.width : state.spriteWidth,
              spriteHeight = state.spriteHeight == -1 ? region.height : state.spriteHeight,
              width = float(double(spriteWidth) * scaleX),
              height = float(double(spriteHeight) * scaleY);
  const float left = state.anchorX == 1   ? 0
                     : state.anchorX == 2 ? -width
                                          : -width / 2,
              right = state.anchorX == 1   ? width
                      : state.anchorX == 2 ? 0
                                           : width / 2,
              top = state.anchorY == 1   ? 0
                    : state.anchorY == 2 ? -height
                                         : -height / 2,
              bottom = state.anchorY == 1   ? height
                       : state.anchorY == 2 ? 0
                                            : height / 2;
  const float cosine = float(std::cos(double(rotation))), sine = float(std::sin(double(rotation))),
              u0 = add(region.u0, state.u), v0 = add(region.v0, state.v),
              u1 = float((double(region.u1) - region.u0) * state.uvScaleX + region.u0 + state.u),
              v1 = float((double(region.v1) - region.v0) * state.uvScaleY + region.v0 + state.v);
  const float xy[4][2] = {{left, top}, {right, top}, {left, bottom}, {right, bottom}};
  const float uv[4][2] = {{u0, v0}, {u1, v0}, {u0, v1}, {u1, v1}};
  const auto argb = color(state);
  // 45a4a0 outputs TL/TR/BL, TR/BL/BR to the actual TriangleList batch.
  for (const int i : {0, 1, 2, 1, 2, 3})
    result.vertices.push_back(
        {float(double(xy[i][0]) * cosine - double(xy[i][1]) * sine + origin.x),
         float(double(xy[i][0]) * sine + double(xy[i][1]) * cosine + origin.y), origin.z, 1, argb,
         uv[i][0], uv[i][1]});
  return result;
}

Mesh straightLaser(const laser::State& laser, const anm_logic::State& state,
                   const SpriteRegion& region, Position viewport) {
  const Position origin{add(add(laser.position.x, viewport.x), state.offsetX),
                        add(add(laser.position.y, viewport.y), state.offsetY),
                        add(laser.position.z, state.offsetZ)};
  auto result = flatQuad(state, region, origin, normalize(add(laser.angle, anm_logic::Pi / 2)),
                         state.sx, state.sy);
  result.pass = DrawPass::LaserManager;
  result.drawPriority = 29;
  return result;
}

Mesh curveLaser(const laser::State& laser, const anm_logic::State& state,
                const SpriteRegion& region, Position viewport) {
  auto result = metadata(state, region);
  result.pass = DrawPass::LaserManager;
  result.drawPriority = 29;
  const size_t count = laser.nodes.size();
  if (count < 2 || count > 65536 || !drawable(state))
    return result;
  const float radius = float(double(laser.parameters.width) * .5),
              textureStep = float(1. / double(count - 1));
  float u = 0;
  result.vertices.reserve(count * 2);
  for (size_t i = 0; i < count; ++i) {
    const auto& point = laser.nodes[i];
    const float previous = i ? laser.nodes[i - 1].angle : point.angle;
    // Native 42cca0 always outputs z0 and opaque white, independent of ANM
    // color and UV scroll. Only V endpoints come from the main sprite corners.
    const Position origin{add(point.position.x, viewport.x), add(point.position.y, viewport.y), 0};
    result.vertices.push_back(polar(joinNormal(point.angle, previous, i == 0, 1), radius, origin,
                                    0xffffffff, u, region.v0));
    result.vertices.push_back(polar(joinNormal(point.angle, previous, i == 0, -1), radius, origin,
                                    0xffffffff, u, region.v1));
    u = add(u, textureStep);
  }
  return result;
}
bool straightCapVisible(const laser::State& laser) {
  return laser.headOffset == 0;
}
bool curveCapVisible(const laser::State& laser) {
  return int(laser.nodes.size()) >= laser.age.current;
}
} // namespace th12::custom_geometry
