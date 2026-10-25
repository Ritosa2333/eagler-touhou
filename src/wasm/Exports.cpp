#include "../game/GameSession.hpp"
#include "Api.hpp"
#include "../game/AdaptedInput.hpp"
#include "../game/PresentationMotion.hpp"
#include "../game/Practice.hpp"
#include "../game/PracticeDamageSources.hpp"
// Stable WebAssembly C ABI. The game implementation is C++.
#include "../game/EnemyCallbacks.hpp"
#include "../game/GameState.hpp"
#include "../game/LaserDraw.hpp"
#include "../game/LaserWorld.hpp"
#include "../game/PresentationGeometry.hpp"
#include "../game/StageFlow.hpp"
#include "../game/SpellHistory.hpp"

extern "C" {
const int32_t* th12_practice_input(){return th12::practice::resolvedInput();}
void th12_practice_enabled(int enabled) { th12::practice::setEnabled(enabled!=0); }
void th12_practice_reset(int clearTools) { th12::practice::reset(clearTools!=0); }
int th12_practice_configure(const int32_t* words,int count) { return th12::practice::configure(words,count)?1:0; }
int th12_practice_start(int c,int shotValue,int difficulty,int seed) { return th12::practice::start(c,shotValue,difficulty,seed); }
int th12_practice_prepare_replay(int difficulty) { return th12::practice::prepareReplay(difficulty)?1:0; }
void th12_practice_replay(int replay) { th12::practice::setReplay(replay!=0); }
void th12_practice_tools(int mask) { th12::practice::setTools(uint32_t(mask)); }
void th12_practice_speed(int fps,int slow,int fast,int debug) { th12::practice::setSpeed(fps,slow,fast,debug!=0); }
double th12_practice_interval(int replay,int fast,int slow,int debug) { return th12::practice::interval(replay!=0,fast!=0,slow!=0,debug!=0); }
const char* th12_practice_state() { return th12::practice::snapshot().c_str(); }
int th12_practice_state_size() { return th12::practice::snapshotSize(); }
int th12_practice_boss_music() { return th12::practice::bossMusic()?1:0; }
int th12_practice_everlasting_bgm() { return th12::practice::everlastingBgm()?1:0; }
int th12_practice_damage_begin(int replay,int practice,int c,int shotValue) { return th12::practice::damage::begin(replay!=0,practice!=0,c,shotValue)?1:0; }
void th12_practice_damage_reset() { th12::practice::damage::reset(); }
int th12_practice_damage_transition(int from,int to) { return th12::practice::damage::armTransition(from,to)?1:0; }
int th12_practice_damage_load(const int32_t* words,int count) { return th12::practice::damage::load(words,count)?1:0; }
const int32_t* th12_practice_damage_stage(int stage) { return th12::practice::damage::stageWords(stage).data(); }
int th12_practice_damage_stage_words(int stage) { return int(th12::practice::damage::stageWords(stage).size()); }
const char* th12_practice_damage_state() { return th12::practice::damage::snapshot().c_str(); }
int th12_practice_damage_state_size() { return th12::practice::damage::snapshotSize(); }
const char* th12_presentation_motion() { return th12::capturePresentationMotion().c_str(); }
const void* th12_presentation_motion_binary() { return th12::capturePresentationMotionBinary().data(); }
int th12_presentation_motion_size() { return int(th12::presentationMotionBuffer.size()); }
void th12_motion(int mode,int x,int y) {
  if(mode<0||mode>4||std::abs(int64_t(x))>0x100000||std::abs(int64_t(y))>0x100000)mode=0;
  th12::pilot().adaptedMotion.begin(mode,x,y);
}
const int32_t* th12_motion_result() { return th12::pilot().adaptedMotion.result.data(); }
int th12_player_state() { th12::multiplayer::Scope seat(th12::multiplayer::active?th12::multiplayer::options.local:0); return th12::pilot().playerState; }
int th12_player_instance() { return int(th12::phasePlayerCounters[0]); }
float th12_player_speed(int focused) {
  th12::multiplayer::Scope seat(th12::multiplayer::active?th12::multiplayer::options.local:0);
  const auto& s=th12::loadouts[th12::pilot().character*2+th12::pilot().shot];
  return (focused?s.focus:s.speed)*th12::pilot().playerBomb.speedMultiplier*th12::gameSpeed;
}
int th12_load_spell_history(const int32_t* words, int count) { return th12::loadSpellHistory(words, count) ? 1 : 0; }
const char* th12_spell_history() { return th12::captureSpellHistory().c_str(); }
int th12_spell_history_size() { return int(th12::spellHistorySnapshotBuffer.size()); }
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
  th12::resetFullGameFlow();
  th12::startGame(c, s, d, seed);
}
int th12_start_full_game(int c, int s, int d, int seed, int practice) {
  return th12::startFullGame(c, s, d, seed, practice != 0);
}
void th12_set_full_game_mode(int practice) {
  th12::setFullGameMode(practice != 0);
}
int th12_replay_stage_flow(int practice, int fade_in) {
  if (!th12::spellState.replayWallClock || (practice != 0 && practice != 1) ||
      (fade_in != 0 && fade_in != 1)) return 0;
  th12::setFullGameMode(practice != 0);
  if (fade_in) th12::fullGameFlow.entryFadeAge = 0;
  return 1;
}
int th12_start_next_stage() { return th12::startNextStage(); }
int th12_continue_game() { return th12::continueFullGame(); }
const char* th12_stage_flow() { return th12::captureStageFlow().c_str(); }
int th12_stage_flow_size() { return int(th12::stageFlowSnapshotBuffer.size()); }
void th12_set_wall_time(double milliseconds) {
  th12::spellState.observeWallTime(milliseconds);
}
int th12_start_replay(int c, int s, int d, int seed, const int32_t* initial, int count) {
  if (!initial || (count != 17 && count != 38))
    return 0;
  th12::resetFullGameFlow();
  th12::startGame(c, s, d, seed, initial);
  if (count == 38) {
    std::copy(initial + 17, initial + 38, th12::spellState.replayResultTimes.begin());
    th12::spellState.replayResultTimeCount = 21;
  }
  return 1;
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
const char* th12_ufo() {
  return th12::captureUfo().c_str();
}
int th12_ufo_size() {
  return int(th12::ufoSnapshotBuffer.size());
}
const char* th12_boss_hud() {
  return th12::captureBossHud().c_str();
}
int th12_boss_hud_size() {
  return int(th12::bossHudSnapshotBuffer.size());
}
int th12_music_track() {
  return th12::stageNumber * 2 - (th12::bossMusic ? 0 : 1);
}
uint32_t th12_music_serial() {
  return th12::musicSerial;
}
const char* th12_background() {
  return th12::captureBackground().c_str();
}
int th12_background_size() {
  return th12::backgroundSnapshotBuffer.size();
}
void th12_tick(int held, int pressed) {
  if (!th12::tickFullGame(held, pressed))
    th12::tick(held, pressed);
}
int th12_draw() {
  using namespace th12;
  drawState.clear();
  // Native413220 (Enemy draw21) submits nothing. Enemy slots and their
  // children draw only through the registered ANM layer callbacks. In
  // particular, Cir00 waits before binding its yin-yang body: exporting the
  // default animation0 here invented a fairy throughout that entry delay.
  for (auto& b : bullets)
    if (b.active) {
      if(multiplayer::active&&b.friendly)multiplayerVisualOwners.push_back({0x10000000u+b.entityId,b.ownerSeat});
      drawState.push_back(
          {b.x + 224, b.y + 16, b.angle + pi / 2,
           b.friendly && b.type == 2 ? b.hitHeight / 448.f : 1, b.friendly ? 4 + multiplayer::pilots[b.ownerSeat].character : 2,
           b.friendly ? b.animation : b.enemy.animation, b.friendly ? (b.type == 2 ? 6 : 2) : 1,
           b.type * 16 + b.color, b.friendly ? b.weapon.visualAge : b.enemy.visualAge,
           0x10000000u + b.entityId, b.friendly ? b.weapon.interrupt : b.enemy.interrupt});
    }
  for(unsigned seat=0;seat<(multiplayer::active?multiplayer::options.count:1);++seat) {
  multiplayer::Scope owner(seat);
  for (const auto& v : pilot().playerBomb.visuals)
    if (v.active)
      drawState.push_back({v.x + 224, v.y + 16, v.angle, 1, 4 + v.bank, v.script, 7, 0, v.age,
                           0x50000000u + v.id, v.interrupt});
  for (const auto& trail : pilot().playerBomb.trails) {
    if (trail.kind == 13)
      if (const auto* node = animationScene.find(0x600000000ull + trail.id);
          node && node->customGeometry)
        continue; // Native410590 emits this entire leaf through the mesh ABI.
    for (size_t i = 1; i < trail.points.size(); i++) {
      const auto& a = trail.points[i - 1];
      const auto& b = trail.points[i];
      if(multiplayer::active)multiplayerVisualOwners.push_back({0x60000000u+trail.id*16+u32(i),seat});
      const float dx = b.x - a.x, dy = b.y - a.y;
      const float viewportX = trail.kind == 13 ? 0 : 224, viewportY = trail.kind == 13 ? 0 : 16;
      drawState.push_back(
          {float(double(a.x + b.x) * .5) + viewportX, float(double(a.y + b.y) * .5) + viewportY,
           float(std::atan2(double(dy), double(dx))), float(std::hypot(double(dx), double(dy))), 0,
           int(b.color), trail.kind, int(a.color), 0, 0x60000000u + trail.id * 16 + u32(i), 0,
           trail.kind == 12 ? trail.width : 1.f});
    }
  }
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
  for(unsigned seat=0;seat<(multiplayer::active?multiplayer::options.count:1);++seat) {
  multiplayer::Scope owner(seat);
  if (!pilot().deathWindow && !pilot().ghost) {
    for (int i = 0; i < pilot().playerMotion.count; i++) {
      const auto& o = pilot().playerMotion.options[i];
      drawState.push_back({o.x / 128.f + 224, o.y / 128.f + 16, 0, 1, 4 + pilot().character,
                           PlayerMotion::optionScripts[pilot().character * 2 + pilot().shot], 5, 0, o.age,
                           pilot().owner() + 1u + u32(i)});
    }
  }
  if (!pilot().deathWindow && (!pilot().invuln || frame % 6 < 3))
    drawState.push_back({pilot().px / 128.f + 224, pilot().py / 128.f + 16, 0, 1, 4 + pilot().character, pilot().playerMotion.bodyAnimation, 4,
                         0, pilot().playerMotion.bodyAge, pilot().owner()});
  }
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
int th12_anm_schedule_count() {
  return int(th12::animationScheduleState.size());
}
const void* th12_anm_schedule_ptr() {
  return th12::animationScheduleState.data();
}
int th12_anm_schedule_stride() {
  return sizeof(th12::AnimationDrawSchedule);
}
int th12_background_frame() {
  return th12::backgroundFrame;
}
int th12_background_visible() {
  return th12::backgroundVisible;
}
const float* th12_hud() {
  using namespace th12;
  multiplayer::Scope seat(multiplayer::active?multiplayer::options.local:0);
  int n = 0;
  for (auto& p : enemies)
    n += !p->hidden;
  u32 missing = 0;
  for (auto v : unsupported)
    missing += v;
  auto* boss = primaryBoss();
  hud = {float(frame),
         pilot().px / 128.f,
         pilot().py / 128.f,
         float(double(score) * 10),
         float(pilot().resources.power),
         float(pilot().resources.lives),
         float(pilot().resources.bombs),
         float(pilot().graze),
         float(th12::random.seed),
         float(th12::random.calls),
         float(n),
         float(bullets.size()),
         float(spell),
         float(ended),
         float(eventBits),
         float(missing),
         float(pilot().bombTimer),
         float(pilot().character),
         float(pilot().shot),
         float(difficulty),
         float(pilot().resources.pointValueStored),
         float(pilot().resources.lifeFragments),
         float(pilot().resources.bombFragments),
         float(ufoManager.inventory.colors[0]),
         float(ufoManager.inventory.colors[1]),
         float(ufoManager.inventory.colors[2]),
         float(ufoManager.kind),
         float(ufoManager.age),
         ufoManager.fill,
         float(pilot().resources.rank),
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
int32_t th12_score_stored() {
  return th12::score;
}
}
