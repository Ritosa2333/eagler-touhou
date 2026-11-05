#include "../../src/game/StageBackgroundEffect.hpp"
#include "../../src/stage_logic.hpp"
using namespace th12;
stage_logic::Program program;
stage_logic::State stage;
StageBackgroundEffect effect;
custom_geometry::Mesh mesh;
bool activeSpell = false;
float fields[5];
struct World : stage_logic::World {
  void backgroundEffect(int kind) override { effect.begin(kind); }
  void tickBackgroundEffect() override { effect.tick(activeSpell); }
} world;
extern "C" {
int load(const uint8_t* bytes, int length) {
  std::string error;
  return program.load(bytes, size_t(length), error);
}
void reset() {
  stage.reset(program);
  effect.reset();
  activeSpell = false;
}
void tick() {
  stage.tick(world);
}
void spell(int active) {
  activeSpell = active != 0;
}
int columns() {
  return effect.columns;
}
int rows() {
  return effect.rows;
}
int mode() {
  return effect.mode;
}
const float* state_fields() {
  fields[0] = effect.targetRadius;
  fields[1] = effect.radius;
  fields[2] = effect.angleX;
  fields[3] = effect.angleY;
  fields[4] = stage.callbackCount;
  return fields;
}
const custom_geometry::Vertex* grid_vertices() {
  return effect.vertices.data();
}
int draw(int column) {
  anm_logic::State state;
  state.flags = 103292935;
  state.spriteBank = 8;
  state.sprite = 75;
  state.primary = 0xffffffffu;
  state.blend = 0;
  mesh = effect.draw(column, state);
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
         : field == 4 ? mesh.drawPriority
                      : mesh.textureEntry;
}
}
