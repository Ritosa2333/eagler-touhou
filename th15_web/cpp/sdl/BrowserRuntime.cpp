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
EM_JS(void, th15_browser_frame, (int ok,double milliseconds,unsigned ticks), { Module["onGameFrame"]?.(ok,milliseconds,ticks); });
EM_JS(int, th15_host_has_focus, (), {
 try { return parent.document.hasFocus() && !parent.document.hidden ? 1 : 0; }
 catch (_) { return document.hasFocus() && !document.hidden ? 1 : 0; }
});
EM_JS(int, th15_browser_keyboard_owned, (), { return typeof Module.resetBrowserKeyboard === "function"; });
EM_JS(void, th15_browser_keyboard_reset, (), { Module.resetBrowserKeyboard?.(); });
namespace th15::sdl {
namespace {
std::unique_ptr<ApplicationState> app;
std::string last_error;
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
touhou::sdl::FrameCadence cadence;
GameInput controls;
SDL_Joystick* controller=nullptr;
bool running=false,suspended=false,lost_focus=false,music_enabled=true;u32 render_scale=1;
u32 loop_epoch=0;
double previous_frame=-1,window_start=0;
u32 window_ticks=0;
float measured_fps=60;
// Presentation counts committed browser frames, independently of simulation
// ticks and replay acceleration. Keep recorded FPS on its existing clock.
double presentation_start=-1;u32 presentation_frames=0;float presentation_fps=60;
FrameCallback fps_drawing;
void reset_presentation_clock(){presentation_start=-1;presentation_frames=0;presentation_fps=60;}
i32 draw_frame_rate(void*){
 if(!app||app->title||app->ending||!app->run)return 1;
 // Original 4336e0 / 40c800: draw priority 72, small ASCII font, no shadow.
 HudTextStyle style;style.font=1;style.color=presentation_fps<30?0xff5050ff:presentation_fps<40?0xffa0a0ff:0xffffffff;
 char text[32];std::snprintf(text,sizeof text,"%2.1ffps",double(presentation_fps)+.05);
 return app->captions.enqueue({{588,470,0},style,text})?1:5;
}
void add_controller(SDL_JoystickID id){if(!controller)controller=SDL_OpenJoystick(id);}
void clear_inputs(){th15_browser_keyboard_reset();for(auto& key:keyboard_map)key.hosted=false;SDL_ResetKeyboard();gestures.reset();controls={};if(app){app->keyboard_state={};app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
void initialize_controller(){if(controller){SDL_CloseJoystick(controller);controller=nullptr;}SDL_InitSubSystem(SDL_INIT_JOYSTICK);int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
touhou::input::TouchState touch_state(){
 touhou::input::TouchState value;if(!app||!app->scene()||!app->session)return value;
 if(app->pause&&app->pause->state.screen!=PauseScreen::Inactive)return value;
 if(app->progress.replay){value.context=3;return value;}
 auto& stage=*app->scene();if(stage.messages.active()){value.context=2;return value;}
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
  if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST){clear_inputs();lost_focus=!th15_host_has_focus();}
  else if(event.type==SDL_EVENT_JOYSTICK_ADDED)add_controller(event.jdevice.which);
  else if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&controller&&SDL_GetJoystickID(controller)==event.jdevice.which){SDL_CloseJoystick(controller);controller=nullptr;int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;i++)add_controller(ids[i]);SDL_free(ids);}
 }
 bool keys[256]{};const auto* physical=SDL_GetKeyboardState(nullptr);const bool browser_keyboard=th15_browser_keyboard_owned();
 for(const auto& key:keyboard_map)if(key.hosted||(!browser_keyboard&&key.native!=SDL_SCANCODE_UNKNOWN&&physical[key.native])){if(key.vk<256)keys[key.vk]=true;if(key.vk>=160&&key.vk<=165)keys[16+(key.vk-160)/2]=true;if(key.scan==28||key.scan==156)keys[13]=true;}
 app->keyboard_state.format=2;for(u32 i=0;i<256;i++)app->keyboard_state.keys[i]=keys[i]?128:0;
 const auto sample=gestures.sample(touch_state(),SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
 for(u32 i=0;i<256;i++)keys[i]=keys[i]||sample.keys[i];app->touch={sample.motion,sample.x,sample.y};
 u32 buttons=0;const u32 held=keyboard_keys(keys)|sample_controller(buttons);controls.update(held);app->controller_buttons=buttons;
 app->numbered_chapter=0;for(u32 i=1;i<=9;i++)if(keys[48+i]){app->numbered_chapter=i;break;}
 const bool focus=lost_focus;lost_focus=false;
 const bool ok=app->step(controls.held,controls.pressed,controls.repeated,measured_fps,focus);
 const double now=double(SDL_GetTicksNS())/1e9;if(!window_start)window_start=now;
 if(ok){window_ticks++;const double duration=now-window_start;if(duration>=1){measured_fps=float(window_ticks/duration);window_ticks=0;window_start=now;}}
 return ok;
}
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th15_prepare_loading(){using namespace th15::sdl;if(running)return 0;if(app)app->scheduler.remove(fps_drawing);app=std::make_unique<ApplicationState>();app->graphics.render_scale=render_scale;app->audio_device.music_enabled=music_enabled;return app->prepare_loading();}
EMSCRIPTEN_KEEPALIVE int th15_initialize(){using namespace th15::sdl;if(running)return 1;clear_inputs();if(controller){SDL_CloseJoystick(controller);controller=nullptr;}if(!app||!app->platform_prepared||app->initialized){if(app)app->scheduler.remove(fps_drawing);app=std::make_unique<ApplicationState>();app->graphics.render_scale=render_scale;app->audio_device.music_enabled=music_enabled;}if(!app->initialize(true)){last_error=app->failure;app.reset();return 0;}fps_drawing.owner=nullptr;fps_drawing.enabled=true;fps_drawing.run=draw_frame_rate;if(app->scheduler.add(fps_drawing,th15::FramePass::Draw,72)<0){app->fail("Frame-rate draw callback registration failed");return 0;}reset_presentation_clock();for(auto& key:keyboard_map)key.native=SDL_GetScancodeFromName(key.sdl);initialize_controller();window_start=0;window_ticks=0;measured_fps=60;return 1;}
EMSCRIPTEN_KEEPALIVE void th15_render_scale(unsigned scale){if(!th15::sdl::running)th15::sdl::render_scale=scale>=2?2:1;}
EMSCRIPTEN_KEEPALIVE const char* th15_error(){using namespace th15::sdl;return app?app->failure.c_str():last_error.c_str();}
EMSCRIPTEN_KEEPALIVE unsigned th15_phase(){using namespace th15::sdl;return !app?4:app->exiting?4:app->title?0:app->ending?3:app->pause&&app->pause->state.screen!=th15::PauseScreen::Inactive?2:1;}
EMSCRIPTEN_KEEPALIVE unsigned th15_frame(){using namespace th15::sdl;return app?app->frames:0;}
EMSCRIPTEN_KEEPALIVE int th15_save_scores(){using namespace th15::sdl;return !app||app->save_settings();}
EMSCRIPTEN_KEEPALIVE int th15_save_replay(unsigned slot,const char* name){using namespace th15::sdl;if(!app||!name||std::strlen(name)>8||slot>=99)return 0;std::array<char,9> text{};std::memcpy(text.data(),name,std::strlen(name));return app->prepare_live_replay(false)&&app->save_slot(slot,text);}
EMSCRIPTEN_KEEPALIVE int th15_validate_file(unsigned kind,const unsigned char* data,unsigned size){if(kind==0){th15::ScoreFile file;return file.open(data,size);}if(kind==1){th15::Replay file;return file.open(data,size);}if(kind==2){th15::GameConfig file;return file.open(data,size);}if(kind==3){th15::CheckpointFile file;return file.open(data,size);}return 0;}
EMSCRIPTEN_KEEPALIVE void th15_key(unsigned scan,unsigned down){for(auto& key:th15::sdl::keyboard_map)if(key.scan==scan)key.hosted=down!=0;}
EMSCRIPTEN_KEEPALIVE void th15_keys_clear(){th15::sdl::clear_inputs();}
EMSCRIPTEN_KEEPALIVE void th15_music_enabled(unsigned enabled){using namespace th15::sdl;music_enabled=enabled!=0;if(app){app->audio_device.music_enabled=music_enabled;app->audio_device.refresh_volume();}}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_audio_statistics(){using namespace th15::sdl;return app?app->audio_device.statistics():nullptr;}
EMSCRIPTEN_KEEPALIVE void th15_audio_close(){using namespace th15::sdl;if(app)app->audio_device.close();}
EMSCRIPTEN_KEEPALIVE void th15_loop_stop(){using namespace th15::sdl;running=false;++loop_epoch;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();if(app)app->audio_device.suspend(true);}
EMSCRIPTEN_KEEPALIVE void th15_loop_pause(unsigned enabled){using namespace th15::sdl;suspended=enabled!=0;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();if(app)app->audio_device.suspend(suspended);}
EMSCRIPTEN_KEEPALIVE void th15_loop_start(){
 using namespace th15::sdl;if(running||!app||!app->initialized)return;running=true;suspended=false;previous_frame=-1;cadence.reset();clear_inputs();window_start=0;window_ticks=0;reset_presentation_clock();app->audio_device.suspend(false);
 emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
  if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
  if(suspended){cadence.reset();return EM_TRUE;}if(presentation_start<0)presentation_start=time;const auto ticks=cadence.advance(delta);bool ok=true;app->graphics.backend.defer=true;
  unsigned completed=0;
  for(unsigned i=0;i<ticks&&ok;i++){ok=sample_and_tick();++completed;if(app->exiting||app->pending_load)break;
   // Keep every executed update/draw, but do not turn one expensive tick into
   // four consecutive full renders. Retain the short debt for the next RAF.
   if(i+1<ticks&&emscripten_get_now()-begin>=1000./60.){cadence.debt=std::min(.1,cadence.debt+(ticks-i-1)*touhou::sdl::FrameCadence::interval);break;}
  }app->graphics.backend.commit();app->graphics.backend.defer=false;app->audio_device.pump();
  if(ok&&completed)++presentation_frames;const double elapsed=time-presentation_start;if(elapsed>=1000){presentation_fps=float(presentation_frames*1000./elapsed);presentation_start=time;presentation_frames=0;}
  if(completed)th15_browser_frame(ok?1:0,emscripten_get_now()-begin,completed);if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
 },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th15_touch(unsigned type,int id,float x,float y){using namespace th15::sdl;if(std::isfinite(x)&&std::isfinite(y))gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);}
EMSCRIPTEN_KEEPALIVE void th15_touch_cancel(){using namespace th15::sdl;gestures.cancel();if(app){app->touch={};if(app->scene())app->scene()->battle.player->motion.touch={};}}
EMSCRIPTEN_KEEPALIVE void th15_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th15::sdl;gestures.enabled=enabled!=0;gestures.mode=mode<=3?int(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,.25f,4.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th15_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th15::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th15_touch_stick(float x,float y){using namespace th15::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
#if TH15_DEVELOPMENT_HARNESS
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* th15_probe_graphics_statistics(){using namespace th15::sdl;return app?&app->graphics.backend.stats:nullptr;}
EMSCRIPTEN_KEEPALIVE int th15_probe_tick(){return !th15::sdl::running&&th15::sdl::sample_and_tick();}
// Diagnostic replay batches retain every update/draw and defer only window presentation.
EMSCRIPTEN_KEEPALIVE int th15_probe_ticks(unsigned count){using namespace th15::sdl;if(running||!app||count>120)return 0;app->graphics.backend.defer=true;bool ok=true;for(unsigned i=0;i<count&&ok;i++)ok=sample_and_tick();app->graphics.backend.commit();app->graphics.backend.defer=false;return ok;}
EMSCRIPTEN_KEEPALIVE const int* th15_probe_state(){using namespace th15::sdl;return app?app->projection():nullptr;}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_probe_world_state(){using namespace th15::sdl;static std::array<unsigned,16> v{};v.fill(0);if(app&&app->scene()){auto& b=app->scene()->battle;const auto& p=*b.player;v={unsigned(app->progress.stage),unsigned(app->progress.stage_frame),b.active_frame().held,th15::float_to_bits(p.motion.position.x),th15::float_to_bits(p.motion.position.y),unsigned(p.life.state),unsigned(b.score.score),unsigned(b.session.deaths),unsigned(b.session.extra_lives),unsigned(b.session.power),unsigned(b.session.bombs),app->environment.game_rng.seed,app->environment.game_rng.calls,b.enemies->count(),b.bullet_scene->manager.visible_count,app->progress.scene_flags};}return v.data();}
EMSCRIPTEN_KEEPALIVE const float* th15_probe_player_state(){using namespace th15::sdl;static std::array<float,10> value{};value.fill(0);if(app&&app->scene()){const auto& p=*app->scene()->battle.player;value={p.motion.position.x,p.motion.position.y,float(p.life.state),float(p.motion.input_frame),float(p.motion.normal_speed)/128,float(p.motion.focus_speed)/128,float(app->scene()->battle.active_frame().held),float(p.motion.touch.mode),p.motion.touch.x,p.motion.touch.y};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th15_probe_spell_state(){using namespace th15::sdl;static std::array<int,6> value{};value.fill(0);if(app&&app->scene()){const auto& b=app->scene()->battle;const auto& c=b.spell_card.clock;value={int(b.spell.flags),b.spell_card.identifier,b.spell_card.active_frames,c.completed_frames,c.encoded,c.sequence};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th15_probe_hud_score(){using namespace th15::sdl;static std::array<int,6> value{};value.fill(0);if(app&&app->scene()){const auto& s=*app->scene();value={s.battle.score.score,s.hud.displayed_score,s.hud.score_increment,app->progress.high_score,app->progress.high_score_extra,int(s.battle.session.mode_flags)};}return value.data();}
EMSCRIPTEN_KEEPALIVE const int* th15_probe_pause_state(){using namespace th15::sdl;static std::array<int,7> value{};value.fill(-1);if(app&&app->pause){const auto& p=app->pause->state;value={int(p.screen),p.phase,p.menu.cursor,p.menu.count,p.names.cursor,p.name_length,p.age.current};}return value.data();}
#endif
}
