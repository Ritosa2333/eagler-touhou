#include "../platform/Application.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <cmath>
#include "FrameCadence.hpp"
#include "PresentationCadence.hpp"
#include "Renderer.hpp"
#include "../game/PresentationAudit.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>

extern "C" void sdl_audio_pump();
extern "C" void sdl_audio_pause(th10::u32);
extern "C" void sdl_native_input(th10::browser::Application*);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
extern "C" void sdl_audio_replay_seek_output(th10::u32);
extern "C" th10::u32 sdl_audio_replay_seek_tick();
extern "C" int sdl_replay_seek_batch(th10::browser::Application*);
#endif
EM_JS(int, browser_prepare_frame, (), { return Module['runtimePrepare'] ? Module['runtimePrepare']() : 0; });
EM_JS(int, th10_limit_presentation_to_60, (), { return Module['eaglerOptions']?.limitPresentationTo60 ? 1 : 0; });
EM_JS(void, browser_finish_frame, (int result,double milliseconds), { Module['runtimeFinish'](result,milliseconds); });
EM_JS(void, browser_loop_stopped, (), { if(Module['runtimeStopped'])Module['runtimeStopped'](); });
namespace {
th10::browser::Application* application=nullptr;
unsigned loop_epoch=0;bool running=false,suspended=false,presentation_primed=false;double elapsed=0,last=-1,audio_remainder=0,callback_begin=0;touhou::sdl::FrameCadence cadence;touhou::sdl::PresentationCadence presentation;
#ifdef TH_PRESENTATION_AUDIT
bool presentation_lab_fault=false;
struct PresentationLabTiming {double timestamp_ms;float delta_ms,alpha;th10::u32 flags,tick,draw_serial,reserved;};
PresentationLabTiming presentation_lab_timing[512]{};th10::u32 presentation_lab_timing_count=0,presentation_lab_timing_next=0;
th10::u32 presentation_lab_state_words[7]{};
th10::u32 replay_verifier_words[24]{};
void record_presentation_timing(double timestamp,double delta,float alpha,th10::u32 flags){auto& row=presentation_lab_timing[presentation_lab_timing_next];row={timestamp,static_cast<float>(delta*1000),alpha,flags,th10::presentation_audit::current_tick(),th10::presentation_audit::current_draw_serial(),0};presentation_lab_timing_next=(presentation_lab_timing_next+1)%512;if(presentation_lab_timing_count<512)++presentation_lab_timing_count;}
void reset_presentation_timing(){presentation_lab_timing_count=presentation_lab_timing_next=0;presentation_lab_fault=false;}
th10::u32 hash_bytes(const void* data,std::size_t bytes,th10::u32 hash=2166136261u){const auto* input=static_cast<const unsigned char*>(data);for(std::size_t i=0;i<bytes;++i){hash^=input[i];hash*=16777619u;}return hash;}
template<class T>th10::u32 hash_value(const T& value,th10::u32 hash=2166136261u){return hash_bytes(&value,sizeof(value),hash);}
th10::u32 hash_anm_authored(const th10::AnmManager& manager){
    th10::u32 hash=2166136261u;
    for(th10::u32 index=0;index<4096;++index){if(!manager.occupied[index])continue;const auto& vm=manager.pool[index];
        hash=hash_value(index,hash);hash=hash_value(vm.id,hash);hash=hash_value(vm.owner_tag,hash);hash=hash_value(vm.rotation,hash);hash=hash_value(vm.angular_velocity,hash);
        hash=hash_value(vm.scale,hash);hash=hash_value(vm.scale_velocity,hash);hash=hash_value(vm.sprite_size,hash);hash=hash_value(vm.uv_offset,hash);hash=hash_value(vm.script_timer,hash);hash=hash_value(vm.script_timer_flags,hash);
        hash=hash_value(vm.position_interpolation,hash);hash=hash_value(vm.color_interpolation,hash);hash=hash_value(vm.alpha_interpolation,hash);hash=hash_value(vm.rotation_interpolation,hash);hash=hash_value(vm.scale_interpolation,hash);hash=hash_value(vm.color2_interpolation,hash);hash=hash_value(vm.alpha2_interpolation,hash);hash=hash_value(vm.uv_velocity,hash);
        hash=hash_value(vm.color,hash);hash=hash_value(vm.secondary_color,hash);hash=hash_value(vm.pending_interrupt,hash);hash=hash_value(vm.animation_file,hash);hash=hash_bytes(vm.integer_variables,sizeof(vm.integer_variables),hash);hash=hash_bytes(vm.float_variables,sizeof(vm.float_variables),hash);hash=hash_bytes(vm.extra_integer_variables,sizeof(vm.extra_integer_variables),hash);
        hash=hash_value(vm.script_position,hash);hash=hash_value(vm.position,hash);hash=hash_value(vm.child_position,hash);hash=hash_value(vm.geometry,hash);const th10::u32 authored_flags=vm.flags&~12u;hash=hash_value(authored_flags,hash);hash=hash_value(vm.saved_timer,hash);hash=hash_value(vm.saved_timer_flags,hash);hash=hash_value(vm.saved_instruction,hash);hash=hash_value(vm.sprite_frame,hash);hash=hash_value(vm.sprite_index,hash);hash=hash_value(vm.file_index,hash);hash=hash_value(vm.script_index,hash);hash=hash_value(vm.script_begin,hash);hash=hash_value(vm.instruction,hash);hash=hash_value(vm.sprite,hash);
    }return hash;
}
th10::u32 hash_world_presentation(const th10::browser::World* world){
    if(!world)return 0;th10::u32 hash=hash_value(world->player_presentation);hash=hash_bytes(world->bullet_presentation,sizeof(world->bullet_presentation),hash);hash=hash_bytes(world->item_regular_presentation,sizeof(world->item_regular_presentation),hash);hash=hash_bytes(world->item_faith_presentation,sizeof(world->item_faith_presentation),hash);hash=hash_bytes(world->popup_presentation,sizeof(world->popup_presentation),hash);
    for(const auto& entry:world->laser_presentation){hash=hash_value(entry.first,hash);hash=hash_value(entry.second,hash);}return hash;
}
th10::u32 hash_replay_cursor(const th10::Replay* replay){
    if(!replay)return 0;th10::u32 hash=hash_value(replay->flags);hash=hash_value(replay->manager_state,hash);hash=hash_value(replay->mode,hash);hash=hash_value(replay->active_buffer,hash);hash=hash_value(replay->recorded_fps,hash);hash=hash_value(replay->elapsed,hash);hash=hash_value(replay->active_stage,hash);
    for(const auto& reader:replay->readers){hash=hash_value(reader.input_cursor,hash);hash=hash_value(reader.rate_cursor,hash);hash=hash_value(reader.frame,hash);}return hash;
}
#endif
bool interpolation_ready(){if(!application||application->stopped||!application->initialized||application->world&&application->world->loading)return false;const th10::i32 screen=application->value.screen;return screen==1||screen==4||screen==7||screen==14;}
EM_BOOL frame(double timestamp,void* epoch){
    if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;
    const double now=timestamp/1000.,delta=last<0?0:std::max(0.,now-last);last=now;callback_begin=emscripten_get_now();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Keep handshake, input/ACK repair and retirement alive while loading,
    // suspended or waiting for input. Network timing is real wall time, not
    // sdl_loop_time(), whose simulation clock legitimately stops here.
    if(!application->multiplayer_pump_network()){
        browser_finish_frame(2,emscripten_get_now()-callback_begin);return running?EM_TRUE:EM_FALSE;
    }
#endif
    // Browser responsibilities end at resource readiness and input snapshots.
    // The C++ ApplicationLoop owns deadlines, logic, draw and the original
    // 60Hz cadence, independent of display callback frequency.
    const int ready=browser_prepare_frame();if(!running)return EM_FALSE;
    if(ready<=0||suspended){sdl_audio_pause(1);cadence.reset();presentation.reset();presentation_primed=false;return EM_TRUE;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // This callback has passed its own readiness/suspension fence. Release
    // only that pause; an active Replay seek retains its separate mute owner.
    sdl_audio_pause(0);
    if(application->multiplayer_replay_seeking()){
        const int result=sdl_replay_seek_batch(application);
        // Seek is a requested offline Replay operation, not catch-up debt.
        // Resume the existing live/SP cadence with no accumulated backlog.
        cadence.reset();presentation.reset();presentation_primed=false;last=-1;
        browser_finish_frame(result,emscripten_get_now()-callback_begin);
        return running?EM_TRUE:EM_FALSE;
    }
#endif
    sdl_audio_pause(0);elapsed+=delta;audio_remainder+=delta*1000;
    const auto milliseconds=th10::u32(std::floor(audio_remainder));audio_remainder-=milliseconds;
    application->audio.advance(milliseconds);
    if(application->world&&application->world->loading){
        // Share the exact object creation sequence with the synchronous test
        // entry point, but yield between owners on the browser thread. Keep
        // gameplay/RNG/recording frozen until the original loading barrier.
        const double deadline=emscripten_get_now()+2.;
        do{application->world->advance_loading_step();}while(application->world->loading&&emscripten_get_now()<deadline);
        application->sync_views();
        if(application->world->loading){application->engine.device.present_frame();sdl_audio_pump();browser_finish_frame(0,emscripten_get_now()-callback_begin);cadence.reset();presentation.reset();presentation_primed=false;return EM_TRUE;}
    }
    // Some Emscripten SDL builds fall back to millisecond gettimeofday for
    // performance counters. Use the display's timestamp for cadence, avoiding
    // a late/early callback's CPU work moving the next deadline across a VSync.
    // Match TH08: locked presentation uses the authored fixed-tick draw, with
    // no extra interpolated draws. Keep logic/input and audio cadence intact.
    const bool limit60=th10_limit_presentation_to_60()!=0;
    const bool presentation_ready=interpolation_ready()&&!limit60,fast=touhou::sdl::PresentationCadence::fast_sample(delta);if(presentation_ready)presentation.advance(delta);else presentation.reset();if(!presentation.high_refresh||!fast)presentation_primed=false;
    double simulation_delta=delta;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto& runtime=application->state.netplay_runtime;
    if(application->multiplayer_active()&&runtime.NetworkEnabled()&&!runtime.Spectator())
        simulation_delta/=runtime.Channel().SimulationIntervalScale();
#endif
    // Clock calibration changes only when the next fixed tick is due. Every
    // admitted tick still executes the original 60 Hz simulation and Draw.
    const bool tick_due=cadence.advance(simulation_delta)!=0;int result=0;sdl_defer(1);
    if(tick_due){
        sdl_native_input(application);result=application->step(true);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // A start-time spectator may receive the relay's bounded confirmed
        // history after joining. Consume a few exact logical frames per
        // display callback until caught up; live players never enter here.
        for(th10::u32 i=1;!result&&i<application->multiplayer_spectator_catchup_budget();++i)
            result=application->step(true);
#endif
    }
    const bool high=presentation.high_refresh&&interpolation_ready();if(high&&fast&&!presentation_primed&&tick_due)presentation_primed=true;const bool interpolate=high&&fast&&presentation_primed;float frame_alpha=1.0f;
    sdl_defer(0);bool presented=false;
    if(!result&&high){const bool frozen=application->world&&application->world->actors.session&&(application->world->actors.session->session_flags&0x74);frame_alpha=interpolate?float(cadence.interpolation_alpha()):1.0f;presented=application->presentation_draw(frame_alpha,interpolate,!frozen);}else presented=sdl_commit()!=0;
    if(presented&&application)application->presentation_frame();
#ifdef TH_PRESENTATION_AUDIT
    record_presentation_timing(timestamp,delta,frame_alpha,(ready>0?1u:0u)|(fast?2u:0u)|(high?4u:0u)|(tick_due?8u:0u)|(interpolate?16u:0u)|(presented?32u:0u));
#endif
    sdl_audio_pump();browser_finish_frame(result,emscripten_get_now()-callback_begin);
    return running?EM_TRUE:EM_FALSE;
}
}
extern "C" {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
int sdl_replay_seek_batch(th10::browser::Application* app){
    if(!app||app->stopped||!app->multiplayer_replay_seeking())return -1;
    sdl_audio_replay_seek_output(1);sdl_defer(1);
    int result=0;
    // Bounded UI work for the new stage-selection feature. Each iteration
    // still executes one original Update + authored Draw; no input, time,
    // stage, state or RNG is skipped. Live multiplayer/SP never enter here.
    for(unsigned i=0;i<4&&app->multiplayer_replay_seeking();++i){
        elapsed+=1.0/60.0;audio_remainder+=1000.0/60.0;
        const auto milliseconds=th10::u32(std::floor(audio_remainder));audio_remainder-=milliseconds;
        app->audio.advance(milliseconds);sdl_native_input(app);result=app->step(true);
        if(result||!sdl_audio_replay_seek_tick()){if(!result){app->error=-7;result=2;}break;}
    }
    sdl_defer(0);
    if(result||!app->multiplayer_replay_seeking()){
        sdl_audio_replay_seek_output(0);
        if(!result){if(sdl_commit())app->presentation_frame();sdl_audio_pump();}
    }
    return result;
}
#endif
__attribute__((export_name("sdl_loop_time"))) double sdl_loop_time(){return elapsed+(running&&!suspended?std::max(0.,emscripten_get_now()-callback_begin)/1000.:0.);}
// A deterministic, stopped-loop entry point for replay/regression runners.
// It shares native input, audio progression and the same Application tick.
__attribute__((export_name("sdl_loop_tick"))) int sdl_loop_tick(th10::browser::Application* app,double seconds,th10::u32 milliseconds){
    if(running)return -1;elapsed+=seconds;app->audio.advance(milliseconds);sdl_native_input(app);return app->step(true);
}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(th10::u32 pause){suspended=pause!=0;last=-1;cadence.reset();presentation.reset();presentation_primed=false;sdl_audio_pause(pause);}
__attribute__((export_name("sdl_loop_start"))) void sdl_loop_start(th10::browser::Application* app){
    application=app;elapsed=audio_remainder=0;cadence.reset();presentation.reset();presentation_primed=false;last=-1;callback_begin=emscripten_get_now();running=true;
    emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
__attribute__((export_name("sdl_loop_stop"))) void sdl_loop_stop(){
    if(!running&&!application)return;running=false;++loop_epoch;sdl_audio_pause(1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    sdl_audio_replay_seek_output(0);
#endif
    browser_loop_stopped();application=nullptr;
}
#ifdef TH_PRESENTATION_AUDIT
// Diagnostic freeze is intentionally distinct from runtime shutdown. Preserve
// the application and cadence state, but invalidate the outstanding browser
// callback. The first resumed callback has zero wall delta, so time spent in
// the inspector cannot become catch-up debt.
__attribute__((export_name("presentation_lab_freeze"))) int presentation_lab_freeze(th10::browser::Application* app){
    if(!app||application!=app)return -1;if(!running)return 0;
    running=false;++loop_epoch;last=-1;sdl_audio_pause(1);return 0;
}
__attribute__((export_name("presentation_lab_resume"))) int presentation_lab_resume(th10::browser::Application* app){
    if(!app||application!=app)return -1;if(running)return 0;
    last=-1;callback_begin=emscripten_get_now();running=true;sdl_audio_pause(suspended?1:0);
    emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));return 0;
}
__attribute__((export_name("presentation_lab_tick"))) int presentation_lab_tick(th10::browser::Application* app){
    if(running||!app||application!=app)return -1;
    constexpr double seconds=1./60.;elapsed+=seconds;audio_remainder+=seconds*1000.;
    const auto milliseconds=th10::u32(std::floor(audio_remainder));audio_remainder-=milliseconds;
    app->audio.advance(milliseconds);sdl_native_input(app);return app->step(true);
}
__attribute__((export_name("presentation_lab_draw"))) int presentation_lab_draw(th10::browser::Application* app,float alpha,th10::u32 world){
    if(running||!app||application!=app||!interpolation_ready()||!std::isfinite(alpha)||alpha<0||alpha>1)return -1;
    return app->presentation_draw(presentation_lab_fault?1.0f:alpha,true,world!=0)?0:1;
}
__attribute__((export_name("presentation_lab_fault"))) void presentation_lab_set_fault(th10::u32 value){presentation_lab_fault=value!=0;}
__attribute__((export_name("presentation_lab_gate"))) th10::u32 presentation_lab_gate(){return interpolation_ready()?1:0;}
__attribute__((export_name("presentation_lab_world_frozen"))) th10::u32 presentation_lab_world_frozen(th10::browser::Application* app){return app&&app->world&&app->world->actors.session&&(app->world->actors.session->session_flags&0x74)?1:0;}
__attribute__((export_name("audit_timing_records"))) const PresentationLabTiming* audit_timing_records(){return presentation_lab_timing;}
__attribute__((export_name("audit_timing_capacity"))) th10::u32 audit_timing_capacity(){return 512;}
__attribute__((export_name("audit_timing_count"))) th10::u32 audit_timing_count(){return presentation_lab_timing_count;}
__attribute__((export_name("audit_timing_next"))) th10::u32 audit_timing_next(){return presentation_lab_timing_next;}
__attribute__((export_name("audit_timing_stride"))) th10::u32 audit_timing_stride(){return sizeof(PresentationLabTiming);}
__attribute__((export_name("audit_timing_reset"))) void audit_timing_reset(){reset_presentation_timing();}
__attribute__((export_name("audit_state"))) const th10::u32* audit_state(th10::browser::Application* app){
    if(!app)return nullptr;const auto& state=app->state;const auto& value=app->value;
    th10::u32 lifecycle=hash_value(value.screen);lifecycle=hash_value(value.pending_screen,lifecycle);lifecycle=hash_value(value.previous_screen,lifecycle);lifecycle=hash_value(value.engine_flags,lifecycle);lifecycle=hash_value(value.frame_gate,lifecycle);lifecycle=hash_value(value.loading_state,lifecycle);lifecycle=hash_value(app->stopped,lifecycle);lifecycle=hash_value(app->initialized,lifecycle);
    th10::u32 simulation=hash_value(state.game);simulation=hash_value(app->engine.script_random,simulation);simulation=hash_value(app->engine.visual_random,simulation);simulation=hash_bytes(state.keyboard,sizeof(state.keyboard),simulation);simulation=hash_bytes(state.previous_keyboard,sizeof(state.previous_keyboard),simulation);simulation=hash_bytes(state.pressed_keyboard,sizeof(state.pressed_keyboard),simulation);simulation=hash_value(state.motion,simulation);
    const th10::u32 anm=hash_anm_authored(*app->manager),sidecars=hash_world_presentation(app->world),replay=hash_replay_cursor(state.replay);
    th10::u32 host=hash_value(elapsed);host=hash_value(audio_remainder,host);host=hash_value(cadence.debt,host);host=hash_value(presentation.fast,host);host=hash_value(presentation.slow,host);host=hash_value(presentation.high_refresh,host);host=hash_value(presentation_primed,host);host=hash_value(running,host);host=hash_value(suspended,host);
    presentation_lab_state_words[0]=1;presentation_lab_state_words[1]=31;presentation_lab_state_words[2]=lifecycle;presentation_lab_state_words[3]=simulation;presentation_lab_state_words[4]=anm;presentation_lab_state_words[5]=sidecars;presentation_lab_state_words[6]=hash_value(host,replay);return presentation_lab_state_words;
}
// Read-only Replay verifier evidence for diagnostic builds. This does not
// schedule work, consume input, advance Replay state or alter the title-owned
// Demo mechanism.
__attribute__((export_name("replay_verifier_trace"))) const th10::u32* replay_verifier_trace(th10::browser::Application* app){
    std::fill(replay_verifier_words,replay_verifier_words+24,0);if(!app)return replay_verifier_words;
    const auto& game=app->state.game;const auto* replay=app->state.replay;const auto* world=app->world;
    replay_verifier_words[0]=1;replay_verifier_words[1]=th10::u32(app->value.screen);replay_verifier_words[2]=th10::u32(app->value.pending_screen);
    replay_verifier_words[3]=th10::u32(game.stage);replay_verifier_words[4]=game.stage_frames;replay_verifier_words[5]=game.section_frames;replay_verifier_words[6]=game.flags;
    replay_verifier_words[7]=th10::u32(game.score);replay_verifier_words[8]=th10::u32(th10::i32(game.power));replay_verifier_words[9]=th10::u32(game.item_value);
    replay_verifier_words[10]=th10::u32(game.lives);replay_verifier_words[11]=th10::u32(game.rank);replay_verifier_words[12]=app->engine.script_random.seed;replay_verifier_words[13]=app->engine.script_random.calls;
    if(replay){replay_verifier_words[14]=th10::u32(replay->mode);replay_verifier_words[15]=th10::u32(replay->active_stage);if(replay->active_stage>=0&&replay->active_stage<8)replay_verifier_words[16]=th10::u32(replay->readers[replay->active_stage].frame);}
    replay_verifier_words[17]=app->input.player_profiles[0].input.current;
    if(world&&world->actors.player){const auto& player=*world->actors.player;std::memcpy(replay_verifier_words+18,&player.position,sizeof(float)*2);replay_verifier_words[20]=th10::u32(player.state);}
    if(world&&world->actors.session)replay_verifier_words[21]=world->actors.session->session_flags;
    replay_verifier_words[22]=world&&world->actors.session?1:0;replay_verifier_words[23]=app->value.screen==app->value.pending_screen?1:0;return replay_verifier_words;
}
#endif
}
