#ifdef TH_NATIVE_PLATFORM
#include "PlatformHost.hpp"
#include "GraphicsHost.hpp"
#include "FrameCadence.hpp"
#include "PresentationCadence.hpp"
#include "Renderer.hpp"
#include "../game/PresentationAudit.hpp"
#include "ThpracUi.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <memory>
EM_JS(int, th08_frame_ready, (), {return Module['runtimePrepare']?Module['runtimePrepare']():1;});
EM_JS(void, th08_frame_finished, (int result,double ms), {if(Module['runtimeFinish'])Module['runtimeFinish'](result,ms);});
EM_JS(int, th08_limit_presentation_to_60, (), {return Module['eaglerOptions']?.limitPresentationTo60?1:0;});
EM_JS(int, th08_keyboard_gamepad_dpad, (), {
    if (!navigator.getGamepads) return 0;
    let bits = 0;
    for (const pad of navigator.getGamepads()) {
        if (!pad || !pad.buttons || pad.buttons.length < 16) continue;
        if (!/keyboard|\bkb\b/i.test(String(pad.id || 0))) continue;
        if (pad.buttons[12]?.pressed) bits |= 1;
        if (pad.buttons[13]?.pressed) bits |= 2;
        if (pad.buttons[14]?.pressed) bits |= 4;
        if (pad.buttons[15]?.pressed) bits |= 8;
    }
    return bits;
});
namespace th08 {
void sdl_validate_capture();
namespace {
#if defined(TH_PRESENTATION_AUDIT)
struct AuditTimingSample {
    double timestamp_ms=0;
    float delta_ms=0,alpha=1;
    u32 flags=0,simulation_frame=0;
    float camera[26]{};
};
static_assert(sizeof(AuditTimingSample)==128);
constexpr u32 audit_timing_capacity=512;
AuditTimingSample audit_timing[audit_timing_capacity]{};u32 audit_timing_next=0,audit_timing_count=0;
void camera_values(float* out,const SceneCamera& c){
    for(const auto& v:{c.position,c.target_offset,c.up,c.eye_offset}){*out++=v.x;*out++=v.y;*out++=v.z;}*out=c.field_of_view;
}
#endif
std::unique_ptr<BrowserRuntime> runtime;touhou::input::TouchController touch;
struct Key{const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
SDL_Gamepad* gamepad=nullptr;u32 prepared=0;bool running=false,suspended=false;double elapsed=0,last=-1,frame_begin=0;u32 frames=0,loop_epoch=0,warm_mask=0;touhou::sdl::FrameCadence cadence;touhou::sdl::PresentationCadence display_cadence;touhou::sdl::PresentationGate presentation_gate;
constexpr SDL_GamepadButton gamepad_slots[]={
    SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_BACK,
    SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,SDL_GAMEPAD_BUTTON_GUIDE,
};
int gamepad_axis(SDL_GamepadAxis axis){const int value=SDL_GetGamepadAxis(gamepad,axis);return value<0?value*1000/32768:value*1000/32767;}
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
    if(event.type==SDL_EVENT_GAMEPAD_ADDED&&!gamepad)gamepad=SDL_OpenGamepad(event.gdevice.which);
    if(event.type==SDL_EVENT_GAMEPAD_REMOVED&&gamepad&&SDL_GetGamepadID(gamepad)==event.gdevice.which){SDL_CloseGamepad(gamepad);gamepad=nullptr;}
    }
    auto* keys=runtime->keyboard_state();std::memset(keys,0,256);const bool* physical=SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){keys[k.vk]=128;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=128;}
    if(gamepad&&SDL_GamepadConnected(gamepad)){u8 buttons[128]{};for(size_t i=0;i<sizeof(gamepad_slots)/sizeof(*gamepad_slots);++i)buttons[i]=SDL_GetGamepadButton(gamepad,gamepad_slots[i])?128:0;
        int x=gamepad_axis(SDL_GAMEPAD_AXIS_LEFTX),y=gamepad_axis(SDL_GAMEPAD_AXIS_LEFTY);
        const int dx=int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))-int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        const int dy=int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_DOWN))-int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_UP));
        if(dx)x=dx*1000;if(dy)y=dy*1000;runtime->controller_state(x,y,buttons,128,true);
    }else runtime->controller_state(0,0,nullptr,0,false);
    const int keyboardDpad=th08_keyboard_gamepad_dpad();
    if(keyboardDpad&1)keys[38]=128;if(keyboardDpad&2)keys[40]=128;if(keyboardDpad&4)keys[37]=128;if(keyboardDpad&8)keys[39]=128;
    const auto state=touch_state();sync_touch_context(state);const auto input=touch.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);for(int i=0;i<256;i++)if(input.keys[i])keys[i]=128;
    runtime->motion.target(input.motion,input.x,input.y);
    ThpracUi::update_input(*runtime);
    if(ThpracUi::captures_game_input())for(const int vk:{16,27,37,38,39,40,88,90})keys[vk]=0;
}
int tick(){poll();return !runtime||!runtime->step(false)?runtime&&(runtime->status(2)||runtime->status(4))?2:1:0;}
EM_BOOL frame(double now,void* epoch){if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double delta=last<0?0:std::max(0.,(now-last)/1000.);last=now;frame_begin=emscripten_get_now();
    if(suspended||!th08_frame_ready()){sdl_audio_pause(true);cadence.reset();display_cadence.reset();presentation_gate.reset();return EM_TRUE;}sdl_audio_pause(false);int result=0;
    const bool limit60=th08_limit_presentation_to_60()!=0;
    const bool ready=interpolation_ready()&&!limit60,fast=touhou::sdl::PresentationCadence::fast_sample(delta);if(ready)display_cadence.advance(delta);else display_cadence.reset();
    const bool tick_due=cadence.advance(delta)!=0;
    const bool high=display_cadence.high_refresh&&interpolation_ready();const bool interpolate=presentation_gate.advance(high,tick_due);bool presented=false;
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
    float presentation_alpha=1.0f;
    if(!result&&runtime&&high){
        const bool frozen=runtime->app.in_game()&&(runtime->app.game.paused||runtime->app.game.retrying||runtime->app.game.menus.context.pause_state||runtime->app.game.menus.context.show_retry);
        presentation_alpha=interpolate?float(cadence.interpolation_alpha()):1.0f;presented=runtime->app.draw(presentation_alpha,interpolate,true,!frozen);
    }
    if(presented&&runtime)runtime->app.statistics.presentation_frame();
#if defined(TH_PRESENTATION_AUDIT)
    if(runtime){auto& sample=audit_timing[audit_timing_next];sample={};sample.timestamp_ms=now;sample.delta_ms=float(delta*1000.);sample.alpha=presentation_alpha;
        sample.flags=u32(ready)|(u32(fast)<<1)|(u32(high)<<2)|(u32(tick_due)<<3)|(u32(interpolate)<<4)|(u32(presented)<<5);
        if(runtime->app.in_game()){sample.simulation_frame=u32(runtime->app.game.control.state.frames);SceneCamera previous,current;bool rebased=false;if(runtime->app.game.background_script.audit_cameras(previous,current,rebased)){sample.flags|=1u<<6;if(rebased)sample.flags|=1u<<7;camera_values(sample.camera,previous);camera_values(sample.camera+13,current);}}
        audit_timing_next=(audit_timing_next+1)%audit_timing_capacity;audit_timing_count=std::min(audit_timing_count+1,audit_timing_capacity);
    }
#endif
    sdl_audio_pump();th08_frame_finished(result,emscripten_get_now()-frame_begin);return running?EM_TRUE:EM_FALSE;
}
}
u32 sdl_game_time(){return u32(elapsed*1000);}
extern "C" {
#define EX(name) __attribute__((export_name(name)))
EX("sdl_game_open") BrowserRuntime* sdl_game_open(u32 milliseconds){if(runtime)return nullptr;prepared=frames=warm_mask=0;elapsed=double(milliseconds)/1000.;cadence.reset();display_cadence.reset();presentation_gate.reset();last=-1;touch.begin_session();
#if defined(TH_PRESENTATION_AUDIT)
    audit_timing_next=audit_timing_count=0;
#endif
    runtime=std::make_unique<BrowserRuntime>();if(!sdl_attach(runtime.get())||!sdl_load_assets(*runtime)){runtime.reset();sdl_detach();return nullptr;}
    SDL_InitSubSystem(SDL_INIT_GAMEPAD);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);int count=0;auto* ids=SDL_GetGamepads(&count);if(count)gamepad=SDL_OpenGamepad(ids[0]);SDL_free(ids);return runtime.get();}
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
EX("sdl_loop_start") void sdl_loop_start(){if(running||!runtime)return;running=true;last=-1;cadence.reset();display_cadence.reset();presentation_gate.reset();emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));}
EX("sdl_loop_stop") void sdl_loop_stop(){running=false;++loop_epoch;sdl_audio_pause(true);}
EX("sdl_loop_pause") void sdl_loop_pause(u32 pause){suspended=pause!=0;last=-1;cadence.reset();display_cadence.reset();presentation_gate.reset();sdl_audio_pause(suspended);}
EX("sdl_loop_time") double sdl_loop_time(){return elapsed;}
#if defined(TH_PRESENTATION_AUDIT)
// Diagnostic freeze is NOT the in-game pause. It retains the last endpoint pair
// and never polls input, advances clocks, executes ANM/ECL, or pumps audio.
EX("audit_draw") i32 audit_draw(float alpha,u32 world){
    if(running||!runtime||!std::isfinite(alpha)||alpha<0||alpha>1)return -1;
    if(runtime->app.title.modal())return -2; // No Draw occurs: do not reuse stale captures.
    return runtime->app.draw(alpha,true,true,world!=0)?0:1;
}
EX("audit_gate") u32 audit_gate(){return interpolation_ready()?1:0;}
EX("audit_world_frozen") u32 audit_world_frozen(){return runtime&&runtime->app.in_game()&&(runtime->app.game.paused||runtime->app.game.retrying||runtime->app.game.menus.context.pause_state||runtime->app.game.menus.context.show_retry);}
EX("audit_scene") const i32* audit_scene(){
    static i32 out[16]{};std::fill(out,out+16,0);if(!runtime)return out;const auto& a=runtime->app;const auto& g=a.game;
    out[0]=a.supervisor.state.active;out[1]=g.globals.stage;out[2]=g.globals.difficulty;out[3]=g.globals.shot;
    out[4]=g.playback.stream.frame;out[5]=g.enemies.state.frames;out[6]=g.control.state.frames;
    out[7]=g.globals.spell_flags;out[8]=g.globals.spell_number;out[9]=g.globals.current_spell;
    out[10]=a.session.numbers.score;out[11]=g.control.state.load_state;out[12]=g.globals.game_flags;
    out[13]=g.paused;out[14]=g.retrying;out[15]=g.globals.stage_completion;return out;
}
EX("audit_spell_name") const char* audit_spell_name(){return runtime?runtime->app.game.globals.spell_name:"";}
EX("audit_effect_sample") const float* audit_effect_sample(uintptr_t object){return runtime?runtime->app.game.effect_system.audit_presentation_sample(object):nullptr;}
EX("audit_ascii_sample") const float* audit_ascii_sample(uintptr_t object){return runtime?runtime->app.ascii.audit_presentation_sample(object):nullptr;}
EX("audit_player_bomb_sample") const float* audit_player_bomb_sample(uintptr_t object,u32 part){return runtime?runtime->app.game.player.audit_bomb_presentation(object,part):nullptr;}
EX("audit_enemy_sample") const float* audit_enemy_sample(uintptr_t object,u32 part){return runtime?runtime->app.game.enemies.audit_presentation_sample(object,part):nullptr;}
EX("audit_calculation_epoch") u32 audit_calculation_epoch(){return u32(th08::presentation::calculation_epoch);}
EX("audit_timing_records") const AuditTimingSample* audit_timing_records(){return audit_timing;}
EX("audit_timing_capacity") u32 audit_timing_capacity_value(){return audit_timing_capacity;}
EX("audit_timing_count") u32 audit_timing_count_value(){return audit_timing_count;}
EX("audit_timing_next") u32 audit_timing_next_value(){return audit_timing_next;}
EX("audit_timing_stride") u32 audit_timing_stride(){return sizeof(AuditTimingSample);}
// Selected authoritative-state fingerprints, independent of render captures.
// They deliberately omit renderer caches/pointers/padding and wall-clock data.
// This is a named evidence set, not a claim to serialize the entire game.
EX("audit_state") const u32* audit_state(){
    static u32 out[9];std::fill(out,out+9,2166136261u);if(!runtime)return out;auto& a=runtime->app;auto& g=a.game;
    const auto add=[](u32& h,const auto& value){const auto* p=reinterpret_cast<const u8*>(&value);for(size_t i=0;i<sizeof(value);++i){h^=p[i];h*=16777619u;}};
    const auto vm=[&](u32& h,const AnmVm& v){add(h,v.pos);add(h,v.pos2);add(h,v.scale);add(h,v.rotation);add(h,v.color1.d3dColor);add(h,v.color2.d3dColor);add(h,v.uvScrollPos);add(h,v.scriptIndex);add(h,v.activeSpriteIndex);add(h,v.currentTimeInScript.current);};
    add(out[0],a.session.random.seed);add(out[0],a.session.random.calls);add(out[0],a.session.numbers);
    add(out[1],g.player_state.motion.movement.position);add(out[1],g.player_state.life.state);add(out[1],g.player_state.input.buttons);
    for(const auto& b:g.projectile_pool.bullets){add(out[2],b.state);if(b.state){add(out[2],b.position);add(out[2],b.angle);add(out[2],b.active_time.current);}}
    for(const auto& l:g.projectile_pool.lasers){add(out[3],l.in_use);if(l.in_use){add(out[3],l.position);add(out[3],l.angle);add(out[3],l.start_offset);add(out[3],l.end_offset);add(out[3],l.timer.current);}}
    for(const auto* first:g.enemies.state.layers){u32 n=0;for(auto* e=first;e&&++n<=480;e=e->next_in_layer){add(out[4],e->resolved_position);add(out[4],e->direction);add(out[4],e->lifetime.current);add(out[4],e->main_context.subroutine);}}
    for(const auto& e:g.effect_pool.objects){add(out[5],e.active);if(e.active){add(out[5],e.position);add(out[5],e.center);add(out[5],e.radius);add(out[5],e.angle);add(out[5],e.age.current);}}
    for(i32 i=0;i<a.title.menus.state.vmCount;++i)vm(out[6],a.title.menus.state.vms[i]);
    add(out[7],a.supervisor.state.active);add(out[7],a.supervisor.state.target);add(out[7],g.control.state.frames);add(out[7],g.globals.game_flags);add(out[7],g.globals.stage);
    add(out[8],g.background.camera.position);add(out[8],g.background.camera.target_offset);add(out[8],g.background.camera.eye_offset);
    return out;
}
#endif
EX("sdl_loop_tick") i32 sdl_loop_tick(BrowserRuntime* r,double seconds,u32){
    if(running||r!=runtime.get())return -1;elapsed+=seconds;int result=tick();if(result||!runtime)return result;
    if(!runtime->app.draw(1.0f,false,false))return (runtime->status(2)||runtime->status(4))?2:1;
    if(runtime->status(2)||runtime->status(4))return 2;
    ++frames;return runtime->audio_tick(u32(elapsed*1000))?0:2;
}
EX("sdl_game_close") void sdl_game_close(){sdl_loop_stop();touch.reset();ThpracUi::shutdown();runtime.reset();if(gamepad)SDL_CloseGamepad(gamepad);gamepad=nullptr;sdl_audio_shutdown();sdl_fonts_shutdown();sdl_detach();}
EX("sdl_key") void sdl_key(const char* code,u32 down){for(auto& key:keyboard_map)if(!std::strcmp(key.code,code)){key.hosted=down!=0;break;}}
EX("sdl_keys_clear") void sdl_keys_clear(){for(auto& key:keyboard_map)key.hosted=false;cancel_touch();touch.reset();}
EX("sdl_touch") void sdl_touch(u32 type,i32 id,float x,float y){
    if(ThpracUi::captures_game_input())ThpracUi::mouse(type==0?1:type==1?0:2,x*640.f,y*480.f);
    pointer(type,id,x,y);
}
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
