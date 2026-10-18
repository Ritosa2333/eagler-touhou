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
#include <emscripten/eventloop.h>
#include <eagler/netplay/FrameBudget.hpp>
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include "RafDeadlineGuard.hpp"
#include "NetworkTailPresentation.hpp"
#include "../multiplayer/NetworkFrameSchedule.hpp"
#include "../multiplayer/LiveMotionObservation.hpp"
#include "../multiplayer/CallbackWorkTiming.hpp"
extern "C" void sdl_audio_replay_seek_output(th10::u32);
extern "C" th10::u32 sdl_audio_replay_seek_tick();
extern "C" int sdl_replay_seek_batch(th10::browser::Application*);
#endif
EM_JS(int, browser_prepare_frame, (), { return Module['runtimePrepare'] ? Module['runtimePrepare']() : 0; });
EM_JS(int, th10_limit_presentation_to_60, (), { return Module['eaglerOptions']?.limitPresentationTo60 ? 1 : 0; });
EM_JS(void, browser_finish_frame, (int result,double milliseconds), { Module['runtimeFinish'](result,milliseconds); });
EM_JS(void, browser_loop_stopped, (), { if(Module['runtimeStopped'])Module['runtimeStopped'](); });
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
EM_JS(int, th10_raf_guard_visible, (), {
    return typeof document!=='undefined'&&!document.hidden?1:0;
});
#endif
namespace {
th10::browser::Application* application=nullptr;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
using HostCadence=Netplay::PendingFrameSchedule<touhou::sdl::FrameCadence>;
namespace work_timing=th10::multiplayer::callback_work_timing;
#else
using HostCadence=touhou::sdl::FrameCadence;
#endif
unsigned loop_epoch=0;bool running=false,suspended=false,presentation_primed=false;double elapsed=0,last=-1,audio_remainder=0,callback_begin=0;HostCadence cadence;touhou::sdl::PresentationCadence presentation;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
th10::multiplayer::NetworkFrameSchedule network_schedule;
bool network_identity_valid=false;std::uint64_t network_session=0;th10::u32 network_generation=0;
double multiplayer_presentation_sample[6]{1,0,0,1,0,0};
using RafGuard=th10::host::RafDeadlineGuard;
RafGuard raf_guard;
int guarded_raf_handle=0,guarded_timer_handle=0;
bool guarded_raf_pending=false,guarded_timer_pending=false;
bool guarded_clock_ready=false;
void cancel_guarded_requests();
void cancel_guarded_timer();
void request_guarded_raf();
struct LiveMotionCallback {
    th10::browser::Application* owner;
    const bool active;
    explicit LiveMotionCallback(th10::browser::Application* app):
        owner(app),active(th10::multiplayer::live_motion::enabled){
        if(active){
            const auto* renderer=touhou::sdl::current();
            th10::multiplayer::live_motion::BeginCallback(app,renderer?renderer->stats.presentations:0u);
        }
    }
    ~LiveMotionCallback(){if(active)th10::multiplayer::live_motion::EndCallback(owner);}
};
#endif
// Optional host-only attribution. No world traversal, heap allocation or new
// clock reads occur while disabled. The switch is latched at callback entry;
// getters expose only the last fully published row, including during Finish.
bool callback_timing_enabled=false;
double callback_timing_sequence=0;
double callback_timing_sample[25]{2};
enum class CallbackTimingExit : unsigned {
    Normal=0,NetworkFailure=1,RetiredAfterPrepare=2,NotReadyOrSuspended=3,
    ReplaySeek=4,FlushFailure=5,Loading=6,RetiredDuringCatchup=7,StaleCallback=8
};
struct CallbackTiming {
    // Schema 2 retains schema 1's first 19 fields. All durations are in
    // milliseconds. Offered/completed are callback-local Host cadence values;
    // spectator extra steps are separate.
    // InputStep includes native input plus Application::step, whereas MaxStep
    // includes only one Application::step (and its complete semantic Draw).
    // Those step fields cover direct Host/spectator calls. ReplaySeekWork is
    // an opaque batch, including its own ticks/audio, with no double counting.
    // FlushPresentation also includes early flush/pause cleanup; Total retains
    // the remaining host bookkeeping and diagnostic overhead between phases.
    enum Field : unsigned {
        Version,Sequence,Offered,Completed,NetworkPrepare,InputStep,
        FlushPresentation,AudioPump,Total,MaxStep,StepCalls,SpectatorStepCalls,
        LoadingWork,ReplaySeekWork,ExitReason,FinishResult,FinishCalled,
        Rollbacks,ResimulatedFrames,
        PresentationGateMs,ScheduledInputGapScale,PresentationScheduleAlpha,
        AuthoredBoundaryFlags,TailPresentationDecision,TailPresentationCommitted,Count
    };
    static_assert(ResimulatedFrames==18&&PresentationGateMs==19&&Count==25);
    const bool active;
    const double began;
    const uintptr_t epoch;
    th10::browser::Application* const owner;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    work_timing::Token work_token=0;
    th10::u32 work_generation=0;
#endif
    double values[Count];
    CallbackTiming(double start,void* token):active(callback_timing_enabled),
        began(start),epoch(uintptr_t(token)),owner(application){
        if(!active)return;
        std::fill(values,values+Count,0.);
        values[Version]=2;values[Sequence]=++callback_timing_sequence;
        values[FinishResult]=-1;values[Rollbacks]=values[ResimulatedFrames]=-1;
        // -1 means the original rollback presentation gate was not reached.
        // Decision 0 is not evaluated; committed is 0/1 for one verified commit.
        values[PresentationGateMs]=values[ScheduledInputGapScale]=-1;
        values[PresentationScheduleAlpha]=values[AuthoredBoundaryFlags]=-1;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(work_timing::state.Enabled()){
            const bool owner_valid=owner&&application==owner&&epoch==loop_epoch;
            work_generation=owner_valid?owner->state.netplay_runtime.Generation():0u;
            work_token=work_timing::state.BeginCallback(active,values[Sequence],epoch,
                reinterpret_cast<std::uintptr_t>(owner),work_generation,
                owner_valid?owner->state.netplay_runtime.LastSimulatedFrame():work_timing::InvalidFrame,
                []{return emscripten_get_now();});
        }
#endif
    }
    double start() const {return active?emscripten_get_now():0.;}
    void add_at(Field field,double start,double end){
        if(active&&field<Count)values[field]+=std::max(0.,end-start);
    }
    void add(Field field,double start){
        if(active)add_at(field,start,emscripten_get_now());
    }
    void progress(unsigned offered,unsigned completed){
        if(active){values[Offered]=offered;values[Completed]=completed;}
    }
    void tail_gate(double gate_ms,double gap_scale,double alpha,unsigned flags,unsigned decision){
        if(!active)return;
        values[PresentationGateMs]=gate_ms;values[ScheduledInputGapScale]=gap_scale;
        values[PresentationScheduleAlpha]=alpha;values[AuthoredBoundaryFlags]=flags;
        values[TailPresentationDecision]=decision;
    }
    void tail_committed(bool committed){
        if(active)values[TailPresentationCommitted]=committed?1.:0.;
    }
    void step(double input_start,double step_start,bool spectator=false){
        if(!active)return;
        const double end=emscripten_get_now();
        add_at(InputStep,input_start,end);
        values[MaxStep]=std::max(values[MaxStep],std::max(0.,end-step_start));
        ++values[StepCalls];if(spectator)++values[SpectatorStepCalls];
    }
    void publish(CallbackTimingExit reason,int result,bool finished,double end){
        if(!active)return;
        values[Total]=std::max(0.,end-began);
        values[ExitReason]=unsigned(reason);values[FinishResult]=result;
        values[FinishCalled]=finished?1:0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // Preparation can stop/replace the app. Never dereference a retired
        // owner merely to report diagnostics; -1 explicitly means unavailable.
        if(owner&&application==owner&&epoch==loop_epoch){
            values[Rollbacks]=owner->multiplayer_rollbacks;
            values[ResimulatedFrames]=owner->multiplayer_resimulated_frames;
        }
        if(work_token){
            // Identity is checked before reading the final frontier. Never
            // attribute a replacement app or generation to an old callback.
            const bool owner_valid=owner&&application==owner&&epoch==loop_epoch&&
                owner->state.netplay_runtime.Generation()==work_generation;
            work_timing::state.Publish(work_token,values[InputStep],unsigned(reason),owner_valid,
                owner_valid?owner->state.netplay_runtime.LastSimulatedFrame():work_timing::InvalidFrame);
        }
#endif
        std::copy(values,values+Count,callback_timing_sample);
    }
    void finish(int result,CallbackTimingExit reason,Field pending=Count,double start=0.){
        // Replaces the pre-existing Finish timestamp, even when disabled.
        // Publish BEFORE runtimeFinish reads the row; its JS work is excluded.
        const double end=emscripten_get_now();
        add_at(pending,start,end);publish(reason,result,true,end);
        browser_finish_frame(result,end-callback_begin);
    }
    void returned(CallbackTimingExit reason,Field pending=Count,double start=0.){
        if(!active)return;
        const double end=emscripten_get_now();
        add_at(pending,start,end);publish(reason,-1,false,end);
    }
};
void reset_cadence(){
    cadence.reset();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    network_schedule.reset();network_identity_valid=false;
#endif
}
bool flush_network_inputs(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(application&&!application->state.netplay_runtime.FlushNetworkInputs()){
        application->multiplayer_fail("Flush display input tail");return false;
    }
#endif
    return true;
}
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
    if(!running||uintptr_t(epoch)!=loop_epoch){
        if(callback_timing_enabled){
            CallbackTiming timing(emscripten_get_now(),epoch);
            timing.returned(CallbackTimingExit::StaleCallback);
        }
        return EM_FALSE;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled())guarded_clock_ready=false;
#endif
    const double now=timestamp/1000.,delta=last<0?0:std::max(0.,now-last);last=now;callback_begin=emscripten_get_now();
    CallbackTiming timing(callback_begin,epoch);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    LiveMotionCallback motion_observation(application);
    // Keep handshake, input/ACK repair and retirement alive while loading,
    // suspended or waiting for input. Network timing is real wall time, not
    // sdl_loop_time(), whose simulation clock legitimately stops here.
    if(!application->multiplayer_pump_network(true)){
        timing.finish(2,CallbackTimingExit::NetworkFailure,CallbackTiming::NetworkPrepare,timing.began);return running?EM_TRUE:EM_FALSE;
    }
#endif
    // Browser responsibilities end at resource readiness and input snapshots.
    // The C++ ApplicationLoop owns deadlines, logic, draw and the original
    // 60Hz cadence, independent of display callback frequency.
    const int ready=browser_prepare_frame();
    timing.add(CallbackTiming::NetworkPrepare,timing.began);
    if(!running||!application||uintptr_t(epoch)!=loop_epoch){timing.returned(CallbackTimingExit::RetiredAfterPrepare);return EM_FALSE;}
    if(ready<=0||suspended){
        const double flush_started=timing.start();
        flush_network_inputs();sdl_audio_pause(1);reset_cadence();presentation.reset();presentation_primed=false;
        timing.returned(CallbackTimingExit::NotReadyOrSuspended,CallbackTiming::FlushPresentation,flush_started);return EM_TRUE;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // This callback has passed its own readiness/suspension fence. Release
    // only that pause; an active Replay seek retains its separate mute owner.
    if(raf_guard.enabled())guarded_clock_ready=true;
    sdl_audio_pause(0);
    if(application->multiplayer_replay_seeking()){
        const double flush_started=timing.start();
        if(!flush_network_inputs()){timing.finish(2,CallbackTimingExit::FlushFailure,CallbackTiming::FlushPresentation,flush_started);return running?EM_TRUE:EM_FALSE;}
        timing.add(CallbackTiming::FlushPresentation,flush_started);
        const double seek_started=timing.start();
        const int result=sdl_replay_seek_batch(application);
        timing.add(CallbackTiming::ReplaySeekWork,seek_started);
        // Seek is a requested offline Replay operation, not catch-up debt.
        // Resume the existing live/SP cadence with no accumulated backlog.
        reset_cadence();presentation.reset();presentation_primed=false;last=-1;
        timing.finish(result,CallbackTimingExit::ReplaySeek);
        return running?EM_TRUE:EM_FALSE;
    }
#endif
    sdl_audio_pause(0);elapsed+=delta;audio_remainder+=delta*1000;
    const auto milliseconds=th10::u32(std::floor(audio_remainder));audio_remainder-=milliseconds;
    application->audio.advance(milliseconds);
    if(application->world&&application->world->loading){
        const double flush_started=timing.start();
        if(!flush_network_inputs()){timing.finish(2,CallbackTimingExit::FlushFailure,CallbackTiming::FlushPresentation,flush_started);return running?EM_TRUE:EM_FALSE;}
        timing.add(CallbackTiming::FlushPresentation,flush_started);
        // Share the exact object creation sequence with the synchronous test
        // entry point, but yield between owners on the browser thread. Keep
        // gameplay/RNG/recording frozen until the original loading barrier.
        const double loading_started=timing.start();
        const double deadline=emscripten_get_now()+2.;
        do{application->world->advance_loading_step();}while(application->world->loading&&emscripten_get_now()<deadline);
        application->sync_views();
        timing.add(CallbackTiming::LoadingWork,loading_started);
        if(application->world->loading){
            const double present_started=timing.start();
            application->engine.device.present_frame();
            timing.add(CallbackTiming::FlushPresentation,present_started);
            const double audio_started=timing.start();sdl_audio_pump();
            timing.finish(0,CallbackTimingExit::Loading,CallbackTiming::AudioPump,audio_started);
            reset_cadence();presentation.reset();presentation_primed=false;return EM_TRUE;
        }
    }
    const bool limit60=th10_limit_presentation_to_60()!=0;
    const bool presentation_ready=interpolation_ready()&&!limit60;
    double presentation_delta=delta;bool sample_presentation_cadence=true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()&&raf_guard.callback_active()&&raf_guard.epoch()==loop_epoch){
        // Timer service time belongs to simulation/audio only. A timeout and
        // its shortly following RAF must not manufacture fast display samples.
        presentation_delta=raf_guard.last_real_raf_delta_ms()/1000.;
        sample_presentation_cadence=raf_guard.source()==RafGuard::Source::Raf;
    }
#endif
    const bool fast=touhou::sdl::PresentationCadence::fast_sample(presentation_delta);
    if(presentation_ready){if(sample_presentation_cadence)presentation.advance(presentation_delta);}
    else presentation.reset();
    double simulation_delta=delta;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    auto& runtime=application->state.netplay_runtime;
    const bool live=application->multiplayer_active()&&runtime.NetworkEnabled()&&
                    runtime.CanStart()&&!runtime.Spectator()&&runtime.InitialInputsReady();
    const bool rollback_clock=live&&runtime.Mode()==Netplay::AdonisMode::Rollback;
    if(rollback_clock){
        if(!network_identity_valid||network_session!=runtime.Config().sessionId||
           network_generation!=runtime.Generation()){
            network_schedule.reset();presentation_primed=false;network_identity_valid=true;
            network_session=runtime.Config().sessionId;network_generation=runtime.Generation();
        }
    }else {network_schedule.reset();network_identity_valid=false;}
    if(live&&!cadence.retry_pending())simulation_delta/=runtime.IntervalScale();
    if(live&&!cadence.retry_pending()&&cadence.debt+1.e-9<touhou::sdl::FrameCadence::interval)
        simulation_delta=runtime.PacedElapsedSeconds(simulation_delta);
    // Scale only new wall time. Keep fractional phase in nominal logic units.
    // Capture the input-information scale here, not after step/repair and not
    // the possibly larger peer phase scale folded into IntervalScale().
    const double scheduled_input_gap_scale=rollback_clock?runtime.InputGapScale():1.;
    const unsigned offered=rollback_clock?network_schedule.advance(delta/runtime.IntervalScale(),true):
        cadence.advance(simulation_delta,live);
    if(!presentation.high_refresh||(!rollback_clock&&!fast))presentation_primed=false;
#else
    const unsigned offered=cadence.advance(simulation_delta);
    if(!presentation.high_refresh||!fast)presentation_primed=false;
#endif
    const bool tick_due=offered!=0;
    int result=0;unsigned completed=0;bool completed_after_correction=false;
    timing.progress(offered,completed);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool unfinished_accounted=false;
#endif
    sdl_defer(1);
    while(completed<offered&&!result){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(rollback_clock&&!Netplay::FrameBudget::CanStartTick(completed,
            std::uint64_t(std::max(0.,emscripten_get_now()-callback_begin)*1.e6)))break;
        // An additional admitted tick still gets one fresh input opportunity.
        if(completed){
            const double prepare_started=timing.start();
            const int additional_ready=browser_prepare_frame();
            timing.add(CallbackTiming::NetworkPrepare,prepare_started);
            // Preparation may stop/replace this loop and release its app.
            // Never finish the old callback through that retired lifetime.
            if(!running||!application||uintptr_t(epoch)!=loop_epoch){
                sdl_defer(0);timing.returned(CallbackTimingExit::RetiredDuringCatchup);return EM_FALSE;
            }
            if(additional_ready<=0||suspended)break;
        }
        const auto before=runtime.LastSimulatedFrame();
        const auto corrections=application->multiplayer_rollbacks;
#endif
        const double input_started=timing.start();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        work_timing::InputRegion work_input;
#endif
        sdl_native_input(application);
        const double step_started=timing.start();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        result=application->step(true,callback_begin);
        timing.step(input_started,step_started);
        work_input.Close();
        if(runtime.Correcting()||application->multiplayer_rollbacks!=corrections){
            presentation_primed=false;completed_after_correction=false;
        }
        const bool advanced=application->multiplayer_forward_advanced;
        if(rollback_clock){
            if(!advanced){
                if(runtime.Correcting())
                    network_schedule.recovering(offered-completed);
                else network_schedule.blocked(true);
                unfinished_accounted=true;break;
            }
            network_schedule.complete();++completed;completed_after_correction=true;
        }else{
            if(live&&!result&&runtime.LastSimulatedFrame()!=before+1u)cadence.blocked(true);
            else cadence.complete();
            ++completed;completed_after_correction=advanced;
        }
#else
        result=application->step(true);timing.step(input_started,step_started);++completed;completed_after_correction=true;
#endif
        timing.progress(offered,completed);
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(rollback_clock&&!unfinished_accounted&&completed<offered)
        network_schedule.defer(offered-completed);
    // Spectator backlog is confirmed history, separate from live recovery.
    if(!rollback_clock&&tick_due){
        for(th10::u32 i=1;!result&&application->multiplayer_spectator_catchup_budget()>1&&
            Netplay::FrameBudget::CanStartTick(i,std::uint64_t(std::max(0.,emscripten_get_now()-callback_begin)*1.e6));++i){
            const auto before=runtime.LastSimulatedFrame();
            const double step_started=timing.start();
            work_timing::InputRegion work_input;
            result=application->step(true,callback_begin);
            timing.step(step_started,step_started,true);
            work_input.Close();
            if(runtime.LastSimulatedFrame()!=before+1u)break;
        }
    }
#endif
    const double present_started=timing.start();
    if(!flush_network_inputs())result=2;
    const bool high=presentation.high_refresh&&interpolation_ready();
    float frame_alpha=1.0f;bool interpolate=false,presented=false;
    sdl_defer(0);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(runtime.StagePlaybackReady()){
        // Confirmed spectator packets own their semantic Draw individually.
        // A browser repaint is read-only even during pause or catch-up.
        if(!result)presented=application->presentation_draw(1.f,false,false);
    }else if(rollback_clock){
        if(high&&completed_after_correction)presentation_primed=true;
        interpolate=high&&presentation_primed;
        if(!result&&!runtime.Correcting()){
            // Keep the same gate clock and the global 10 ms threshold. Only
            // the default-OFF network presentation may spend one final Draw in
            // [10,14) ms; it never admits another input/update/correction.
            const double presentation_gate_ms=emscripten_get_now()-callback_begin;
            const bool budget_spent=presentation_gate_ms>=10.;
            const bool corrected=application->multiplayer_corrected_presentation_pending;
            const double schedule_alpha=network_schedule.presentation_alpha();
            using TailPresentation=th10::host::NetworkTailPresentation;
            const auto boundary_flags=completed?application->multiplayer_authored_boundary.flags:
                TailPresentation::Unobserved;
            const bool complete_frontier=completed_after_correction&&!corrected&&
                application->multiplayer_forward_advanced&&!application->multiplayer_frame_open&&
                application->world&&!application->world->rollback_resimulating&&
                !application->engine.rollback_state;
            const auto tail_decision=TailPresentation::Decide({runtime.NetworkTailPresentationEnabled(),
                sample_presentation_cadence&&fast,high,completed,complete_frontier,
                scheduled_input_gap_scale,schedule_alpha,presentation_gate_ms,boundary_flags});
            const bool tail_exception=tail_decision==TailPresentation::Decision::Accept;
            timing.tail_gate(presentation_gate_ms,scheduled_input_gap_scale,schedule_alpha,
                boundary_flags,unsigned(tail_decision));
            if(completed_after_correction&&(!high||(budget_spent&&!tail_exception))){
                // Full semantic pixels already exist. Once their endpoint was
                // shown, do not interpolate backwards through the same pair.
                presented=sdl_commit()!=0;
                if(presented){presentation_primed=false;interpolate=false;}
            }else if(high&&(!corrected||!budget_spent)){
                const bool frozen=application->world&&application->world->actors.session&&
                    (application->world->actors.session->session_flags&0x74);
                frame_alpha=interpolate?float(schedule_alpha):1.0f;
                // Optional attribution uses existing native commit counters;
                // no extra clock, JS getter, sampling allocation or flush.
                const auto* observed_renderer=tail_exception&&timing.active?touhou::sdl::current():nullptr;
                const auto commits_before=observed_renderer?observed_renderer->stats.presentations:0u;
                presented=application->presentation_draw(frame_alpha,interpolate,!frozen);
                if(tail_exception&&timing.active)timing.tail_committed(presented&&observed_renderer&&
                    observed_renderer==touhou::sdl::current()&&
                    observed_renderer->stats.presentations==commits_before+1u);
            }else if(corrected&&!budget_spent){
                // Historical Draw can suppress GPU output. Only after full
                // reconciliation may a read-only pass show the corrected world.
                presented=application->presentation_draw(1.0f,false,false);
            }
            if(presented)application->multiplayer_corrected_presentation_pending=false;
        }
    }else
#endif
    {
        if(high&&fast&&!presentation_primed&&tick_due)presentation_primed=true;
        interpolate=high&&fast&&presentation_primed;
        if(!result&&high){
            const bool frozen=application->world&&application->world->actors.session&&
                (application->world->actors.session->session_flags&0x74);
            frame_alpha=interpolate?float(cadence.interpolation_alpha()):1.0f;
            presented=application->presentation_draw(frame_alpha,interpolate,!frozen);
        }else presented=sdl_commit()!=0;
    }
    if(presented&&application)application->presentation_frame();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(presented&&application&&th10::multiplayer::live_motion::enabled){
        const auto* renderer=touhou::sdl::current();
        th10::multiplayer::live_motion::Commit(application,renderer?renderer->stats.presentations:0u);
    }
    multiplayer_presentation_sample[1]=high?1:0;
    multiplayer_presentation_sample[2]=interpolate?1:0;
    multiplayer_presentation_sample[3]=frame_alpha;
    multiplayer_presentation_sample[4]=presented?1:0;
    multiplayer_presentation_sample[5]=presentation_primed?1:0;
#endif
#ifdef TH_PRESENTATION_AUDIT
    record_presentation_timing(timestamp,delta,frame_alpha,(ready>0?1u:0u)|(fast?2u:0u)|(high?4u:0u)|(tick_due?8u:0u)|(interpolate?16u:0u)|(presented?32u:0u));
#endif
    timing.add(CallbackTiming::FlushPresentation,present_started);
    const double audio_started=timing.start();sdl_audio_pump();
    timing.finish(result,CallbackTimingExit::Normal,CallbackTiming::AudioPump,audio_started);
    return running?EM_TRUE:EM_FALSE;
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
// Browser handles never enter the world or the rollback journal. A stale
// callback must pass the policy's RAF request or independent timer-arm token
// before clearing a CURRENT handle. Old arms may arrive within the same RAF
// request, or after runtimeFinish stopped/restarted the loop.
void cancel_guarded_timer(){
    if(guarded_timer_pending){
        guarded_timer_pending=false;emscripten_clear_timeout(guarded_timer_handle);
    }
}
void cancel_guarded_requests(){
    cancel_guarded_timer();
    if(guarded_raf_pending){
        guarded_raf_pending=false;emscripten_cancel_animation_frame(guarded_raf_handle);
    }
}
bool guarded_timer_eligible(){
    if(!running||suspended||!guarded_clock_ready||!application||application->error||
       !interpolation_ready()||!application->world||!presentation.high_refresh||
       th10_limit_presentation_to_60()||!th10_raf_guard_visible())return false;
    // A formerly fast display that has become 30/15 Hz must not acquire a
    // timer-driven loop during the detector's deliberate slow-sample hysteresis.
    if(!touhou::sdl::PresentationCadence::fast_sample(
           raf_guard.last_real_raf_delta_ms()/1000.))return false;
    const auto* session=application->world->actors.session;
    if(!session||(session->session_flags&0x74))return false;
    const auto& runtime=application->state.netplay_runtime;
    return application->multiplayer_active()&&runtime.NetworkEnabled()&&runtime.CanStart()&&
        !runtime.Spectator()&&runtime.InitialInputsReady()&&runtime.Mode()==Netplay::AdonisMode::Rollback;
}
void sample_skipped_real_raf(const RafGuard::Dispatch& dispatch){
    if(!dispatch.sampleRaf)return;
    // A near-timer RAF is still a real display sample even though it does not
    // execute native work. Do not silently omit its long real-RAF interval.
    if(!suspended&&th10_raf_guard_visible()&&interpolation_ready()&&
       !th10_limit_presentation_to_60())
        presentation.advance(dispatch.realRafDeltaMs/1000.);
    else {presentation.reset();presentation_primed=false;}
}
EM_BOOL guarded_raf_frame(double timestamp,void* request){
    const auto dispatch=raf_guard.Raf(uintptr_t(request),timestamp,emscripten_get_now());
    if(!dispatch.current)return EM_FALSE;
    guarded_raf_pending=false;cancel_guarded_timer();
    if(dispatch.service){
        auto* const owner=application;
        const bool keep=frame(dispatch.acceptedTimestampMs,
            reinterpret_cast<void*>(dispatch.ticket.epoch))!=EM_FALSE;
        // This timestamp follows runtimeFinish and native destructors. The
        // existing timing row intentionally excludes that JS tail; the RAF
        // deadline must not mistake it for time outside the completed callback.
        if(!raf_guard.Complete(dispatch.ticket,emscripten_get_now()))return EM_FALSE;
        if(!keep||!running||!application||application!=owner||
           dispatch.ticket.epoch!=loop_epoch)return EM_FALSE;
    }else sample_skipped_real_raf(dispatch);
    request_guarded_raf();
    return EM_FALSE; // The single-shot Emscripten API ignores this return value.
}
void schedule_guarded_timer();
void guarded_timer_frame(void* arm){
    const auto token=uintptr_t(arm);
    // Clear only our own fired arm, before reading application/world state.
    // Sharing the pending RAF request does not make an old queued timer current.
    const bool current=running&&guarded_raf_pending&&guarded_timer_pending&&
        raf_guard.timer_current(token)&&!raf_guard.callback_active()&&
        raf_guard.epoch()==loop_epoch;
    if(current)guarded_timer_pending=false;
    const auto dispatch=raf_guard.Timer(token,emscripten_get_now(),
        current&&guarded_timer_eligible());
    if(!dispatch.service)return;
    auto* const owner=application;
    const bool keep=frame(dispatch.acceptedTimestampMs,
        reinterpret_cast<void*>(dispatch.ticket.epoch))!=EM_FALSE;
    // Completion includes synchronous runtimeFinish and all native destructors.
    // A replacement loop owns its own handles; never touch them from this tail.
    if(!raf_guard.Complete(dispatch.ticket,emscripten_get_now()))return;
    if(!keep||!running||!application||application!=owner||
       dispatch.ticket.epoch!=loop_epoch)return;
    // Keep ONE original RAF pending. Each successful complete service gets a
    // fresh completion-based policy deadline; no expired-deadline catch-up or interval.
    if(raf_guard.Rearm(dispatch.ticket,guarded_timer_eligible()))schedule_guarded_timer();
}
void schedule_guarded_timer(){
    if(guarded_timer_pending||!guarded_raf_pending||!raf_guard.timer_armed())return;
    guarded_timer_pending=true;
    guarded_timer_handle=emscripten_set_timeout(guarded_timer_frame,
        raf_guard.timeout_delay_ms(emscripten_get_now()),
        reinterpret_cast<void*>(raf_guard.timer_serial()));
}
void request_guarded_raf(){
    if(!running||!application||!raf_guard.enabled()||raf_guard.epoch()!=loop_epoch)return;
    const auto token=raf_guard.Request(emscripten_get_now(),guarded_timer_eligible());
    if(!token)return;
    guarded_raf_pending=true;
    guarded_raf_handle=emscripten_request_animation_frame(
        guarded_raf_frame,reinterpret_cast<void*>(token));
    schedule_guarded_timer();
}
#endif
}
extern "C" {
// Runtime opt-in, available in the same WASM as normal gameplay. Sequence zero
// means no sample yet; disabling preserves the last completed 19-double row.
__attribute__((export_name("sdl_callback_timing_set_enabled")))
void sdl_callback_timing_set_enabled(th10::u32 enabled){callback_timing_enabled=enabled!=0;}
__attribute__((export_name("sdl_callback_timing_status")))
const double* sdl_callback_timing_status(){return callback_timing_sample;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
// Independent callback-local subphases. OFF adds no clock reads; the last
// complete row survives disable. Parent timing owns the InputAndStep boundary.
__attribute__((export_name("sdl_callback_work_timing_set_enabled")))
int sdl_callback_work_timing_set_enabled(th10::u32 enabled){
    if(enabled>1u)return -2;
    if(enabled&&!callback_timing_enabled)return -3;
    return work_timing::state.SetEnabled(enabled!=0)?1:0;
}
__attribute__((export_name("sdl_callback_work_timing_status")))
const double* sdl_callback_work_timing_status(){return work_timing::state.Published();}
// A fresh Module may opt in before its FIRST loop start. Stopping measurement,
// Restart, pause, and game close do not unlock this experimental switch.
// Return -1 after lock; -2 for a value other than 0/1. Default OFF preserves
// the original emscripten_request_animation_frame_loop path below.
__attribute__((export_name("sdl_raf_guard_set_enabled")))
int sdl_raf_guard_set_enabled(th10::u32 enabled){
    if(running)return -1;
    if(enabled>1)return -2;
    return raf_guard.Configure(enabled!=0);
}
__attribute__((export_name("sdl_raf_guard_status")))
const double* sdl_raf_guard_status(){
    static double sample[RafGuard::StatusCount]{};
    raf_guard.Status(sample);return sample;
}
__attribute__((export_name("multiplayer_presentation_status")))
const double* multiplayer_presentation_status(){return multiplayer_presentation_sample;}
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
__attribute__((export_name("sdl_loop_time"))) double sdl_loop_time(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()){
        const bool advancing=running&&!suspended&&guarded_clock_ready;
        return raf_guard.HostTime(elapsed,advancing?emscripten_get_now():0.,advancing);
    }
#endif
    return elapsed+(running&&!suspended?std::max(0.,emscripten_get_now()-callback_begin)/1000.:0.);
}
// A deterministic, stopped-loop entry point for replay/regression runners.
// It shares native input, audio progression and the same Application tick.
__attribute__((export_name("sdl_loop_tick"))) int sdl_loop_tick(th10::browser::Application* app,double seconds,th10::u32 milliseconds){
    if(running)return -1;elapsed+=seconds;app->audio.advance(milliseconds);sdl_native_input(app);return app->step(true);
}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(th10::u32 pause){
    suspended=pause!=0;last=-1;reset_cadence();presentation.reset();presentation_primed=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()){guarded_clock_ready=false;cancel_guarded_timer();raf_guard.ResetAfterPause();}
#endif
    sdl_audio_pause(pause);
}
__attribute__((export_name("sdl_loop_start"))) void sdl_loop_start(th10::browser::Application* app){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()){guarded_clock_ready=false;cancel_guarded_requests();}
#endif
    application=app;elapsed=audio_remainder=0;reset_cadence();presentation.reset();presentation_primed=false;last=-1;callback_begin=emscripten_get_now();running=true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    raf_guard.Start(loop_epoch+1u);
    if(raf_guard.enabled()){++loop_epoch;request_guarded_raf();return;}
#endif
    emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
__attribute__((export_name("sdl_loop_stop"))) void sdl_loop_stop(){
    if(!running&&!application)return;running=false;++loop_epoch;sdl_audio_pause(1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()){guarded_clock_ready=false;cancel_guarded_requests();}
    raf_guard.Stop(loop_epoch);
    if(application)application->invalidate_remote_player_presentation();
    if(th10::multiplayer::live_motion::enabled)th10::multiplayer::live_motion::Stop(application);
    sdl_audio_replay_seek_output(0);
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled()){
        // runtimeStopped may synchronously start a replacement application.
        // Only this stopped loop may retire its own pointer after that hook.
        auto* const owner=application;const auto retired_epoch=loop_epoch;
        browser_loop_stopped();
        if(!running&&loop_epoch==retired_epoch&&application==owner)application=nullptr;
        return;
    }
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
    running=false;++loop_epoch;last=-1;sdl_audio_pause(1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(raf_guard.enabled())cancel_guarded_requests();
    raf_guard.Stop(loop_epoch);
#endif
    return 0;
}
__attribute__((export_name("presentation_lab_resume"))) int presentation_lab_resume(th10::browser::Application* app){
    if(!app||application!=app)return -1;if(running)return 0;
    last=-1;callback_begin=emscripten_get_now();running=true;sdl_audio_pause(suspended?1:0);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    raf_guard.Start(loop_epoch+1u);
    if(raf_guard.enabled()){++loop_epoch;request_guarded_raf();return 0;}
#endif
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
