#include "StageBackgroundEffect.hpp"

namespace th12 {
StageBackgroundEffect stageBackgroundEffect;
namespace {
float add(float a, double b) {
  return float(double(a) + b);
}
float normalize(float angle) {
  return anm_logic::normalized(angle);
}
} // namespace
void StageBackgroundEffect::reset() {
  *this = {};
}
bool StageBackgroundEffect::begin(int kind) {
  reset();
  mode = kind;
  targetRadius = 112;
  radius = 192;
  color = 0xffffffffu;
  // Mode2 is a separate radial effect and game RNG path. It is deliberately
  // left unsupported until its complete native formula is sampled.
  if (kind != 1)
    return false;
  columns = 17;
  rows = 7;
  vertices.resize(size_t(columns * rows));
  return true;
}
void StageBackgroundEffect::tick(bool activeSpell) {
  if (mode != 1 || !columns || activeSpell)
    return;
  // 410020 prepares screen-space base geometry before the Stage12 deformation.
  float x = 32;
  for (int column = 0; column < columns; ++column) {
    float y = 16;
    for (int row = 0; row < rows; ++row) {
      vertices[size_t(column * rows + row)] = {
          x, y, 0, 1, 0xc0ffffffu, float(double(x) / 640), float(double(y) / 480)};
      y = add(y, float(128. / (rows - 1)));
    }
    x = add(x, float(384. / (columns - 1)));
  }
  float a = angleX, b = angleY;
  for (int column = 0; column < columns; ++column) {
    const float verticalSine = float(std::sin(double(b)));
    for (int row = 0; row < rows; ++row) {
      const float amplitude = float(24. - double(row) * 24 / (rows - 1)),
                  horizontalSine = float(std::sin(double(a))),
                  dx = float(double(horizontalSine) * amplitude),
                  dy = float(double(amplitude) * verticalSine);
      if (column != 0 && row != 0 && column != columns - 1 && row != rows - 1) {
        auto& vertex = vertices[size_t(column * rows + row)];
        vertex.x = add(vertex.x, dx);
        vertex.y = add(vertex.y, dy);
      }
      a = normalize(add(a, .6684240102767944));
    }
    b = normalize(add(b, -1.4959965944290161));
  }
  angleX = normalize(add(angleX, .04908738657832146));
  angleY = normalize(add(angleY, .039269909262657166));
}
custom_geometry::Mesh StageBackgroundEffect::draw(int column, const anm_logic::State& state) const {
  custom_geometry::Mesh mesh;
  mesh.bank = state.spriteBank;
  mesh.sprite = state.sprite;
  mesh.textureEntry = 2; // text.anm sprite75 uses the original @R entry.
  mesh.layer = 28;
  mesh.drawPriority = 14;
  mesh.blend = state.blend;
  mesh.filter = state.flags2 & 2 ? custom_geometry::Filter::Point : custom_geometry::Filter::Linear;
  if (column < 0 || column >= columns - 1 || (state.flags & 3) != 3 || !(state.primary >> 24))
    return mesh;
  for (int row = 0; row < rows; ++row) {
    mesh.vertices.push_back(vertices[size_t(column * rows + row)]);
    mesh.vertices.push_back(vertices[size_t((column + 1) * rows + row)]);
  }
  return mesh;
}
bool StageBackgroundEffect::owns(uint64_t key) const {
  return key && std::find(keys.begin(), keys.end(), key) != keys.end();
}
} // namespace th12
