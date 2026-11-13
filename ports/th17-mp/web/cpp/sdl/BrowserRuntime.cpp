#include <eagler/input/BrowserGamepad.hpp>
#include "ApplicationState.hpp"
#include "ThpracUi.hpp"
#include <eagler/thprac/PracticePointer.hpp>
#include "../game/GameKeyboard.hpp"
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <optional>
#if TH17_MULTIPLAYER
#include <malloc.h>
#include "../multiplayer/Session.hpp"
#include "../multiplayer/FrameImage.hpp"
#include "../multiplayer/ReplayArchive.hpp"
#include "../multiplayer/StageEntry.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/input/LocalPlayerCue.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>
#include <eagler/replay/Workspace.hpp>
#endif
EM_JS(void, th17_browser_frame, (int ok,double milliseconds,unsigned ticks,int presented), { Module["onGameFrame"]?.(ok,milliseconds,ticks,presented); });
EM_JS(void, th17_browser_frame_begin, (), { Module["onGameFrameStart"]?.(); });
EM_JS(int, th17_host_has_focus, (), {
 try { return parent.document.hasFocus() && !parent.document.hidden ? 1 : 0; }
 catch (_) { return document.hasFocus() && !document.hidden ? 1 : 0; }
});
EM_JS(int, th17_browser_keyboard_owned, (), { return typeof Module.resetBrowserKeyboard === "function"; });
EM_JS(void, th17_browser_keyboard_reset, (), { Module.resetBrowserKeyboard?.(); });
#if TH17_ROOM_BENCHMARK
namespace th17 {extern u32 room_benchmark_route_defeats;}
#endif
namespace th17::sdl {
#if TH17_MULTIPLAYER
extern bool room_phase_trace;
extern std::array<u32,3> room_application_us;
#endif
#if TH17_ROOM_BENCHMARK
extern std::array<u32,4> room_benchmark_draw;
#endif
namespace {
std::unique_ptr<ApplicationState> app;
std::string last_error;
#if TH17_MULTIPLAYER
enum RoomPhase { Input, Capture, Restore, Confirm, Canvas, Pcm, ImageRelease, RoomPhaseCount };
std::array<u32,RoomPhaseCount> room_phase_us{};
struct RoomPhaseTimer {
 RoomPhase phase;
 double begin=room_phase_trace?emscripten_get_now():0;
 ~RoomPhaseTimer(){if(begin)room_phase_us[phase]+=u32((emscripten_get_now()-begin)*1000);}
};
struct RoomRuntime final : multiplayer::World {
#if TH17_ROOM_BENCHMARK
 using Status=std::array<u32,54>;
#else
 using Status=std::array<u32,44>;
#endif
 struct Frame {
  u32 number=Netplay::INVALID_FRAME;
  multiplayer::FrameImage image;
  bool has_image=false,release_pending=false;
  std::vector<ApplicationState::RoomSound> sounds;
  Status snapshot{};
  u32 stage=0,score=0;
  Netplay::SpectatorStagePublisher::Frame publication;
 };
 multiplayer::FrameImage::BufferPool snapshot_buffers;
 std::array<Frame,multiplayer::Session::rollback_frames+1> history;
 std::array<Status,16> confirmed{};
 Netplay::BrowserPeerTransport transport;
 multiplayer::Session session{transport};
 multiplayer::SessionSetup setup;
 multiplayer::ReplayArchive replay;
 Netplay::SpectatorStagePublisher publisher;
 Netplay::SpectatorStagePlayback playback{Netplay::SpectatorStagePlayback::RestoreMode::Run};
 Netplay::SpectatorStageView spectator_view;
 bool spectator=false,playing_replay=false,local_results=false,publisher_failed=false,ended=false;
 bool publication_started=false,terminal=false;
 u32 playback_frame=0,bootstrap_target=0,start_stage=1,entry_stage=0,confirmed_score=0;
 u32 published_through=0,current_stage=0;
 u64 spectator_deadline=0;
 std::string saved_path,saved_name="PLAYER";
 bool save_replay(i32 slot=-1,const char* name=nullptr,bool completed=false);
 bool publish(bool finish=false);
 bool receive_spectator();
 bool advance_playback(bool advance);
 GameInput main_input;
 bool entered=false, trace=false, discarded_images=false, retry_forward=false;
 double replay_deadline=0,refresh_start=0,refresh_deadline=0,tick_cost_ms=5;
 double replay_step_start=0,capture_start=0,logical_step_ms=4;
 std::uint64_t now_ms=0;
 u32 capture_count=0,capture_us=0,replay_slices=0,logical_draws=0;
 unsigned pending_releases=0;
 uintptr_t early_capture_token=0;
 double early_capture_until=0;
 u32 early_capture_head=0;
 bool released_confirmed=false;
 std::array<u32,3> phase_corrections{};
 u32 barrier=Netplay::INVALID_FRAME;
 u32 restored_checkpoint=Netplay::INVALID_FRAME;
 u32 combat_corrections=0,spell_corrections=0;
#if TH17_ROOM_BENCHMARK
 u32 benchmark_input_frame=0;
 u32 benchmark_ghost_seat=3;
 i32 benchmark_story_stage=0;
 std::array<u32,4> benchmark_history{};
#endif

 RoomRuntime(){for(auto& frame:history)frame.image.use_buffer_pool(snapshot_buffers);}

 Frame& state(u32 frame){return history[frame%history.size()];}
 void clear_image(Frame& frame){
  if(!frame.has_image)return;
  const RoomPhaseTimer timing{ImageRelease};
  frame.has_image=false;
  if(frame.release_pending){frame.release_pending=false;--pending_releases;}
#if TH17_ROOM_BENCHMARK
  if(trace&&!frame.image.empty()){
   const auto begin=emscripten_get_now();
   frame.image.clear();
   ++benchmark_history[2];benchmark_history[3]+=u32((emscripten_get_now()-begin)*1000);
   return;
  }
#endif
  frame.image.clear();
 }
 void release_retired(double deadline){
  if(!pending_releases)return;
  const bool boundary=!app->can_rollback();
  if(!boundary&&(released_confirmed||emscripten_get_now()+2>=deadline))return;
  for(auto& saved:history)if(saved.release_pending){
   clear_image(saved);
   if(!boundary)break;
  }
 }
 Status status(u32 number) const;
 bool can_predict() const override{return entered&&barrier!=session.next_frame()&&app->can_rollback();}
 void speculating(bool active) override{app->room_speculative=active;}
 void replaying(bool intermediate) override{
  app->graphics.logical_drawing=intermediate;
  if(intermediate)replay_step_start=emscripten_get_now();
 }
 bool replay_budget(unsigned replayed,unsigned remaining) const override{
  if(replayed>=multiplayer::Session::rollback_frames)return false;
  const double now=emscripten_get_now();
  // Preserve the short repair slice even when the browser delivered RAF late.
  // Estimating the cost before every step can otherwise leave only one step
  // per callback in a dense fight.
  if(replayed<4&&now<replay_deadline)return true;
  if(!remaining)return false;
  // Spend spare refresh time on the next repair step. The whole tail does
  // not have to fit to make progress.
  const bool logical=remaining>1||app->graphics.backend.presentation_pending();
  const double cost=logical?logical_step_ms:tick_cost_ms;
  return now+cost+1<std::max(refresh_deadline,refresh_start+3);
 }
 bool blocked() const override{return app->room_barrier;}
 bool capture(u32 frame) override{
  const RoomPhaseTimer timing{Capture};
  if(!app->graphics.logical_drawing&&!app->graphics.backend.presentation_pending())
   capture_start=emscripten_get_now();
  auto& saved=state(frame);
  saved.number=frame;
  // Rewinding to a speculative frame restores exactly its existing image.
  // Keep it until the corrected simulation consumes that frame.
  if(restored_checkpoint==frame)return true;
  clear_image(saved);saved.has_image=true;saved.image.save(main_input);
  const double begin=trace?emscripten_get_now():0;
  const bool ok=app->save_rollback(saved.image);
  if(trace){++capture_count;capture_us+=u32((emscripten_get_now()-begin)*1000);}
  return ok;
 }
 bool restore(u32 frame) override{
  const RoomPhaseTimer timing{Restore};
  auto& saved=state(frame);
  if(saved.number!=frame)return false;
  const bool rewinding=frame<session.next_frame();
  if(rewinding){
   const auto& battle=app->scene()->battle;
   if(battle.bullet_scene->manager.visible_count||battle.laser_scene->manager.count())++combat_corrections;
   if(trace){
    // Classify the frame that will be corrected, not the current prediction head.
    const auto& before=frame?state(frame-1).snapshot:Status{};
    const unsigned phase=(before[30]&1)?2:before[38]?1:0;
    ++phase_corrections[phase];
    if(phase==2)++spell_corrections;
   }else if(battle.spell.flags&1)++spell_corrections;
  }
#if TH17_ROOM_BENCHMARK
  const auto begin=trace?emscripten_get_now():0;
#endif
  if(!saved.image.restore())return false;
  restored_checkpoint=rewinding?frame:Netplay::INVALID_FRAME;
#if TH17_ROOM_BENCHMARK
  if(trace){++benchmark_history[0];benchmark_history[1]+=u32((emscripten_get_now()-begin)*1000);}
#endif
  // A repair can span several refreshes. Invalidate the whole abandoned
  // timeline now, but release each image when replay replaces its slot.
  discarded_images|=rewinding;
  for(auto& future:history)if(future.number!=Netplay::INVALID_FRAME&&future.number>=frame){
   if(!rewinding)clear_image(future);
   future.sounds.clear();future.number=Netplay::INVALID_FRAME;
  }
  if(barrier!=Netplay::INVALID_FRAME&&barrier>frame)barrier=Netplay::INVALID_FRAME;
  return true;
 }
 bool confirm(u32 frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs) override{
  const RoomPhaseTimer timing{Confirm};
  auto& saved=state(frame);
  if(saved.number!=frame)return app->fail("TH17 confirmed frame output is missing");
  for(const auto& sound:saved.sounds)if(!app->audio(sound.id,sound.pan,sound.action,sound.queued))return false;
  app->audio_device.update();
  if(!replay.append(frame,saved.stage,inputs.data()))return app->fail("TH17 confirmed Replay capacity exhausted");
  confirmed_score=saved.score;
  if(!setup.local&&!publisher_failed&&(publication_started||saved.publication.schema)){
   if(!publication_started){
    if(!publisher.Reset('G',setup.seats,setup.room,setup.gameplay_abi(),frame)){
     publisher_failed=true;transport.StopSpectators();
    }else publication_started=true;
   }
   saved.publication.inputs=inputs;saved.publication.hasInputs=true;
   if(!publisher.Record(std::move(saved.publication))){publisher_failed=true;transport.StopSpectators();}
   else {
    published_through=frame;
    if(frame+1-publisher.Published()>=4)publish();
   }
  }
  confirmed[(frame+1)%confirmed.size()]=saved.snapshot;
  if(saved.has_image){
   // Retain even the first confirmed image until after the draw when the
   // existing ring has room. Reclaim immediately at the bound or a scene edge.
   if(pending_releases<2&&app->can_rollback()){
    saved.release_pending=true;++pending_releases;
   }else{clear_image(saved);released_confirmed=true;}
  }
  saved.sounds.clear();saved.number=Netplay::INVALID_FRAME;
  return app->audio_device.error.empty()||app->fail(app->audio_device.error);
 }
 bool step(u32 number,const Netplay::FrameDecision& frame) override{
  const bool measure_draw=!spectator&&!playing_replay&&!app->graphics.logical_drawing&&
                          !app->graphics.backend.presentation_pending();
  const double tick_start=measure_draw?(app->room_speculative?capture_start:emscripten_get_now()):0;
  restored_checkpoint=Netplay::INVALID_FRAME;
  app->room_barrier=false;
  if(!entered){
   std::array<std::array<i32,2>,3> loadouts{};
   for(u32 seat=0;seat<setup.seats;++seat)
    loadouts[seat]={setup.loadouts[seat].character,setup.loadouts[seat].animal};
   i32 initial_stage=setup.difficulty==4?7:1;
#if TH17_ROOM_BENCHMARK
   if(benchmark_story_stage)initial_stage=benchmark_story_stage;
#endif
   if(!app->begin_room(setup.seed,setup.difficulty,setup.seats,loadouts,initial_stage,setup.challenge))return false;
   app->renderer.hud_viewer=setup.local+1;
   entered=true;
  }
  app->room_preview=false;
  if(app->title)app->title->state.room_preview=false;
  const auto movement=[](const Netplay::FrameInput& input)->PlayerTouch{
   if(input.analogMode!=Netplay::AnalogMode::DirectTouch)return {};
   return {input.unlimited?2:1,input.x,input.y};
  };
  const auto menu_before=main_input;
  u32 menu_buttons=frame.inputs[0].buttons;
  if(app->ending)for(u32 seat=1;seat<setup.seats;++seat)menu_buttons|=frame.inputs[seat].buttons&0x200u;
  main_input.update(menu_buttons);
  if((main_input.pressed&0x100)&&!app->rollback_boundary()){barrier=number;return false;}
  app->touch=movement(frame.inputs[0]);
  for(u32 seat=1;seat<setup.seats;++seat)
   app->room_partners[seat-1]={frame.inputs[seat].buttons,movement(frame.inputs[seat])};
  app->controller_buttons=0;app->keyboard_state={};
  auto& output=state(number);
  if(!app->room_speculative)clear_image(output);
  output.number=number;output.sounds.clear();app->room_sounds=&output.sounds;
  if(app->scene())current_stage=u32(app->scene()->battle.stage);
  if(!current_stage)current_stage=u32(app->progress.initial_stage);
  output.stage=current_stage;
  output.publication={};output.publication.frame=number;output.publication.stage=output.stage;
  if(!spectator&&!playing_replay&&!setup.local&&!publisher_failed&&app->scene()&&!app->scene()->active()&&
     entry_stage!=app->room_stage_generation&&app->loading_destination==AppScene::Uninitialized){
   if(multiplayer::capture_stage_entry(*app,setup,menu_before,number,output.publication.initializer)){
    output.publication.schema=multiplayer::StageEntry::transport_schema;entry_stage=app->room_stage_generation;
   }else {publisher_failed=true;transport.StopSpectators();}
  }
  const auto bosses_before=app->scene()?app->scene()->battle.enemy_world.boss_ids:std::array<u32,3>{};
#if TH17_ROOM_BENCHMARK
  if(auto* scene=app->scene();scene&&(scene->battle.stage==7||benchmark_story_stage)){
   for(u32 seat=0;seat<scene->battle.pilot_count();++seat)scene->battle.pilot(seat).life.invulnerability.set(2);
  }
  if(number==800&&benchmark_ghost_seat<setup.seats){
   auto* scene=app->scene();
   if(!scene||!scene->active()){
    app->room_sounds=nullptr;
    return app->fail("Ghost fixture needs an active stage");
   }
   auto& pilot=scene->battle.pilot(benchmark_ghost_seat);
   pilot.session.extra_lives=0;
   if(!pilot.life.hit()){app->room_sounds=nullptr;return app->fail(pilot.life.error);}
  }
#endif
  const bool previous_draw_mode=app->graphics.logical_drawing;
  // Keep one completed image per RAF and advance draw-dependent state on the extra ticks.
  app->graphics.logical_drawing=(previous_draw_mode||
      (!spectator&&!playing_replay&&app->graphics.backend.presentation_pending()))&&app->can_rollback();
#if TH17_ROOM_BENCHMARK
  room_benchmark_route_defeats=0;
#endif
  const bool ok=app->step(main_input.held,main_input.pressed,main_input.repeated,60,false);
  const bool logical=app->graphics.logical_drawing;
  app->graphics.logical_drawing=previous_draw_mode;
  app->room_sounds=nullptr;
  if(!ok){if(app->room_barrier)barrier=number;return false;}
  if(!logical&&!spectator&&!playing_replay&&app->scene()){
   auto& battle=app->scene()->battle;
   const auto& pos=battle.pilot(setup.local).motion.position;
   eagler::presentation::local_player_cue(setup.local,pos.x+224,pos.y+16);
  }
  output.score=app->scene()?u32(app->scene()->battle.score.score):u32(app->selection_score.score);
  if(app->scene())for(unsigned slot=0;slot<3;++slot)
   if(app->scene()->battle.enemy_world.boss_ids[slot]&&app->scene()->battle.enemy_world.boss_ids[slot]!=bosses_before[slot])
    output.publication.bossSlots|=u8(1u<<slot);
  if(barrier==number)barrier=Netplay::INVALID_FRAME;
  if(trace){
   output.snapshot=status(number+1);
   if(logical)++logical_draws;
  }
  if(previous_draw_mode&&logical){
   const double cost=std::clamp(emscripten_get_now()-replay_step_start,.25,16.);
   logical_step_ms+=(cost-logical_step_ms)*.25;
  }
  if(measure_draw&&app->graphics.backend.presentation_pending()){
   const double cost=std::clamp(emscripten_get_now()-tick_start,.5,16.);
   tick_cost_ms+=(cost-tick_cost_ms)*.25;
  }
  return true;
 }
 bool poll(bool capture_follows=false,double input_time=-1){
  const RoomPhaseTimer timing{Input};
  const double now=emscripten_get_now();
  now_ms=std::uint64_t(now);
  // Input cadence follows the refresh clock. Use the live clock for the
  // transport so callback jitter does not change packet and retry timings.
  session.capture_phase(entered&&!can_predict(),std::uint64_t((input_time>=0?input_time:now)*1000),app->touch.mode==2);
  replay_deadline=now+8;
  if(spectator)return receive_spectator();
  if(playing_replay)return true;
  if(local_results){session.pump(now_ms);publish(true);return true;}
  return session.pump(now_ms,capture_follows&&!terminal)||app->fail(session.error());
 }
 bool capture_physical(u32 held,const PlayerTouch& touch){
  Netplay::FrameInput sample(u16(held&0x3fbu));
#if TH17_ROOM_BENCHMARK
  if(setup.difficulty==4){
   // Use the physical capture index so real network pacing and corrections
   // do not change the input sequence between benchmark builds.
   const u32 n=benchmark_input_frame++;
   const u32 seat=setup.local;
   const bool move=seat==1&&n>=600&&!(n>=900&&n<952);
   const bool left=(seat==0&&n>=120&&n<170)||(move&&n%96>=48&&n%96<60)||(seat==1&&n>=900&&n<952);
   const bool right=(seat==1&&n>=120&&n<170)||(seat==0&&n>=300&&n<350)||(move&&n%96<12);
   const bool up=seat==2&&n>=120&&n<150;
   const bool fire=(n>=140&&n<260)||(n>=900&&n%240<180);
   const bool skip=seat==0&&n>=1400&&n%120<100;
   const bool bomb=seat==1&&n>=185&&n<193;
   sample=Netplay::FrameInput(u16(fire|bomb*2|up*0x10|left*0x40|right*0x80|skip*0x200));
   return session.capture_input(sample,now_ms)||app->fail(session.error());
  }
#endif
  if(touch.mode){
   sample.analogMode=Netplay::AnalogMode::DirectTouch;
   sample.x=touch.x;sample.y=touch.y;
   sample.unlimited=touch.mode==2;
  }
  return session.capture_input(sample,now_ms)||app->fail(session.error());
 }
 bool update(bool advance=true){
  if(spectator||playing_replay)return advance_playback(advance);
  if(local_results)return true;
  if(!session.flush_inputs(now_ms))return app->fail(session.error());
  const auto result=terminal?multiplayer::Session::Tick::Waiting:session.tick(*this,advance);
  if(trace&&result==multiplayer::Session::Tick::Waiting&&session.repair_pending())++replay_slices;
  if(result==multiplayer::Session::Tick::Failed)return app->fail(session.error());
  if(discarded_images&&!session.repair_pending()){
   for(auto& frame:history)if(frame.number==Netplay::INVALID_FRAME)clear_image(frame);
   discarded_images=false;
  }
  // The opening loader keeps the outgoing title alive until it constructs Stage 1.
  const bool returned_to_title=app->title&&app->loading_destination==AppScene::Uninitialized;
  if(entered&&(returned_to_title||app->exiting)&&session.confirmed_frames()==session.next_frame()){
   terminal=true;
   if(!session.retire(now_ms))return !*session.error()||app->fail(session.error());
   if(app->title&&app->flow.title_return_reason==3)app->completed_recording=true;
   local_results=true;
   if(!save_replay(-1,nullptr,app->completed_recording))return false;
   publish(true);
  }
  return true;
 }
};
std::unique_ptr<RoomRuntime> room;
#include "RoomPlayback.inc"
RoomRuntime::Status RoomRuntime::status(u32 number)const{
 Status result{};
 result[0]=number;result[1]=setup.seats;result[2]=setup.local;
 if(const auto* scene=app->scene()){
  auto& battle=scene->battle;
  result[3]=u32(battle.stage);result[4]=u32(scene->controller->counters[0]);
  result[5]=u32(battle.items->roaring.state.count);result[6]=u32(battle.items->roaring.state.countdown.current);
  result[7]=app->environment.game_rng.seed;result[8]=app->environment.game_rng.calls;
  result[9]=battle.enemies->count();result[10]=battle.bullet_scene->manager.visible_count;result[11]=u32(battle.score.score);
  for(u32 seat=0;seat<battle.pilot_count();++seat){
   const auto& pilot=battle.pilot(seat);
   result[12+seat*4]=u32(pilot.motion.position_fixed.x);result[13+seat*4]=u32(pilot.motion.position_fixed.y);
   result[14+seat*4]=u32(pilot.session.extra_lives);result[15+seat*4]=u32(pilot.session.bombs);
   result[24+seat]=u32(pilot.session.power);
   if(battle.pilot_ghost(seat))result[43]|=1u<<seat;
#if TH17_ROOM_BENCHMARK
   result[51+seat]=u32(pilot.session.deaths);
#endif
  }
  result[27]=battle.items->items.active_count;result[28]=battle.items->spirits.count();
  result[29]=battle.laser_scene->manager.count();result[30]=battle.spell.flags;result[31]=app->visual.calls;
  result[33]=battle.dialogue_present();result[34]=battle.items->roaring.state.flags;
  result[35]=u32(battle.items->roaring.visuals.hit_delay);
  result[36]=float_to_bits(battle.frame_state().rate);result[37]=app->visual.seed;
  result[38]=battle.enemy_world.boss_at(0)||battle.enemy_world.boss_at(1)||battle.enemy_world.boss_at(2);
  if(!trace)return result;
  result[39]=u32(battle.spell.id);
  result[40]=u32(battle.spell.frame);
  for(i32 index=0;index<3;++index)if(const auto* boss=battle.enemy_world.boss_at(index)){
   result[41]=boss->id;
   result[42]=u32(battle.enemies->find(boss->id)->scripts.at(0)->routine);
   break;
  }
  // The room fixture compares entity and script state on each confirmed frame.
  u32 hash=2166136261u;
  const auto word=[&](u32 value){hash=(hash^value)*16777619u;};
  const auto position=[&](const Vec3& p){word(float_to_bits(p.x));word(float_to_bits(p.y));word(float_to_bits(p.z));};
  for(const auto* e:battle.enemy_world.enemies){
   word(e->id);position(e->motion.position);word(e->flags);word(u32(e->life));
   auto& scripts=battle.enemies->find(e->id)->scripts;
   for(u32 n=0;n<scripts.count();++n){
    const auto* code=scripts.at(n);word(u32(code->routine));word(u32(code->instruction_offset));word(float_to_bits(code->time));
   }
  }
  auto& bullets=battle.bullet_scene->manager;
  for(u32 n=bullets.first_active();n!=BulletManager::none;n=bullets.next_active(n)){
   const auto& b=*bullets.state(n);word(n);position(b.motion.position);word(b.flags);word(u32(b.phase));word(u32(b.transform_index));
  }
  battle.laser_scene->manager.each([&](const LaserObject& laser){word(laser.id);word(laser.flags);word(u32(laser.age.current));});
  const auto& item_manager=battle.items->items;
  for(u32 i=item_manager.next_used(0);i<ItemManager::pool_size;i=item_manager.next_used(i+1)){
   const auto& item=item_manager.find(i+1)->state;
   if(item.state){word(i+1);word(u32(item.kind));word(item.gift_seat());position(item.position);word(u32(item.age.current));}
  }
  for(u32 seat=0;seat<battle.pilot_count();++seat){
   const auto& p=battle.pilot(seat);word(u32(p.session.life_pieces));word(u32(p.session.bomb_pieces));word(u32(p.life.state));
   for(const auto& shot:p.shots.shots)if(shot.state){
    word(u32(shot.index));position(shot.motion.position);word(u32(shot.state));
#if TH17_ROOM_BENCHMARK
    ++result[44+seat];
#endif
   }
  }
#if TH17_ROOM_BENCHMARK
  result[47]=app->animations.registry.count();
  result[48]=u32(std::max(0,battle.enemy_world.shot_damage));
  result[49]=u32(std::max(0,battle.enemy_world.chapter_defeated));
  result[50]=room_benchmark_route_defeats;
#endif
  result[32]=hash;
 }
 return result;
}
#endif
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
eagler::thprac::PracticePointer practice_pointers;
std::optional<int> imgui_touch;
bool practice_fast=false,practice_slow=false,practice_debug=false;
double previous_period=1./60.;
// Submit the nearest 60 Hz tick, retaining signed phase error. A fixed 1 ms
// epsilon still dropped one in six presentations when RAF phase jitter crossed
// that threshold. Half a tick covers both sides of the nearest-tick boundary
// without accumulating extra simulation time or counting empty callbacks.
touhou::sdl::FrameCadence cadence{touhou::sdl::FrameCadence::interval/2.};
GameInput controls;
SDL_Joystick* controller=nullptr;
bool running=false,suspended=false,lost_focus=false,music_enabled=true;u32 render_scale=2;
u32 loop_epoch=0;
double previous_frame=-1,window_start=0;
u32 window_ticks=0;
float measured_fps=60;
// Presentation counts committed browser frames, independently of simulation
// ticks and replay acceleration. Keep recorded FPS on its existing clock.
double presentation_start=-1;u32 presentation_frames=0;float presentation_fps=60;
void reset_presentation_clock(){presentation_start=-1;presentation_frames=0;presentation_fps=60;}
void add_controller(SDL_JoystickID id){if(!controller)controller=SDL_OpenJoystick(id);}
void clear_inputs(){th17_browser_keyboard_reset();for(auto& key:keyboard_map)key.hosted=false;SDL_ResetKeyboard();gestures.reset();practice_pointers.reset();imgui_touch.reset();ThpracUi::reset_input();practice_fast=practice_slow=practice_debug=false;controls={};if(app){app->keyboard_state={};app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
void initialize_controller(){if(controller){SDL_CloseJoystick(controller);controller=nullptr;}SDL_InitSubSystem(SDL_INIT_JOYSTICK);int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
touhou::input::TouchState touch_state(){
 touhou::input::TouchState value;
 if(app&&app->ending){value.context=2;return value;}
 if(app&&app->title)value.instance=1+int(app->title->state.screen)*256+app->title->state.substate;
 if(!app||!app->scene()||!app->run_input)return value;
 if(app->pause&&app->pause->state.screen!=PauseScreen::Inactive)return value;
 if(app->progress.practice_record){value.context=3;return value;}
 auto& stage=*app->scene();
 u32 seat=0;
#if TH17_MULTIPLAYER
 if(room)seat=room->setup.local;
#endif
 auto& player=stage.battle.pilot(seat);auto& motion=player.motion;
 value.context=stage.battle.dialogue_present()?2:1;
 value.instance=app->progress.stage;value.ready=player.life.state==1&&!(app->progress.scene_flags&(4|0x70|0x4000|0x10000));
 value.x=motion.position.x;value.y=motion.position.y;value.fast=float(motion.normal_speed)/128;value.slow=float(motion.focus_speed)/128;
 value.min_x=-184;value.max_x=184;value.min_y=32;value.max_y=432;return value;
}
u32 sample_controller(u32& buttons){
 buttons=0;if(!controller)return 0;
 for(int i=0;i<std::min(32,SDL_GetNumJoystickButtons(controller));i++)if(SDL_GetJoystickButton(controller,i))buttons|=1u<<i;
 u32 keys=0;constexpr u32 actions[]={1,2,8,0x100};for(u32 i=0;i<4;i++){const i32 binding=app->controller.values[i];if(binding>=0&&(buttons&(1u<<(u32(binding)&31))))keys|=actions[i];}
 const float x=SDL_GetNumJoystickAxes(controller)>0?float(SDL_GetJoystickAxis(controller,0))/32768:0,y=SDL_GetNumJoystickAxes(controller)>1?float(SDL_GetJoystickAxis(controller,1))/32768:0;
 if(x<-.5f)keys|=0x40;if(x>.5f)keys|=0x80;if(y<-.5f)keys|=0x10;if(y>.5f)keys|=0x20;
 const auto hat=SDL_GetNumJoystickHats(controller)>0?SDL_GetJoystickHat(controller,0):0;
 if(hat&SDL_HAT_UP)keys|=0x10;if(hat&SDL_HAT_DOWN)keys|=0x20;if(hat&SDL_HAT_LEFT)keys|=0x40;if(hat&SDL_HAT_RIGHT)keys|=0x80;return keys;
}
bool sample_and_tick(double input_time=-1,bool capture_only=false){
 if(!app)return false;SDL_Event event;while(SDL_PollEvent(&event)){
  ThpracUi::process_event(event,float(render_scale));
  if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST){clear_inputs();lost_focus=!th17_host_has_focus();}
  else if(event.type==SDL_EVENT_JOYSTICK_ADDED)add_controller(event.jdevice.which);
  else if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&controller&&SDL_GetJoystickID(controller)==event.jdevice.which){SDL_CloseJoystick(controller);controller=nullptr;int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
 }
#if TH17_MULTIPLAYER
 if(room){
  if(!room->poll(true,input_time))return false;
  if(!room->local_results){
   if(room->spectator||room->playing_replay)return room->update();
   if(room->terminal)return room->update(false);
   if(!room->session.needs_input())return capture_only?room->session.flush_inputs(room->now_ms):room->update();
  }
 }
#endif
 #if TH17_MULTIPLAYER
 if(app->room_preview){
  if(!app->title||app->run)return false;
  app->title->state.room_preview=true;app->title->frame.demo_idle=0;
 }
 #endif
 bool keys[256]{};const auto* physical=SDL_GetKeyboardState(nullptr);const bool browser_keyboard=th17_browser_keyboard_owned();
 for(const auto& key:keyboard_map)if(key.hosted||(!browser_keyboard&&key.native!=SDL_SCANCODE_UNKNOWN&&physical[key.native])){if(key.vk<256)keys[key.vk]=true;if(key.vk>=160&&key.vk<=165)keys[16+(key.vk-160)/2]=true;if(key.scan==28||key.scan==156)keys[13]=true;}
 const int mappedPad=eagler::input::BrowserGamepadState();eagler::input::ApplyGamepadVirtualKeys(keys,mappedPad);
 app->keyboard_state.format=2;for(u32 i=0;i<256;i++)app->keyboard_state.keys[i]=keys[i]?128:0;
 const auto sample=gestures.sample(touch_state(),SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
 for(u32 i=0;i<256;i++)keys[i]=keys[i]||sample.keys[i];app->touch={sample.motion,sample.x,sample.y};
 u32 buttons=0;
 const u32 controller_keys=mappedPad<0?sample_controller(buttons):0;
 ThpracUi::update_input(*app,keys);
 practice_fast=keys[85];practice_slow=keys[16];practice_debug=keys[32];
 u32 held=keyboard_keys(keys)|controller_keys;
 if(app->practice.enabled&&app->scene()&&!app->progress.practice_record){
  std::array<u8,256> virtual_keys{};
  for(u32 i=0;i<256;++i)virtual_keys[i]=keys[i]?0x80:0;
  if(held&1)virtual_keys[90]=0x80;
  if(held&2)virtual_keys[88]=0x80;
  if(held&8)virtual_keys[160]=0x80;
  app->practice.input.apply(virtual_keys.data());
  virtual_keys[16]=virtual_keys[160]|virtual_keys[161];
  for(u32 i=0;i<256;++i)keys[i]=(virtual_keys[i]&0x80)!=0;
  held=keyboard_keys(keys)|(controller_keys&~0xbu);
 }
 if(ThpracUi::captures_game_input(*app)){held=buttons=0;app->touch={};}
#if TH17_MULTIPLAYER
 if(room&&!room->local_results){
  if(room->capture_physical(held,app->touch))return capture_only?room->session.flush_inputs(room->now_ms):room->update();
  room->session.flush_inputs(room->now_ms);
  return false;
 }
#endif
 controls.update(held);app->controller_buttons=buttons;
 const bool focus=lost_focus;lost_focus=false;
 const bool ok=app->step(controls.held,controls.pressed,controls.repeated,measured_fps,focus);
 const double now=double(SDL_GetTicksNS())/1e9;if(!window_start)window_start=now;
 if(ok){window_ticks++;const double duration=now-window_start;if(duration>=1){measured_fps=float(window_ticks/duration);window_ticks=0;window_start=now;}}
 return ok;
}
#if TH17_MULTIPLAYER
// A slow lane still samples only one real input for each immutable slot.
// Use a timer because a 60 Hz page has no intervening rAF to send that slot.
void queue_early_room_capture(double next_tick){
 if(!room||room->spectator||room->playing_replay||room->local_results||room->terminal||
    app->touch.mode!=2||!room->can_predict()||!room->session.can_capture_early())return;
 const double now=emscripten_get_now();
 if(next_tick-now<4)return;
 static uintptr_t token=0;
 room->early_capture_token=++token;
 room->early_capture_head=room->session.prediction_head();
 room->early_capture_until=next_tick-1;
 emscripten_async_call([](void* cookie){
  const double capture_time=emscripten_get_now();
  if(!running||suspended||!room||room->early_capture_token!=uintptr_t(cookie)||
     capture_time>room->early_capture_until||room->session.prediction_head()!=room->early_capture_head||
     room->terminal||room->local_results||!room->can_predict()||
     room->session.repair_pending()||!room->session.can_capture_early())return;
  if(!sample_and_tick(capture_time,true)){
   running=false;
   th17_browser_frame(0,0,0,0);
  }
 },reinterpret_cast<void*>(token),int(std::max(1.,next_tick-now-8.)));
}
#endif
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th17_initialize(){using namespace th17::sdl;if(running)return 1;clear_inputs();if(controller){SDL_CloseJoystick(controller);controller=nullptr;}ThpracUi::shutdown();app.reset();app=std::make_unique<ApplicationState>();app->graphics.render_scale=render_scale;app->audio_device.music_enabled=music_enabled;if(!app->initialize(true)||!ThpracUi::initialize(*app)){last_error=app->failure;app.reset();return 0;}reset_presentation_clock();for(auto& key:keyboard_map)key.native=SDL_GetScancodeFromName(key.sdl);initialize_controller();window_start=0;window_ticks=0;measured_fps=60;return 1;}
#if TH17_MULTIPLAYER
EMSCRIPTEN_KEEPALIVE int th17_room_preview(){
 using namespace th17::sdl;
 if(running||room||!app||!app->title)return 0;
 app->room_preview=app->title->state.room_preview=true;
 // Settle the entrance before handing animation and Options to the preview loop.
 app->graphics.backend.defer=true;
 bool ok=true;
 for(unsigned frame=0;frame<120&&ok;++frame)
  ok=app->title&&app->step(0,0,0,60,false);
 const bool presented=app->graphics.backend.commit();
 app->graphics.backend.defer=false;
 return ok&&presented&&app->title;
}
EMSCRIPTEN_KEEPALIVE int th17_room_open(const unsigned* words,unsigned count,const char* url,const char* spectator_id){
 using namespace th17::sdl;
 if(!app||!app->title||running||room||!words||!url)return 0;
 const bool legacy=count==19&&words[0]==2;
 if((!legacy&&(count!=20||words[0]!=3||words[19]>1))||
    words[1]>3||words[2]>=words[1]||words[3]>4||words[4]>65535||words[7]>9||words[18]>1||
    (words[18]&&words[7]))return 0;
 auto candidate=std::make_unique<RoomRuntime>();
 candidate->spectator=spectator_id&&*spectator_id;
 auto& setup=candidate->setup;
 setup.seats=std::uint8_t(words[1]);setup.local=std::uint8_t(words[2]);
 setup.difficulty=std::uint8_t(words[3]);setup.seed=th17::u16(words[4]);
 setup.room=words[5]|(std::uint64_t(words[6])<<32);
 setup.delay=std::uint8_t(words[7]);setup.rollback=words[18]!=0;
 setup.challenge=!legacy&&words[19]!=0;
 for(unsigned seat=0;seat<3;++seat){
  if(words[8+seat*2]>2||words[9+seat*2]>2)return 0;
  setup.loadouts[seat]={std::uint8_t(words[8+seat*2]),std::uint8_t(words[9+seat*2])};
 }
 for(unsigned i=0;i<4;++i)setup.build[i]=words[14+i];
 if(!setup.valid())return 0;
 if(candidate->spectator){
  if(!candidate->transport.ConnectSpectator(url,spectator_id,setup.seats,true))return 0;
  candidate->spectator_deadline=std::uint64_t(emscripten_get_now())+90000;
 }else{
  if(!candidate->transport.Connect(url,setup.local,setup.seats,true)||
     !candidate->session.begin(setup,std::uint64_t(emscripten_get_now()))||
     !candidate->replay.begin(setup,std::uint64_t(std::time(nullptr))))return 0;
  candidate->transport.BeginSpectatorSession(setup.room,setup.seed);
 }
 room=std::move(candidate);
 app->frames=0;
 app->room_preview=false;app->title->state.room_preview=false;
 app->room_replay_save=[](th17::i32 slot,const std::array<char,9>& name,bool completed){
  return room&&room->save_replay(slot,name.data(),completed);
 };
 return 1;
}
EMSCRIPTEN_KEEPALIVE int th17_room_return(){
 using namespace th17::sdl;
 if(running||!app)return 0;
 room.reset();app->room_replay_save={};app->room_seats=0;app->room_challenge=false;app->room_partners={};
 app->flow.pending=th17::AppScene::Title;
 if(!app->release_ending()||!app->release_game()||!app->release_title())return 0;
 app->recording.reset();app->room_description.reset();app->selected_replay.reset();
 app->exiting=false;app->loading_destination=th17::AppScene::Uninitialized;app->loading_handle=app->transition_overlay=0;
 app->progress={};app->selection={};app->physical_input={};app->retained_input={};
 app->flow.current=app->flow.pending=th17::AppScene::Title;app->flow.title_return_reason=1;
 app->room_preview=true;clear_inputs();
 return app->begin_title();
}
EMSCRIPTEN_KEEPALIVE int th17_mp_replay_save(){using namespace th17::sdl;return !room||room->save_replay(-1,nullptr,app->completed_recording);}
EMSCRIPTEN_KEEPALIVE int th17_mp_replay_validate(const unsigned char* bytes,unsigned count){
 th17::multiplayer::ReplayArchive replay;return replay.load(bytes,count);
}
EMSCRIPTEN_KEEPALIVE const char* th17_mp_spectator_state(){
 using namespace th17::sdl;return room&&room->spectator?room->spectator_view.State(room->playback):nullptr;
}
EMSCRIPTEN_KEEPALIVE int th17_mp_spectator_control(const char* run,const char* action,unsigned frame){
 using namespace th17::sdl;
 return room&&room->spectator&&room->spectator_view.Control(room->playback,run,action,frame);
}
EMSCRIPTEN_KEEPALIVE int th17_mp_spectator_result(const unsigned char* bytes,unsigned count){
 using namespace th17::sdl;
 if(!room||!room->spectator||!room->playback.History().Ended()||room->playback.Paused()||
    room->playback.Seeking()||room->playback.Position()<room->playback.History().Received())return 0;
 th17::multiplayer::ReplayArchive replay;
 if(!replay.load(bytes,count)||replay.setup().room!=room->setup.room||
    replay.setup().gameplay_abi()!=room->setup.gameplay_abi())return 0;
 room->replay=std::move(replay);room->confirmed_score=room->replay.score();room->local_results=true;
 app->room_replay_save=[](th17::i32 slot,const std::array<char,9>& name,bool completed){
  return room&&room->save_replay(slot,name.data(),completed);
 };
 app->completed_recording=room->replay.completed();
 return app->prepare_live_replay(app->completed_recording)&&app->finish_practice();
}
EMSCRIPTEN_KEEPALIVE void th17_room_close(){th17::sdl::room.reset();}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_status(){
 using namespace th17::sdl;
 static RoomRuntime::Status value{};
 value=room&&app?room->status(room->spectator?room->playback.Position():room->session.next_frame()):RoomRuntime::Status{};
 return value.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_confirmed(unsigned number){
 using namespace th17::sdl;
 static RoomRuntime::Status empty{};
 if(!room)return empty.data();
 if(!number)number=room->session.confirmed_frames();
 const auto& value=room->confirmed[number%room->confirmed.size()];
 return value[0]==number?value.data():empty.data();
}
EMSCRIPTEN_KEEPALIVE void th17_room_trace(unsigned enabled){
 if(th17::sdl::room)th17::sdl::room->trace=enabled!=0;
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_phase_costs(unsigned enabled){
 using namespace th17::sdl;
 room_phase_trace=enabled!=0;
 static std::array<unsigned,RoomPhaseCount+3> costs{};
 std::copy(room_phase_us.begin(),room_phase_us.end(),costs.begin());
 std::copy(room_application_us.begin(),room_application_us.end(),costs.begin()+RoomPhaseCount);
 return costs.data();
}
#if TH17_ROOM_BENCHMARK
EMSCRIPTEN_KEEPALIVE int th17_room_fixture_story_stage(unsigned stage){
 using namespace th17::sdl;
 if(!room||room->setup.difficulty>=4||room->entered||stage<1||stage>6)return 0;
 room->benchmark_story_stage=th17::i32(stage);
 return 1;
}
EMSCRIPTEN_KEEPALIVE int th17_room_fixture_ghost(unsigned seat){
 using namespace th17::sdl;
 if(!room||room->setup.difficulty>=4||seat>=room->setup.seats||room->session.next_frame()>=800)return 0;
 room->benchmark_ghost_seat=seat;
 return 1;
}
#endif
EMSCRIPTEN_KEEPALIVE unsigned th17_room_wait_state(){
 using namespace th17::sdl;
 if(!room||!app)return 0;
 const auto* scene=app->scene();
 return (room->session.repair_pending()?1u:0u)|
        (room->barrier==room->session.next_frame()?2u:0u)|
        (!app->can_rollback()?4u:0u)|
        (scene&&scene->battle.dialogue_present()?8u:0u);
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_input_frontiers(){
 using namespace th17::sdl;
 static std::array<unsigned,7> value{};
 value=room?room->session.input_frontiers():std::array<unsigned,7>{};
 return value.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_performance(){
 using namespace th17::sdl;
 static std::array<unsigned,7> value{};
 value=room?std::array<unsigned,7>{room->capture_count,room->capture_us,room->replay_slices,room->logical_draws,
  room->phase_corrections[0],room->phase_corrections[1],room->phase_corrections[2]}:std::array<unsigned,7>{};
 return value.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_load(){
 using namespace th17::sdl;
 static std::array<unsigned,2> value{};
 const auto* scene=app?app->scene():nullptr;
 value=scene&&room&&room->trace?std::array<unsigned,2>{
  scene->battle.bullet_scene->manager.visible_count,unsigned(mallinfo().uordblks)}:std::array<unsigned,2>{};
 return value.data();
}
#if TH17_ROOM_BENCHMARK
EMSCRIPTEN_KEEPALIVE unsigned th17_room_benchmark_story_status_size(){return th17::sdl::RoomRuntime::Status{}.size();}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_benchmark_draw(){return th17::sdl::room_benchmark_draw.data();}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_benchmark_history(){
 static const std::array<unsigned,4> empty{};
 return th17::sdl::room?th17::sdl::room->benchmark_history.data():empty.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_benchmark_memory(){
 using namespace th17::sdl;
 using Image=th17::multiplayer::FrameImage;
 static std::array<unsigned,7+Image::PayloadCount> value{};
 value.fill(0);
 const auto* scene=app?app->scene():nullptr;
 if(!room||!scene||!room->trace)return value.data();
 const auto& battle=scene->battle;
 value[0]=room->session.next_frame();
 value[1]=battle.bullet_scene->manager.visible_count;
 value[2]=battle.spell.flags&1?unsigned(battle.spell.id):UINT32_MAX;
 value[3]=unsigned(mallinfo().uordblks);
 value[4]=battle.laser_scene->manager.count();
 for(const auto& frame:room->history){
  if(frame.image.empty())continue;
  ++value[5];
  value[6]+=frame.image.unused_capacity;
  for(unsigned i=0;i<Image::PayloadCount;++i)value[7+i]+=frame.image.payload[i];
 }
 return value.data();
}
#endif
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_rollback(){
 using namespace th17::sdl;
 static std::array<unsigned,6> value{};
 value=room?std::array<unsigned,6>{room->session.next_frame(),room->session.confirmed_frames(),room->session.corrections(),room->session.resimulated_frames(),room->combat_corrections,room->spell_corrections}:std::array<unsigned,6>{};
 return value.data();
}
#endif
EMSCRIPTEN_KEEPALIVE void th17_always_hitbox(unsigned enabled){th17::presentation::always_hitbox=enabled!=0;}
EMSCRIPTEN_KEEPALIVE void th17_render_scale(unsigned scale){if(!th17::sdl::running)th17::sdl::render_scale=scale>=2?2:1;}
EMSCRIPTEN_KEEPALIVE const char* th17_error(){using namespace th17::sdl;return app?app->failure.c_str():last_error.c_str();}
EMSCRIPTEN_KEEPALIVE unsigned th17_phase(){
 using namespace th17::sdl;
#if TH17_MULTIPLAYER
 if(room&&room->terminal&&!room->local_results)return 2;
 if(room&&room->local_results&&app&&app->title&&app->title->state.screen==th17::TitleScreen::Main)return 4;
#endif
 return !app?4:app->exiting?4:app->title?0:app->ending?3:app->pause&&app->pause->state.screen!=th17::PauseScreen::Inactive?2:1;
}
EMSCRIPTEN_KEEPALIVE unsigned th17_frame(){using namespace th17::sdl;return app?app->frames:0;}
EMSCRIPTEN_KEEPALIVE int th17_save_scores(){using namespace th17::sdl;return !app||app->save_settings();}
EMSCRIPTEN_KEEPALIVE int th17_save_replay(unsigned slot,const char* name){using namespace th17::sdl;if(!app||!name||std::strlen(name)>8||slot>=25)return 0;std::array<char,9> text{};std::memcpy(text.data(),name,std::strlen(name));return app->prepare_live_replay(false)&&app->save_slot(slot,text);}
EMSCRIPTEN_KEEPALIVE int th17_validate_file(unsigned kind,const unsigned char* data,unsigned size){if(kind==0){th17::ScoreFile file;return file.open(data,size);}if(kind==1){th17::Replay file;return file.open(data,size);}if(kind==2){th17::GameConfig file;return file.open(data,size);}return 0;}
EMSCRIPTEN_KEEPALIVE void th17_key(unsigned scan,unsigned down){for(auto& key:th17::sdl::keyboard_map)if(key.scan==scan)key.hosted=down!=0;}
EMSCRIPTEN_KEEPALIVE void th17_keys_clear(){th17::sdl::clear_inputs();}
EMSCRIPTEN_KEEPALIVE void th17_music_enabled(unsigned enabled){using namespace th17::sdl;music_enabled=enabled!=0;if(app){app->audio_device.music_enabled=music_enabled;app->audio_device.refresh_volume();}}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_audio_statistics(){using namespace th17::sdl;return app?app->audio_device.statistics():nullptr;}
EMSCRIPTEN_KEEPALIVE void th17_audio_close(){using namespace th17::sdl;if(app)app->audio_device.close();}
EMSCRIPTEN_KEEPALIVE void th17_loop_stop(){using namespace th17::sdl;running=false;++loop_epoch;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();if(app)app->audio_device.suspend(true);}
EMSCRIPTEN_KEEPALIVE void th17_loop_pause(unsigned enabled){using namespace th17::sdl;suspended=enabled!=0;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();if(app){app->performance.rebase(double(SDL_GetTicksNS())/1e9);app->audio_device.suspend(suspended);}}
EMSCRIPTEN_KEEPALIVE void th17_loop_start(){
 using namespace th17::sdl;if(running||!app||!app->initialized)return;running=true;suspended=false;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();app->audio_device.suspend(false);
 emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
  if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
  if(suspended){cadence.reset();return EM_TRUE;}if(presentation_start<0)presentation_start=time;const double period=app->practice.enabled&&app->scene()?app->practice.speed.interval(app->progress.practice_record,practice_fast,practice_slow,practice_debug):touhou::sdl::FrameCadence::interval;
#if TH17_MULTIPLAYER
  if(room_phase_trace){room_phase_us.fill(0);room_application_us.fill(0);}
#endif
  if(period!=previous_period){cadence.reset();previous_period=period;}
  if(app->display)app->display->presentation_fps=presentation_fps;
  const auto ticks=cadence.advance(delta,period);bool ok=true;app->graphics.backend.defer=true;
  bool work=ticks!=0;
#if TH17_MULTIPLAYER
  work=work||(room&&room->session.repair_pending());
#endif
  if(work)th17_browser_frame_begin();unsigned completed=0;double predicted_tick=0;
  // RAF's timestamp and emscripten_get_now share the performance clock. Work
  // already done before this callback has consumed part of this refresh.
  const double deadline=begin+std::max(0.,1000./60.-std::max(0.,begin-time));
#if TH17_MULTIPLAYER
  if(room){room->refresh_start=begin;room->refresh_deadline=deadline;room->released_confirmed=false;}
#endif
  for(unsigned i=0;i<ticks&&ok;i++){
   const double tick_begin=emscripten_get_now();
#if TH17_MULTIPLAYER
   const bool live=room&&!room->spectator&&!room->playing_replay&&!room->local_results;
   const auto previous_head=live?room->session.prediction_head():0;
#endif
   ok=sample_and_tick(time);
   const double tick_end=emscripten_get_now();
#if TH17_MULTIPLAYER
   const bool advanced=live&&room&&room->session.prediction_head()>previous_head;
   if(!live||advanced)++completed;
   if(live&&room&&!room->terminal&&(room->session.repair_pending()||room->session.forward_deferred())){
    room->retry_forward|=room->session.forward_deferred();
    cadence.debt=std::min(.1,cadence.debt+(ticks-i-1)*period);
    break;
   }
#else
   ++completed;
#endif
   predicted_tick=std::max(predicted_tick,tick_end-tick_begin);if(app->exiting)break;
   // Check whether the NEXT complete update/draw fits, rather than noticing
   // an overrun only after it happened. Keep unexecuted ticks as short debt.
   if(i+1<ticks&&tick_end+predicted_tick>=deadline){cadence.debt=std::min(.1,cadence.debt+(ticks-i-1)*period);break;}
  }
#if TH17_MULTIPLAYER
  if(!ticks&&work)ok=room->poll(false,time)&&room->update(false);
  // Spend the single saved tick only on an already confirmed input. A
  // speculative catch-up would push the head forward and multiply repairs.
  if(ok&&ticks==1&&completed==1&&room&&room->retry_forward&&!room->terminal&&!room->session.repair_pending()&&
      room->session.has_confirmed_forward_input()&&emscripten_get_now()+room->tick_cost_ms+1<deadline){
   const auto head=room->session.prediction_head();
   room->retry_forward=false;
   ok=sample_and_tick(time);
   if(room){
    completed+=unsigned(room->session.prediction_head()>head);
    room->retry_forward=room->session.forward_deferred();
   }
  }
#endif
  if(ok&&completed)ok=ThpracUi::render(*app);
  bool presented;
  {
#if TH17_MULTIPLAYER
   const RoomPhaseTimer timing{Canvas};
#endif
   presented=app->graphics.backend.commit();app->graphics.backend.defer=false;
  }
  {
#if TH17_MULTIPLAYER
   const RoomPhaseTimer timing{Pcm};
#endif
   app->audio_device.pump();
  }
#if TH17_MULTIPLAYER
  if(room)room->release_retired(deadline);
  if(ok&&completed)queue_early_room_capture(time+(period-cadence.debt)*1000);
#endif
  if(ok&&presented)++presentation_frames;const double elapsed=time-presentation_start;if(elapsed>=1000){presentation_fps=float(presentation_frames*1000./elapsed);presentation_start=time;presentation_frames=0;}
  if(work||presented||elapsed>=1000)th17_browser_frame(ok?1:0,emscripten_get_now()-begin,completed,presented);
  if(!ok||app->exiting)running=false;return running?EM_TRUE:EM_FALSE;
 },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th17_touch(unsigned type,int id,float x,float y){
 using namespace th17::sdl;
 if(type>2||!std::isfinite(x)||!std::isfinite(y))return;
 const float px=x*640,py=y*480;
 using Route=eagler::thprac::PracticePointer::TouchRoute;
 if(type==0&&imgui_touch==id){imgui_touch.reset();ThpracUi::cancel_pointer();}
 const auto decision=practice_pointers.route_touch(type,id,type==0&&ThpracUi::captures_pointer(px,py),
     type==0&&ThpracUi::passive_pointer(px,py),px,py);
 if(decision.route==Route::Pending)return;
 if(decision.route==Route::Tap){
  if(!imgui_touch){ThpracUi::mouse(1,decision.x,decision.y);ThpracUi::mouse(2,px,py);}
  return;
 }
 if(decision.route==Route::Ui){
  if(type==0&&!imgui_touch)imgui_touch=id;
  if(imgui_touch==id){
   ThpracUi::mouse(type==0?1:type==2?2:0,px,py);
   if(type==2)imgui_touch.reset();
  }
  return;
 }
 const auto state=touch_state();const auto now=SDL_GetTicks();
 if(decision.route==Route::BeginGame)gestures.pointer(0,id,decision.x/640,decision.y/480,now,state,false);
 gestures.pointer(type,id,x,y,now,state,false);
}
EMSCRIPTEN_KEEPALIVE void th17_thprac_mouse(int type,float x,float y){th17::sdl::ThpracUi::mouse(type,x,y);}
EMSCRIPTEN_KEEPALIVE void th17_touch_cancel(){using namespace th17::sdl;practice_pointers.reset();imgui_touch.reset();gestures.reset();ThpracUi::cancel_pointer();if(app){app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
EMSCRIPTEN_KEEPALIVE void th17_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.mode=mode<=3?int(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,.25f,4.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th17_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th17_touch_stick(float x,float y){using namespace th17::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
EMSCRIPTEN_KEEPALIVE int th17_open_replay(const char* name,int stage){using namespace th17::sdl;if(running||!app||!app->title||!name||!app->files.replay(name,app->selected_replay)||!app->selected_replay)return 0;if(stage==0){for(int i=1;i<=7;i++)if(app->selected_replay->stage(i)){stage=i;break;}}th17::ReplayStageState entry;if(!app->selected_replay->initial_state(stage,entry))return 0;auto& p=app->progress;p.stage=p.initial_stage=stage;p.character=entry.run.character;p.animal=entry.run.animal;p.difficulty=entry.run.difficulty;p.spell_id=entry.run.spell_id;p.scoring.mode_flags=app->selected_replay->mode_flags()&2?0x20u:app->selected_replay->practice()?0x10u:0;p.demo_slot=-1;app->flow.pending=th17::AppScene::ReplayStart;return 1;}
#if TH17_DEVELOPMENT_HARNESS
// Browser regression observations; absent from release builds.
EMSCRIPTEN_KEEPALIVE const int* th17_probe_practice_state(){using namespace th17::sdl;static std::array<int,8> v{};v.fill(0);if(app)v={app->practice.enabled,app->practice.advanced,app->practice.information,app->practice.menu,int(app->keyboard_state.keys[8]),int(app->keyboard_state.keys[9]),int(app->keyboard_state.keys[123]),int(app->keyboard_state.keys[82])};return v.data();}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_transition_state(){using namespace th17::sdl;static std::array<int,8> v{};v.fill(-1);if(app&&app->scene()){const auto& s=*app->scene();const auto* p=app->run->previous_scene();v={s.battle.stage,s.controller->age.current,s.background_transition_age,p?p->background_transition_age:-1,s.controller->waiting_for_stage_clear(),p?int(p->background_flags):-1,int(s.background_flags),s.battle.hud->notice_age.current};}return v.data();}
EMSCRIPTEN_KEEPALIVE int th17_probe_finish_stage(){using namespace th17::sdl;return !running&&app&&app->scene()&&app->scene()->progression->next_stage();}
// Enter the supplied replay without menu automation. All subsequent ticks use
// the production input, audio, update, draw and RAF presentation paths.

EMSCRIPTEN_KEEPALIVE const int* th17_probe_complete_world(){using namespace th17::sdl;static std::array<int,24> v{};v.fill(0);if(app&&app->scene()){auto& s=*app->scene();auto& b=s.battle;v={s.controller->counters[0],b.player->motion.position_fixed.x,b.player->motion.position_fixed.y,b.player->life.state,b.session.extra_lives,b.session.life_pieces,b.session.bombs,b.session.bomb_pieces,b.session.power,b.session.deaths,b.score.score,b.score.point_value,b.graze_total,b.enemy_world.current_chapter,int(b.enemies->count()),int(b.bullet_scene->manager.visible_count),app->environment.game_rng.seed,int(app->environment.game_rng.calls),int(app->visual.seed),int(app->visual.calls),int(s.background.drawn_instances),int(s.background.culled_instances),int(s.background.drawn_primitives),int(s.background_flags)};}return v.data();}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* th17_probe_graphics_statistics(){using namespace th17::sdl;return app?&app->graphics.backend.stats:nullptr;}
EMSCRIPTEN_KEEPALIVE int th17_probe_tick(){return !th17::sdl::running&&th17::sdl::sample_and_tick();}
// Diagnostic replay batches retain every update/draw and defer only window presentation.
EMSCRIPTEN_KEEPALIVE int th17_probe_ticks(unsigned count){using namespace th17::sdl;if(running||!app||count>120)return 0;app->graphics.backend.defer=true;bool ok=true;for(unsigned i=0;i<count&&ok;i++)ok=sample_and_tick();app->graphics.backend.commit();app->graphics.backend.defer=false;return ok;}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_state(){using namespace th17::sdl;return app?app->projection():nullptr;}
EMSCRIPTEN_KEEPALIVE int th17_probe_dialogue(){return th17::sdl::touch_state().context==2;}
// On-demand development snapshot, never sampled by the production frame loop.
// Each record identifies a bank/script/layer/mode and its live instance count.
EMSCRIPTEN_KEEPALIVE const unsigned* th17_probe_animation_load(){using namespace th17::sdl;static std::vector<unsigned> result;result.assign(3,0);if(!app)return result.data();auto& registry=app->animations.registry;result[1]=registry.count();for(unsigned layer=0;layer<44;layer++)for(auto* vm:registry.layer(layer)){const unsigned bank=unsigned(app->animations.resource_id(vm->resource)),script=unsigned(vm->source_script),mode=vm->visual.draw_mode();size_t at=3;for(;at<result.size();at+=5)if(result[at]==bank&&result[at+1]==script&&result[at+2]==layer&&result[at+3]==mode)break;if(at==result.size())result.insert(result.end(),{bank,script,layer,mode,0});++result[at+4];++result[2];}result[0]=(result.size()-3)/5;return result.data();}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_probe_world_state(){using namespace th17::sdl;static std::array<unsigned,16> v{};v.fill(0);if(app&&app->scene()){auto& b=app->scene()->battle;const auto& p=*b.player;v={unsigned(app->progress.stage),unsigned(app->scene()->controller->counters[0]),b.active_frame().held,th17::float_to_bits(p.motion.position.x),th17::float_to_bits(p.motion.position.y),unsigned(p.life.state),unsigned(b.score.score),unsigned(b.session.deaths),unsigned(b.session.extra_lives),unsigned(b.session.power),unsigned(b.session.bombs),app->environment.game_rng.seed,app->environment.game_rng.calls,b.enemies->count(),b.bullet_scene->manager.visible_count,app->progress.scene_flags};}return v.data();}
EMSCRIPTEN_KEEPALIVE const float* th17_probe_player_state(){using namespace th17::sdl;static std::array<float,10> value{};value.fill(0);if(app&&app->scene()){const auto& p=*app->scene()->battle.player;value={p.motion.position.x,p.motion.position.y,float(p.life.state),float(p.motion.input_frame),float(p.motion.normal_speed)/128,float(p.motion.focus_speed)/128,float(app->scene()->battle.active_frame().held),float(p.motion.touch.mode),p.motion.touch.x,p.motion.touch.y};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_spell_state(){using namespace th17::sdl;static std::array<int,6> value{};value.fill(0);if(app&&app->scene()){const auto& b=app->scene()->battle;const auto& c=b.spell_card->result_clock;value={int(b.spell.flags),b.spell.id,b.spell_card->active_frames,c.completed,c.encoded_time,0};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_hud_score(){using namespace th17::sdl;static std::array<int,6> value{};value.fill(0);if(app&&app->scene()){const auto& s=*app->scene();value={s.battle.score.score,s.battle.hud->displayed_score,s.battle.hud->score_increment,app->progress.scoring.high_score,app->progress.scoring.high_score_extra,int(app->progress.scoring.mode_flags)};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_pause_state(){using namespace th17::sdl;static std::array<int,7> value{};value.fill(-1);if(app&&app->pause){const auto& p=app->pause->state;value={int(p.screen),p.phase,p.menu.selected,p.menu.count,p.names.selected,p.name_length,p.age.current};}return value.data();}
#endif
}
#if TH17_MULTIPLAYER
#include "RoomReplayWorkspace.inc"
#endif
