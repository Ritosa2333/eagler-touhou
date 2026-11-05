#include <eagler/input/BrowserGamepad.hpp>
#include "ApplicationState.hpp"
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
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
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
void clear_inputs(){th17_browser_keyboard_reset();for(auto& key:keyboard_map)key.hosted=false;SDL_ResetKeyboard();gestures.reset();controls={};if(app){app->keyboard_state={};app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
void initialize_controller(){if(controller){SDL_CloseJoystick(controller);controller=nullptr;}SDL_InitSubSystem(SDL_INIT_JOYSTICK);int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
touhou::input::TouchState touch_state(){
 touhou::input::TouchState value;if(!app||!app->scene()||!app->run_input)return value;
 if(app->pause&&app->pause->state.screen!=PauseScreen::Inactive)return value;
 if(app->progress.practice_record){value.context=3;return value;}
 auto& stage=*app->scene();if(stage.battle.dialogue_present()){value.context=2;return value;}
 auto& player=*stage.battle.player;auto& motion=player.motion;
 value.context=1;value.instance=app->progress.stage;value.ready=player.life.state==1&&!(app->progress.scene_flags&(4|0x70|0x4000|0x10000));
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
 u32 buttons=0;const u32 held=keyboard_keys(keys)|(mappedPad<0?sample_controller(buttons):0);controls.update(held);app->controller_buttons=buttons;
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
EMSCRIPTEN_KEEPALIVE int th17_initialize(){using namespace th17::sdl;if(running)return 1;clear_inputs();if(controller){SDL_CloseJoystick(controller);controller=nullptr;}app.reset();app=std::make_unique<ApplicationState>();app->graphics.render_scale=render_scale;app->audio_device.music_enabled=music_enabled;if(!app->initialize(true)){last_error=app->failure;app.reset();return 0;}reset_presentation_clock();for(auto& key:keyboard_map)key.native=SDL_GetScancodeFromName(key.sdl);initialize_controller();window_start=0;window_ticks=0;measured_fps=60;return 1;}
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
  if(suspended){cadence.reset();return EM_TRUE;}if(presentation_start<0)presentation_start=time;const auto ticks=cadence.advance(delta);bool ok=true;app->graphics.backend.defer=true;
  if(ticks)th17_browser_frame_begin();unsigned completed=0;double predicted_tick=0;
  // RAF's timestamp and emscripten_get_now share the performance clock. Work
  // already done before this callback has consumed part of this refresh.
  const double deadline=begin+std::max(0.,1000./60.-std::max(0.,begin-time));
  for(unsigned i=0;i<ticks&&ok;i++){const double tick_begin=emscripten_get_now();ok=sample_and_tick();++completed;const double tick_end=emscripten_get_now();predicted_tick=std::max(predicted_tick,tick_end-tick_begin);if(app->exiting)break;
   // Check whether the NEXT complete update/draw fits, rather than noticing
   // an overrun only after it happened. Keep unexecuted ticks as short debt.
   if(i+1<ticks&&tick_end+predicted_tick>=deadline){cadence.debt=std::min(.1,cadence.debt+(ticks-i-1)*touhou::sdl::FrameCadence::interval);break;}
  }app->graphics.backend.commit();app->graphics.backend.defer=false;app->audio_device.pump();
  if(ok&&completed)++presentation_frames;const double elapsed=time-presentation_start;if(elapsed>=1000){presentation_fps=float(presentation_frames*1000./elapsed);presentation_start=time;presentation_frames=0;}
  if(completed)th17_browser_frame(ok?1:0,emscripten_get_now()-begin,completed);if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
 },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th17_touch(unsigned type,int id,float x,float y){using namespace th17::sdl;if(std::isfinite(x)&&std::isfinite(y))gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);}
EMSCRIPTEN_KEEPALIVE void th17_touch_cancel(){using namespace th17::sdl;gestures.cancel();if(app){app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
EMSCRIPTEN_KEEPALIVE void th17_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.mode=mode<=3?int(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,.25f,4.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th17_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th17::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th17_touch_stick(float x,float y){using namespace th17::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
EMSCRIPTEN_KEEPALIVE int th17_open_replay(const char* name,int stage){using namespace th17::sdl;if(running||!app||!app->title||!name||!app->files.replay(name,app->selected_replay)||!app->selected_replay)return 0;if(stage==0){for(int i=1;i<=7;i++)if(app->selected_replay->stage(i)){stage=i;break;}}th17::ReplayStageState entry;if(!app->selected_replay->initial_state(stage,entry))return 0;auto& p=app->progress;p.stage=p.initial_stage=stage;p.character=entry.run.character;p.animal=entry.run.animal;p.difficulty=entry.run.difficulty;p.spell_id=entry.run.spell_id;p.scoring.mode_flags=0;p.demo_slot=-1;app->flow.pending=th17::AppScene::ReplayStart;return 1;}
#if TH17_DEVELOPMENT_HARNESS
// Enter the supplied replay without menu automation. All subsequent ticks use
// the production input, audio, update, draw and RAF presentation paths.

EMSCRIPTEN_KEEPALIVE const int* th17_probe_complete_world(){using namespace th17::sdl;static std::array<int,24> v{};v.fill(0);if(app&&app->scene()){auto& s=*app->scene();auto& b=s.battle;v={s.controller->counters[0],b.player->motion.position_fixed.x,b.player->motion.position_fixed.y,b.player->life.state,b.session.extra_lives,b.session.life_pieces,b.session.bombs,b.session.bomb_pieces,b.session.power,b.session.deaths,b.score.score,b.score.point_value,b.graze_total,b.enemy_world.current_chapter,int(b.enemies->count()),int(b.bullet_scene->manager.visible_count),app->environment.game_rng.seed,int(app->environment.game_rng.calls),int(app->visual.seed),int(app->visual.calls),int(s.background.drawn_instances),int(s.background.culled_instances),int(s.background.drawn_primitives),int(s.background_flags)};}return v.data();}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* th17_probe_graphics_statistics(){using namespace th17::sdl;return app?&app->graphics.backend.stats:nullptr;}
EMSCRIPTEN_KEEPALIVE int th17_probe_tick(){return !th17::sdl::running&&th17::sdl::sample_and_tick();}
// Diagnostic replay batches retain every update/draw and defer only window presentation.
EMSCRIPTEN_KEEPALIVE int th17_probe_ticks(unsigned count){using namespace th17::sdl;if(running||!app||count>120)return 0;app->graphics.backend.defer=true;bool ok=true;for(unsigned i=0;i<count&&ok;i++)ok=sample_and_tick();app->graphics.backend.commit();app->graphics.backend.defer=false;return ok;}
EMSCRIPTEN_KEEPALIVE const int* th17_probe_state(){using namespace th17::sdl;return app?app->projection():nullptr;}
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
