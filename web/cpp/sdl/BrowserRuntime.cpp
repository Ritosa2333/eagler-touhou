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
#include "../multiplayer/Session.hpp"
#include "../multiplayer/FrameImage.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#endif
EM_JS(void, th17_browser_frame, (int ok,double milliseconds,unsigned ticks), { Module["onGameFrame"]?.(ok,milliseconds,ticks); });
EM_JS(void, th17_browser_frame_begin, (), { Module["onGameFrameStart"]?.(); });
EM_JS(int, th17_host_has_focus, (), {
 try { return parent.document.hasFocus() && !parent.document.hidden ? 1 : 0; }
 catch (_) { return document.hasFocus() && !document.hidden ? 1 : 0; }
});
EM_JS(int, th17_browser_keyboard_owned, (), { return typeof Module.resetBrowserKeyboard === "function"; });
EM_JS(void, th17_browser_keyboard_reset, (), { Module.resetBrowserKeyboard?.(); });
namespace th17::sdl {
namespace {
std::unique_ptr<ApplicationState> app;
std::string last_error;
#if TH17_MULTIPLAYER
struct RoomRuntime final : multiplayer::World {
 using Status=std::array<u32,38>;
 struct Frame {
  u32 number=Netplay::INVALID_FRAME;
  multiplayer::FrameImage image;
  std::vector<ApplicationState::RoomSound> sounds;
  Status snapshot{};
 };
 std::array<Frame,multiplayer::Session::rollback_frames+1> history;
 std::array<Status,16> confirmed{};
 Netplay::BrowserPeerTransport transport;
 multiplayer::Session session{transport};
 multiplayer::SessionSetup setup;
 GameInput main_input;
 bool entered=false;
 u32 barrier=Netplay::INVALID_FRAME;
 u32 combat_corrections=0,spell_corrections=0;

 Frame& state(u32 frame){return history[frame%history.size()];}
 Status status(u32 number) const;
 bool can_predict() const override{return entered&&barrier!=session.next_frame()&&app->can_rollback();}
 void speculating(bool active) override{app->room_speculative=active;}
 bool blocked() const override{return app->room_barrier;}
 bool capture(u32 frame) override{
  auto& saved=state(frame);saved.number=frame;saved.image.clear();saved.image.save(main_input);
  return app->save_rollback(saved.image);
 }
 bool restore(u32 frame) override{
  auto& saved=state(frame);
  if(saved.number!=frame)return false;
  if(frame<session.next_frame()){
   const auto& battle=app->scene()->battle;
   if(battle.bullet_scene->manager.visible_count||battle.laser_scene->manager.count())++combat_corrections;
   if(battle.spell.flags&1)++spell_corrections;
  }
  if(!saved.image.restore())return false;
  for(auto& future:history)if(future.number!=Netplay::INVALID_FRAME&&future.number>=frame){
   future.image.clear();future.sounds.clear();future.number=Netplay::INVALID_FRAME;
  }
  if(barrier!=Netplay::INVALID_FRAME&&barrier>frame)barrier=Netplay::INVALID_FRAME;
  return true;
 }
 bool confirm(u32 frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>&) override{
  auto& saved=state(frame);
  if(saved.number!=frame)return app->fail("TH17 confirmed frame output is missing");
  for(const auto& sound:saved.sounds)if(!app->audio(sound.id,sound.pan,sound.action,sound.queued))return false;
  app->audio_device.update();
  confirmed[(frame+1)%confirmed.size()]=saved.snapshot;
  saved.image.clear();saved.sounds.clear();saved.number=Netplay::INVALID_FRAME;
  return app->audio_device.error.empty()||app->fail(app->audio_device.error);
 }
 bool step(u32 number,const Netplay::FrameDecision& frame) override{
  app->room_barrier=false;
  if(!entered){
   std::array<std::array<i32,2>,3> loadouts{};
   for(u32 seat=0;seat<setup.seats;++seat)
    loadouts[seat]={setup.loadouts[seat].character,setup.loadouts[seat].animal};
   if(!app->begin_room(setup.seed,setup.difficulty,setup.seats,loadouts))return false;
   app->renderer.hud_viewer=setup.local+1;
   entered=true;
  }
  const auto movement=[](const Netplay::FrameInput& input)->PlayerTouch{
   if(input.analogMode!=Netplay::AnalogMode::DirectTouch)return {};
   return {input.unlimited?2:1,input.x,input.y};
  };
  main_input.update(frame.inputs[0].buttons);
  if((main_input.pressed&0x100)&&!app->rollback_boundary()){barrier=number;return false;}
  app->touch=movement(frame.inputs[0]);
  for(u32 seat=1;seat<setup.seats;++seat)
   app->room_partners[seat-1]={frame.inputs[seat].buttons,movement(frame.inputs[seat])};
  app->controller_buttons=0;app->keyboard_state={};
  auto& output=state(number);
  if(!app->room_speculative)output.image.clear();
  output.number=number;output.sounds.clear();app->room_sounds=&output.sounds;
  const bool ok=app->step(main_input.held,main_input.pressed,main_input.repeated,60,false);
  app->room_sounds=nullptr;
  if(!ok){if(app->room_barrier)barrier=number;return false;}
  if(barrier==number)barrier=Netplay::INVALID_FRAME;
  output.snapshot=status(number+1);
  return true;
 }
 bool update(u32 held,const PlayerTouch& touch){
  const auto now=std::uint64_t(emscripten_get_now());
  if(!session.pump(now))return app->fail(session.error());
  if(session.needs_input()){
   Netplay::FrameInput sample(u16(held&0x3fbu));
   if(touch.mode){
    sample.analogMode=Netplay::AnalogMode::DirectTouch;
    sample.x=touch.x;sample.y=touch.y;
    sample.unlimited=touch.mode==2;
   }
   if(!session.capture_input(sample,now))return app->fail(session.error());
  }
  const auto result=session.tick(*this);
  return result!=multiplayer::Session::Tick::Failed||app->fail(session.error());
 }
};
std::unique_ptr<RoomRuntime> room;
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
  }
  result[27]=battle.items->items.active_count;result[28]=battle.items->spirits.count();
  result[29]=battle.laser_scene->manager.count();result[30]=battle.spell.flags;result[31]=app->visual.calls;
  // Hash semantic fields in execution order so diagnostics expose changes to
  // movement and script state even when entity counts happen to match.
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
  for(u32 n=1;n<=ItemManager::pool_size;++n){
   const auto& item=battle.items->items.find(n)->state;
   if(item.state){word(n);word(u32(item.kind));position(item.position);word(u32(item.age.current));}
  }
  for(u32 seat=0;seat<battle.pilot_count();++seat){
   const auto& p=battle.pilot(seat);word(u32(p.session.life_pieces));word(u32(p.session.bomb_pieces));word(u32(p.life.state));
   for(const auto& shot:p.shots.shots)if(shot.state){word(u32(shot.index));position(shot.motion.position);word(u32(shot.state));}
  }
  result[32]=hash;
  result[33]=battle.dialogue_present();result[34]=battle.items->roaring.state.flags;
  result[35]=u32(battle.items->roaring.visuals.hit_delay);
  result[36]=float_to_bits(battle.frame_state().rate);result[37]=app->visual.seed;
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
 touhou::input::TouchState value;if(!app||!app->scene()||!app->run_input)return value;
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
bool sample_and_tick(){
 if(!app)return false;SDL_Event event;while(SDL_PollEvent(&event)){
  ThpracUi::process_event(event,float(render_scale));
  if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST){clear_inputs();lost_focus=!th17_host_has_focus();}
  else if(event.type==SDL_EVENT_JOYSTICK_ADDED)add_controller(event.jdevice.which);
  else if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&controller&&SDL_GetJoystickID(controller)==event.jdevice.which){SDL_CloseJoystick(controller);controller=nullptr;int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
 }
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
 if(room)return room->update(held,app->touch);
#endif
 controls.update(held);app->controller_buttons=buttons;
 if(app->display)app->display->presentation_fps=presentation_fps;
 const bool focus=lost_focus;lost_focus=false;
 const bool ok=app->step(controls.held,controls.pressed,controls.repeated,measured_fps,focus);
 const double now=double(SDL_GetTicksNS())/1e9;if(!window_start)window_start=now;
 if(ok){window_ticks++;const double duration=now-window_start;if(duration>=1){measured_fps=float(window_ticks/duration);window_ticks=0;window_start=now;}}
 return ok;
}
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th17_initialize(){using namespace th17::sdl;if(running)return 1;clear_inputs();if(controller){SDL_CloseJoystick(controller);controller=nullptr;}ThpracUi::shutdown();app.reset();app=std::make_unique<ApplicationState>();app->graphics.render_scale=render_scale;app->audio_device.music_enabled=music_enabled;if(!app->initialize(true)||!ThpracUi::initialize(*app)){last_error=app->failure;app.reset();return 0;}reset_presentation_clock();for(auto& key:keyboard_map)key.native=SDL_GetScancodeFromName(key.sdl);initialize_controller();window_start=0;window_ticks=0;measured_fps=60;return 1;}
#if TH17_MULTIPLAYER
EMSCRIPTEN_KEEPALIVE int th17_room_open(const unsigned* words,unsigned count,const char* url){
 using namespace th17::sdl;
 if(!app||!app->title||running||room||!words||count!=19||words[0]!=2||!url||
    words[1]>3||words[2]>=words[1]||words[3]>4||words[4]>65535||words[7]>9||words[18]>1||
    (words[18]&&words[7]))return 0;
 auto candidate=std::make_unique<RoomRuntime>();
 auto& setup=candidate->setup;
 setup.seats=std::uint8_t(words[1]);setup.local=std::uint8_t(words[2]);
 setup.difficulty=std::uint8_t(words[3]);setup.seed=th17::u16(words[4]);
 setup.room=words[5]|(std::uint64_t(words[6])<<32);
 setup.delay=std::uint8_t(words[7]);setup.rollback=words[18]!=0;
 for(unsigned seat=0;seat<3;++seat){
  if(words[8+seat*2]>2||words[9+seat*2]>2)return 0;
  setup.loadouts[seat]={std::uint8_t(words[8+seat*2]),std::uint8_t(words[9+seat*2])};
 }
 for(unsigned i=0;i<4;++i)setup.build[i]=words[14+i];
 if(!setup.valid()||!candidate->transport.Connect(url,setup.local,setup.seats)||
    !candidate->session.begin(setup,std::uint64_t(emscripten_get_now())))return 0;
 room=std::move(candidate);
 return 1;
}
EMSCRIPTEN_KEEPALIVE void th17_room_close(){th17::sdl::room.reset();}
EMSCRIPTEN_KEEPALIVE const unsigned* th17_room_status(){
 using namespace th17::sdl;
 static RoomRuntime::Status value{};
 value=room&&app?room->status(room->session.next_frame()):RoomRuntime::Status{};
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
EMSCRIPTEN_KEEPALIVE unsigned th17_phase(){using namespace th17::sdl;return !app?4:app->exiting?4:app->title?0:app->ending?3:app->pause&&app->pause->state.screen!=th17::PauseScreen::Inactive?2:1;}
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
  if(period!=previous_period){cadence.reset();previous_period=period;}
  const auto ticks=cadence.advance(delta,period);bool ok=true;app->graphics.backend.defer=true;
  if(ticks)th17_browser_frame_begin();unsigned completed=0;double predicted_tick=0;
  // RAF's timestamp and emscripten_get_now share the performance clock. Work
  // already done before this callback has consumed part of this refresh.
  const double deadline=begin+std::max(0.,1000./60.-std::max(0.,begin-time));
  for(unsigned i=0;i<ticks&&ok;i++){
   const double tick_begin=emscripten_get_now();
#if TH17_MULTIPLAYER
   const auto previous_input_frame=room?room->session.next_frame():0;
#endif
   ok=sample_and_tick();
#if TH17_MULTIPLAYER
   if(!room||room->session.next_frame()!=previous_input_frame)++completed;
#else
   ++completed;
#endif
   const double tick_end=emscripten_get_now();predicted_tick=std::max(predicted_tick,tick_end-tick_begin);if(app->exiting)break;
   // Check whether the NEXT complete update/draw fits, rather than noticing
   // an overrun only after it happened. Keep unexecuted ticks as short debt.
   if(i+1<ticks&&tick_end+predicted_tick>=deadline){cadence.debt=std::min(.1,cadence.debt+(ticks-i-1)*period);break;}
  }
  if(ok&&completed)ok=ThpracUi::render(*app);
  app->graphics.backend.commit();app->graphics.backend.defer=false;app->audio_device.pump();
  if(ok&&completed)++presentation_frames;const double elapsed=time-presentation_start;if(elapsed>=1000){presentation_fps=float(presentation_frames*1000./elapsed);presentation_start=time;presentation_frames=0;}
  if(ticks)th17_browser_frame(ok?1:0,emscripten_get_now()-begin,completed);if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
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
EMSCRIPTEN_KEEPALIVE void th17_touch_cancel(){using namespace th17::sdl;practice_pointers.reset();imgui_touch.reset();gestures.cancel();ThpracUi::cancel_pointer();if(app){app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
EMSCRIPTEN_KEEPALIVE void th17_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.mode=mode<=3?int(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,.25f,4.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th17_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th17_touch_stick(float x,float y){using namespace th17::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
EMSCRIPTEN_KEEPALIVE int th17_open_replay(const char* name,int stage){using namespace th17::sdl;if(running||!app||!app->title||!name||!app->files.replay(name,app->selected_replay)||!app->selected_replay)return 0;if(stage==0){for(int i=1;i<=7;i++)if(app->selected_replay->stage(i)){stage=i;break;}}th17::ReplayStageState entry;if(!app->selected_replay->initial_state(stage,entry))return 0;auto& p=app->progress;p.stage=p.initial_stage=stage;p.character=entry.run.character;p.animal=entry.run.animal;p.difficulty=entry.run.difficulty;p.spell_id=entry.run.spell_id;p.scoring.mode_flags=app->selected_replay->mode_flags()&2?0x20u:app->selected_replay->practice()?0x10u:0;p.demo_slot=-1;app->flow.pending=th17::AppScene::ReplayStart;return 1;}
#if TH17_DEVELOPMENT_HARNESS
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
