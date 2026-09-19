#ifdef TH_NATIVE_PLATFORM
#include "PlatformHost.hpp"
#include "GraphicsHost.hpp"
#include "FrameCadence.hpp"
#include "PresentationCadence.hpp"
#include "Renderer.hpp"
#include "ThpracUi.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <memory>
EM_JS(int, th08_frame_ready, (), {return Module['runtimePrepare']?Module['runtimePrepare']():1;});
EM_JS(void, th08_frame_finished, (int result,double ms), {if(Module['runtimeFinish'])Module['runtimeFinish'](result,ms);});
EM_JS(int, th08_limit_presentation_to_60, (), {return Module['eaglerOptions']?.limitPresentationTo60?1:0;});
namespace th08 {
void sdl_validate_capture();
namespace {
std::unique_ptr<BrowserRuntime> runtime;touhou::input::TouchController touch;
struct Key{const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
SDL_Joystick* joystick=nullptr;u32 prepared=0;bool running=false,suspended=false,presentation_primed=false;double elapsed=0,last=-1,frame_begin=0;u32 frames=0,loop_epoch=0,warm_mask=0;touhou::sdl::FrameCadence cadence;touhou::sdl::PresentationCadence presentation;
constexpr const char* warmAnimations[]={"etama.anm","enemy.anm","front.anm","times.anm","stg1bg.anm","stg1enm.anm","eff01.anm","stg1txt.anm","stg2bg.anm","stg2enm.anm","eff02.anm","stg2txt.anm","player00.anm","player01.anm","player02.anm","player03.anm","staff01.anm"};
constexpr u32 warmCount=sizeof(warmAnimations)/sizeof(*warmAnimations);
touhou::input::TouchState touch_state(){touhou::input::TouchState s;if(!runtime)return s;const auto& a=runtime->app;const auto& g=a.game;const auto& p=g.player_state;
    if(a.in_game()&&(g.globals.game_flags&8)){s.context=3;return s;}
    if(!a.in_game()||a.loading_game()||!g.ready()||g.paused||g.menus.context.pause_state||p.context.game_over||(g.globals.game_flags&0x60))return s;
    s.context=g.dialogue.present()?2:1;s.ready=p.life.state!=1&&p.life.state!=2;s.instance=g.globals.stage+1;
    s.x=p.motion.movement.position.x;s.y=p.motion.movement.position.y;s.fast=g.shots[0].settings().normal_speed*g.player.timing.rate;s.slow=g.shots[1].settings().focus_speed*g.player.timing.rate;
    s.min_x=p.input.minimum.x;s.min_y=p.input.minimum.y;s.max_x=s.min_x+p.input.extent.x;s.max_y=s.min_y+p.input.extent.y;return s;
}
int touch_stage(){return runtime&&runtime->app.in_game()?runtime->app.game.globals.stage:-1;}
void sync_touch_context(const touhou::input::TouchState& state){const int previous=touch.current_context();if(runtime&&previous!=state.context&&(previous==1||previous==2))runtime->motion.touch_cancel(touch_stage());}
void pointer(int type,int id,float x,float y){const auto state=touch_state();sync_touch_context(state);if(runtime&&(state.context==1||state.context==2))runtime->motion.touch_event(touch_stage(),type,id,x,y);touch.pointer(type,id,x,y,SDL_GetTicks(),state,runtime&&runtime->keyboard_state()[16]);}
void cancel_touch(){if(runtime){runtime->motion.touch_cancel(touch_stage());runtime->motion.target(0,0,0);}touch.cancel_transient();}
bool interpolation_ready(){
    if(!runtime||runtime->app.loading_game()||runtime->app.title.modal())return false;
    if(runtime->app.title.active())return true;
    if(!runtime->app.in_game())return false;
    const auto& g=runtime->app.game;
    return g.menus.context.supervisor_state==2&&!g.control.state.load_state&&(g.globals.game_flags&0x60)!=0x20;
}
void poll(){if(!runtime)return;SDL_Event event;while(SDL_PollEvent(&event)){
    ThpracUi::process_event(event);
    if(event.type==SDL_EVENT_FINGER_CANCELED)cancel_touch();
    if(event.type==SDL_EVENT_FINGER_DOWN||event.type==SDL_EVENT_FINGER_MOTION||event.type==SDL_EVENT_FINGER_UP)pointer(event.type==SDL_EVENT_FINGER_DOWN?0:event.type==SDL_EVENT_FINGER_MOTION?1:2,int(event.tfinger.fingerID),event.tfinger.x,event.tfinger.y);
    if(event.type==SDL_EVENT_JOYSTICK_ADDED&&!joystick)joystick=SDL_OpenJoystick(event.jdevice.which);
    if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&joystick&&SDL_GetJoystickID(joystick)==event.jdevice.which){SDL_CloseJoystick(joystick);joystick=nullptr;}
    }
    auto* keys=runtime->keyboard_state();std::memset(keys,0,256);const bool* physical=SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){keys[k.vk]=128;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=128;}
    if(joystick&&SDL_JoystickConnected(joystick)){u8 buttons[128]{};for(int i=0;i<std::min(128,SDL_GetNumJoystickButtons(joystick));i++)buttons[i]=SDL_GetJoystickButton(joystick,i)?128:0;
        runtime->controller_state(SDL_GetJoystickAxis(joystick,0)*1000/32767,SDL_GetJoystickAxis(joystick,1)*1000/32767,buttons,128,true);
    }else runtime->controller_state(0,0,nullptr,0,false);
    const auto state=touch_state();sync_touch_context(state);const auto input=touch.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);for(int i=0;i<256;i++)if(input.keys[i])keys[i]=128;
    runtime->motion.target(input.motion,input.x,input.y);
    ThpracUi::update_input(*runtime);
    if(ThpracUi::captures_game_input())for(const int vk:{16,27,37,38,39,40,88,90})keys[vk]=0;
}
int tick(){poll();return !runtime||!runtime->step(false)?runtime&&(runtime->status(2)||runtime->status(4))?2:1:0;}
EM_BOOL frame(double now,void* epoch){if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double delta=last<0?0:std::max(0.,(now-last)/1000.);last=now;frame_begin=emscripten_get_now();
    if(suspended||!th08_frame_ready()){sdl_audio_pause(true);cadence.reset();presentation.reset();presentation_primed=false;return EM_TRUE;}sdl_audio_pause(false);int result=0;
    const bool limit60=th08_limit_presentation_to_60()!=0;
    const bool ready=interpolation_ready()&&!limit60,fast=touhou::sdl::PresentationCadence::fast_sample(delta);if(ready)presentation.advance(delta);else presentation.reset();if(!presentation.high_refresh||!fast)presentation_primed=false;
    const bool tick_due=cadence.advance(delta)!=0;
    const bool high=presentation.high_refresh&&interpolation_ready();if(high&&fast&&!presentation_primed&&tick_due)presentation_primed=true;const bool interpolate=high&&fast&&presentation_primed;bool presented=false;
    if(tick_due&&!result){
        elapsed+=touhou::sdl::FrameCadence::interval;result=tick();
        if(!result&&runtime){
            // Preserve TH08's authoritative update+draw tick exactly. At high
            // presentation rates every fixed-tick draw is semantic but hidden;
            // the visible frame below is a second, side-effect-free presentation
            // pass. Missed original deadlines are skipped rather than caught up.
            const bool hidden=high;if(hidden)sdl_defer(1);
            if(!runtime->app.draw(1.0f,false,false))result=(runtime->status(2)||runtime->status(4))?2:1;
            else if(runtime->status(2)||runtime->status(4))result=2;
            if(hidden)sdl_defer(0);else presented=true;
            if(!result){++frames;if(!runtime->audio_tick(u32(elapsed*1000)))result=2;}
        }
    }
    if(!result&&runtime&&high){
        const bool frozen=runtime->app.in_game()&&(runtime->app.game.paused||runtime->app.game.retrying||runtime->app.game.menus.context.pause_state||runtime->app.game.menus.context.show_retry);
        const float alpha=interpolate?float(cadence.interpolation_alpha()):1.0f;presented=runtime->app.draw(alpha,interpolate,true,!frozen);
    }
    if(presented&&runtime)runtime->app.statistics.presentation_frame();
    sdl_audio_pump();th08_frame_finished(result,emscripten_get_now()-frame_begin);return running?EM_TRUE:EM_FALSE;
}
}
u32 sdl_game_time(){return u32(elapsed*1000);}
extern "C" {
#define EX(name) __attribute__((export_name(name)))
EX("sdl_game_open") BrowserRuntime* sdl_game_open(u32 milliseconds){if(runtime)return nullptr;prepared=frames=warm_mask=0;elapsed=double(milliseconds)/1000.;cadence.reset();presentation.reset();presentation_primed=false;last=-1;touch.begin_session();
    runtime=std::make_unique<BrowserRuntime>();if(!sdl_attach(runtime.get())||!sdl_load_assets(*runtime)){runtime.reset();sdl_detach();return nullptr;}
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);int count=0;auto* ids=SDL_GetJoysticks(&count);if(count)joystick=SDL_OpenJoystick(ids[0]);SDL_free(ids);return runtime.get();}
EX("sdl_prepare_total") u32 sdl_prepare_total(){return runtime?runtime->resources().size()+runtime->native_font_steps()+warmCount:0;}
EX("sdl_prepare_next") i32 sdl_prepare_next(){if(!runtime)return -1;if(prepared>=sdl_prepare_total())return 0;const auto assets=runtime->resources().size();
    bool ok=true;const auto fonts=runtime->native_font_steps();
    if(prepared<assets)ok=sdl_prepare_asset(*runtime,prepared);
    else if(prepared<assets+fonts)ok=runtime->native_font_step(prepared-assets);
    else {const auto& bytes=runtime->file(warmAnimations[prepared-assets-fonts]);
        // Optional bounded cache: exhaustion must never prevent launching.
        if(!bytes.empty()&&runtime->app.library.preload(bytes.data(),bytes.size()))warm_mask|=1u<<(prepared-assets-fonts);}
    ++prepared;return ok?i32(prepared):-1;}
EX("sdl_warm_assets") u32 sdl_warm_assets(){return warm_mask;}
EX("sdl_game_initialize") bool sdl_game_initialize(){if(!runtime||prepared!=sdl_prepare_total()||!runtime->initialize()||!ThpracUi::initialize())return false;sdl_validate_capture();return true;}
EX("sdl_loop_start") void sdl_loop_start(){if(running||!runtime)return;running=true;last=-1;cadence.reset();presentation.reset();presentation_primed=false;emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));}
EX("sdl_loop_stop") void sdl_loop_stop(){running=false;++loop_epoch;sdl_audio_pause(true);}
EX("sdl_loop_pause") void sdl_loop_pause(u32 pause){suspended=pause!=0;last=-1;cadence.reset();presentation.reset();presentation_primed=false;sdl_audio_pause(suspended);}
EX("sdl_loop_time") double sdl_loop_time(){return elapsed;}
EX("sdl_loop_tick") i32 sdl_loop_tick(BrowserRuntime* r,double seconds,u32){
    if(running||r!=runtime.get())return -1;elapsed+=seconds;int result=tick();if(result||!runtime)return result;
    if(!runtime->app.draw(1.0f,false,false))return (runtime->status(2)||runtime->status(4))?2:1;
    if(runtime->status(2)||runtime->status(4))return 2;
    ++frames;return runtime->audio_tick(u32(elapsed*1000))?0:2;
}
EX("sdl_game_close") void sdl_game_close(){sdl_loop_stop();touch.reset();ThpracUi::shutdown();runtime.reset();if(joystick)SDL_CloseJoystick(joystick);joystick=nullptr;sdl_audio_shutdown();sdl_fonts_shutdown();sdl_detach();}
EX("sdl_key") void sdl_key(const char* code,u32 down){for(auto& key:keyboard_map)if(!std::strcmp(key.code,code)){key.hosted=down!=0;break;}}
EX("sdl_keys_clear") void sdl_keys_clear(){for(auto& key:keyboard_map)key.hosted=false;cancel_touch();touch.reset();}
EX("sdl_touch") void sdl_touch(u32 type,i32 id,float x,float y){pointer(type,id,x,y);}
EX("sdl_touch_cancel") void sdl_touch_cancel(){cancel_touch();}
EX("sdl_thprac_mouse") void sdl_thprac_mouse(u32 type,float x,float y){ThpracUi::mouse(type,x,y);}
EX("sdl_touch_options") void sdl_touch_options(u32 on,u32 free,float speed){touch.enabled=on;touch.unlimited=free;touch.sensitivity=std::clamp(speed,1.f,3.f);if(!on)sdl_touch_cancel();}
EX("sdl_touch_gestures") void sdl_touch_gestures(u32 two,u32 taps){touch.two_finger=two;touch.double_tap=taps;}
EX("sdl_touch_mode") void sdl_touch_mode(u32 mode){if(touch.set_mode(static_cast<int>(mode))&&runtime)runtime->motion.target(0,0,0);}
EX("sdl_touch_controls") void sdl_touch_controls(u32 fire,u32 focus,u32 bomb,u32 escape,float x,float y){touch.controls(fire,focus,bomb,escape,x,y);}
EX("sdl_touch_display") void sdl_touch_display(u32 hitbox){if(runtime)runtime->app.game.always_hitbox=hitbox!=0;}
EX("sdl_game_status") const i32* sdl_game_status(){static i32 out[10]{};if(runtime){out[0]=runtime->status(0);out[1]=runtime->status(3);out[2]=runtime->status(2)||runtime->status(4);out[3]=number(runtime->app.session.numbers.lives).truncate_int();out[4]=runtime->app.session.numbers.power;out[5]=touch.current_context();out[6]=touch.active();out[7]=touch.fire;out[8]=touch.focus;out[9]=frames;}return out;}
}
}
#endif
