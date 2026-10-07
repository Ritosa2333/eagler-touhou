#include "../game/GameSession.hpp"
#include "Api.hpp"
// Stable WebAssembly C ABI. The game implementation is C++.
#include "../game/EnemyCallbacks.hpp"
#include "../game/GameState.hpp"
#include "../game/LaserDraw.hpp"
#include "../game/LaserWorld.hpp"
#include "../game/PresentationGeometry.hpp"

extern "C" {
int th12_sound_events() {
  return th12::sound_system::capture();
}
const void* th12_sound_events_ptr() {
  return th12::sound_system::drawState.data();
}
int th12_sound_events_stride() {
  return sizeof(th12::sound_system::Event);
}
int th12_mesh_draw() {
  th12::captureCustomGeometry();
  return th12::meshDrawState.size();
}
const void* th12_mesh_draw_ptr() {
  return th12::meshDrawState.data();
}
int th12_mesh_draw_stride() {
  return sizeof(th12::MeshRecord);
}
const void* th12_mesh_vertices_ptr() {
  return th12::meshVertexState.data();
}
int th12_mesh_vertices_count() {
  return th12::meshVertexState.size();
}
int th12_mesh_vertex_stride() {
  return sizeof(th12::custom_geometry::Vertex);
}
int th12_score_popups() {
  return th12::scorePopups.size();
}
const void* th12_score_popups_ptr() {
  return th12::scorePopups.data();
}
int th12_score_popups_stride() {
  return sizeof(th12::special_enemy::ScorePopup);
}
int th12_laser_draw() {
  th12::captureLaserDraw();
  return th12::laserDrawState.size();
}
const void* th12_laser_draw_ptr() {
  return th12::laserDrawState.data();
}
int th12_laser_draw_stride() {
  return sizeof(th12::LaserSegment);
}
int th12_load_ecl(const uint8_t* p, int n) {
  return th12::program.add(p, n);
}
int th12_load_msg(int index, const uint8_t* p, int n) {
  if (index < 0 || index >= 6)
    return 0;
  return th12::messageFiles[index].load(p, n);
}
int th12_load_std(const uint8_t* p, int n) {
  if (n < 0)
    return 0;
  std::string error;
  return th12::stageProgram.load(p, size_t(n), error);
}
int th12_load_anm(int bank, const uint8_t* p, int n) {
  if (bank < 0 || bank > 31 || n < 0)
    return 0;
  std::string error;
  return th12::animationScene.registry.loadBank(bank, p, size_t(n), error);
}
void th12_unload_anm(int bank) {
  th12::animationScene.registry.unloadBank(bank);
}
void th12_oracle_fixture(int flags) {
  th12::oracleFixtureFlags = flags & 7;
}
void th12_select_stage(int number) {
  th12::selectStage(number);
}
int th12_load_sht(int index, const uint8_t* p, int n) {
  using namespace th12;
  if (index < 0 || index >= 6 || !loadouts[index].open(p, n))
    return 0;
  const int c = index / 2;
  loadouts[index].hit = std::array<float, 3>{2, 3.5f, 3}[c];
  loadouts[index].attractionSpeed = std::array<float, 3>{5, 7, 6}[c];
  loadouts[index].attractionDiameter = std::array<float, 3>{60, 70, 60}[c];
  return 1;
}
void th12_start(int c, int s, int d, int seed) {
  th12::startGame(c, s, d, seed);
}
void th12_set_initial(int x, int y, int p, int life, int bomb, int points) {
  th12::setInitial(x, y, p, life, bomb, points);
}
void th12_set_replay_economy(int piv, int lifeFragments, int bombFragments, int red, int blue,
                             int green, int rank) {
  th12::setReplayEconomy(piv, lifeFragments, bombFragments, red, blue, green, rank);
}
const char* th12_state() {
  return th12::captureState().c_str();
}
int th12_state_size() {
  return th12::snapshotBuffer.size();
}
const char* th12_message() {
  return th12::captureMessage().c_str();
}
int th12_message_size() {
  return th12::messageSnapshotBuffer.size();
}
const char* th12_spell() {
  return th12::captureSpell().c_str();
}
int th12_spell_size() {
  return th12::spellSnapshotBuffer.size();
}
const char* th12_background() {
  return th12::captureBackground().c_str();
}
int th12_background_size() {
  return th12::backgroundSnapshotBuffer.size();
}
void th12_tick(int held, int pressed) {
  th12::tick(held, pressed);
}
int th12_draw() {
  using namespace th12;
  drawState.clear();
  for (auto& p : enemies) {
    auto& e = *p;
    if (e.active && !e.hidden)
      drawState.push_back(
          {e.x + 224, e.y + 16, 0, 1, e.boundBank, e.animation, 0, 0, e.animationAge, e.id});
  }
  for (auto& b : bullets)
    if (b.active)
      drawState.push_back(
          {b.x + 224, b.y + 16, b.angle + pi / 2,
           b.friendly && b.type == 2 ? b.hitHeight / 448.f : 1, b.friendly ? 3 : 2,
           b.friendly ? b.animation : b.enemy.animation, b.friendly ? (b.type == 2 ? 6 : 2) : 1,
           b.type * 16 + b.color, b.friendly ? b.weapon.visualAge : b.enemy.visualAge,
           0x10000000u + b.entityId, b.friendly ? b.weapon.interrupt : b.enemy.interrupt});
  for (const auto& v : playerBomb.visuals)
    if (v.active)
      drawState.push_back({v.x + 224, v.y + 16, v.angle, 1, v.bank, v.script, 7, 0, v.age,
                           0x50000000u + v.id, v.interrupt});
  for (const auto& trail : playerBomb.trails)
    for (size_t i = 1; i < trail.points.size(); i++) {
      const auto& a = trail.points[i - 1];
      const auto& b = trail.points[i];
      const float dx = b.x - a.x, dy = b.y - a.y;
      drawState.push_back({float(double(a.x + b.x) * .5) + 224, float(double(a.y + b.y) * .5) + 16,
                           float(std::atan2(double(dy), double(dx))),
                           float(std::hypot(double(dx), double(dy))), 0, int(b.color), trail.kind,
                           int(a.color), 0, 0x60000000u + trail.id * 16 + u32(i), 0,
                           trail.kind == 12 ? trail.width : 1.f});
    }
  for (const auto& i : itemManager.entities)
    if (i.visible()) {
      const int age = i.visualInterrupt ? i.visualInterruptAge : i.visualAge;
      if (i.position.y >= -8 || i.type >= 9)
        drawState.push_back({i.position.x + 224, i.position.y + 16, 0, 1, 2, i.visualScript, 3,
                             i.type, age, 0x20000000u + i.id, i.visualInterrupt, i.alpha});
      else if (i.type >= 1 && i.type <= 8) {
        const float alpha =
            float(nativeInt(float(double(i.position.y + 8) * 255 / 32)) & 255) / 255.f;
        drawState.push_back({i.position.x + 224, 24, 0, 1, 2, i.type + 185, 3, i.type, i.visualAge,
                             0x30000000u + i.id, 0, alpha});
      }
      if (i.hintActive)
        drawState.push_back({i.position.x + 224, i.position.y + 16, 0, 1, 2, i.hintScript, 3,
                             i.type, 0, 0x40000000u + i.id, i.hintInterrupt, i.alpha});
    }
  if (!deathWindow) {
    for (int i = 0; i < playerMotion.count; i++) {
      const auto& o = playerMotion.options[i];
      drawState.push_back({o.x / 128.f + 224, o.y / 128.f + 16, 0, 1, 3,
                           PlayerMotion::optionScripts[character * 2 + shot], 5, 0, o.age,
                           0xffff0001u + u32(i)});
    }
  }
  if (!deathWindow && (!invuln || frame % 6 < 3))
    drawState.push_back({px / 128.f + 224, py / 128.f + 16, 0, 1, 3, playerMotion.bodyAnimation, 4,
                         0, playerMotion.bodyAge, 0xffff0000u});
  return drawState.size();
}
const void* th12_draw_ptr() {
  return th12::drawState.data();
}
int th12_draw_stride() {
  return sizeof(th12::Sprite);
}
int th12_draw_version() {
  return 2;
}
int th12_anm_draw() {
  th12::captureAnimationDraw();
  return th12::animationDrawState.size();
}
const void* th12_anm_draw_ptr() {
  return th12::animationDrawState.data();
}
int th12_anm_draw_stride() {
  return sizeof(th12::AnimationPose);
}
int th12_background_frame() {
  return th12::backgroundFrame;
}
int th12_background_visible() {
  return th12::backgroundVisible;
}
const float* th12_hud() {
  using namespace th12;
  int n = 0;
  for (auto& p : enemies)
    n += !p->hidden;
  u32 missing = 0;
  for (auto v : unsupported)
    missing += v;
  auto* boss = primaryBoss();
  hud = {float(frame),
         px / 128.f,
         py / 128.f,
         float(double(score) * 10),
         float(power),
         float(lives),
         float(bombs),
         float(graze),
         float(th12::random.seed),
         float(th12::random.calls),
         float(n),
         float(bullets.size()),
         float(spell),
         float(ended),
         float(eventBits),
         float(missing),
         float(bombTimer),
         float(character),
         float(shot),
         float(difficulty),
         float(resources.pointValueStored),
         float(resources.lifeFragments),
         float(resources.bombFragments),
         float(ufoManager.inventory.colors[0]),
         float(ufoManager.inventory.colors[1]),
         float(ufoManager.inventory.colors[2]),
         float(ufoManager.kind),
         float(ufoManager.age),
         ufoManager.fill,
         float(resources.rank),
         float(ufoManager.bucketA),
         float(ufoManager.bucketB),
         float(boss ? boss->maxLife : 0),
         float(boss ? boss->phaseAge : 0),
         float(boss ? boss->life : 0),
         ufoManager.rewardDisplayMultiplier,
         float(ufoManager.rewardDisplayFrames),
         float(ufoManager.rewardDisplayScore),
         float(dialogue),
         float(stageNumber),
         float(double(score) * 10)};
  return hud.data();
}
int th12_hud_size() {
  return th12::hud.size();
}
const uint32_t* th12_unsupported() {
  return th12::unsupported.data();
}
uint32_t th12_rng16(int seed) {
  th12::Random r{uint16_t(seed), 0};
  return r.next16();
}
uint32_t th12_rng32(int seed) {
  th12::Random r{uint16_t(seed), 0};
  return r.next32();
}
}
