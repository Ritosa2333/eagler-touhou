#include "RollbackDriver.hpp"
#include "ResourceTrace.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../platform/PlatformDevices.hpp"
#include <algorithm>
#include <cstdio>
#include <emscripten/emscripten.h>

namespace th08::multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
namespace {bool fixture_skip_resim_visual_enabled=false,fixture_skip_resim_geometry_enabled=false;}
bool fixture_skip_resim_visual(bool enabled){fixture_skip_resim_visual_enabled=enabled;return enabled;}
bool fixture_skip_resim_geometry(bool enabled){fixture_skip_resim_geometry_enabled=enabled;return enabled;}
#endif
namespace {
u8 route_state(const GameplaySession& session,const SessionSetup& setup){
    if(session.multiplayer_route_state<=2)return session.multiplayer_route_state;
    if(setup.characters[0]>=12)return 0;
    const auto& record=session.clears[setup.characters[0]];
    bool cleared_a=false,cleared_b=setup.characters[0]>3;
    for(i32 difficulty=0;difficulty<4;++difficulty){
        cleared_a|=bool(record.with_retries[difficulty]&64);
        cleared_b|=bool(record.without_retries[difficulty]&128);
    }
    return cleared_b?2:cleared_a?1:0;
}
}
RollbackDriver::RollbackDriver(BrowserRuntime& r):runtime(r),network(r.app.session.netplay){
#ifdef TH_MULTIPLAYER_FIXTURES
    skip_resim_visual=fixture_skip_resim_visual_enabled;
    skip_resim_geometry=fixture_skip_resim_geometry_enabled;
#endif
}
const char* RollbackDriver::Error()const{return failed?error:network.Error();}
bool RollbackDriver::FailNativeUpdate(u32 frame,bool updated){
    const auto& a=runtime.app;
    u32 guests=0;
    for(u32 seat=0;seat<2;++seat){const auto& guest=a.game.guest_pilots[seat];
        if(guest){if(guest->services.invalid())guests|=1u<<(seat*2);if(guest->simulation.invalid())guests|=2u<<(seat*2);}}
    std::snprintf(native_error,sizeof(native_error),
        "native update failed: frame=%u stage=%u scene=%d target=%d chain=%d updated=%u capture=%u gameFaults=0x%03x guestFaults=0x%x appFaults=0x%x",
        frame,a.game.globals.stage,a.supervisor.state.active,a.supervisor.state.target,a.last_update_result,u32(updated),
        u32(runtime.capture_failed),a.game.faults(),guests,
        u32(a.animations.invalid)|(u32(a.title.invalid())<<1)|(u32(a.results.invalid())<<2)|(u32(a.screen.invalid())<<3));
    return Fail(native_error);
}
bool RollbackDriver::Connect(const char* relay){return !failed&&network.Connect(relay);}
bool RollbackDriver::ConnectSpectator(const char* relay,const char* id){return !failed&&network.ConnectSpectator(relay,id);}
bool RollbackDriver::Pump(){
    const auto& a=runtime.app;
    return !failed&&(network.Pump(a.session.multiplayer_session.started&&!a.loading_game())||Fail("network channel failed"));
}
bool RollbackDriver::Reconcile(){
    if(failed||open||!Pump())return false;
    if(!initialized||!runtime.app.session.netplay.CanStart())return true;
    return Correct()&&Commit();
}
bool RollbackDriver::PresentCorrection(){
    if(!corrected_present_pending)return true;
    corrected_present_pending=false;
    return graphics_device().present(runtime.back)||Fail("corrected presentation failed");
}
bool RollbackDriver::Stable()const{
    const auto& a=runtime.app;
    return a.in_game()&&!a.loading_game()&&!a.invalid()&&a.supervisor.state.active==i32(th08::Scene::Game)&&
        a.supervisor.state.target==i32(th08::Scene::Game)&&a.game.menus.context.supervisor_state==i32(th08::Scene::Game)&&
        !a.supervisor.state.close_requested&&!(a.game.globals.game_flags&8);
}
bool RollbackDriver::apply_file_event(const char* path,const u8* data,u32 size){
    return runtime.resources_.put(path,data,size)&&file_device().save(path,data,size);
}
bool RollbackDriver::Commit(){
    auto& net=runtime.app.session.netplay;
    if(net.Correcting()||net.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
    auto through=net.ConfirmedThrough();const auto last=net.LastFrame();
    if(through!=Netplay::INVALID_FRAME&&last!=Netplay::INVALID_FRAME&&bound){
        const auto next=std::min(through,last)+1;world.DiscardBefore(next);textures.DiscardBefore(next);
    }
    // A partially confirmed interval can still rewind its exact prefix.
    // Defer irreversible outputs until the whole checkpoint is retired.
    const auto before=world.FirstCheckpoint();
    if(before!=Netplay::INVALID_FRAME&&through!=Netplay::INVALID_FRAME&&before<=through)
        through=before?before-1:Netplay::INVALID_FRAME;
    if(!runtime.commit_audio_events(audio,through,last)||!files.CommitThrough(through,last,*this))return Fail("confirmed external output failed");
    if(!runtime.replay_archive.Commit(net,[](void* context,const char* path,const u8* bytes,u32 size){
        return static_cast<RollbackDriver*>(context)->apply_file_event(path,bytes,size);
    },this,before))return Fail("confirmed Replay commit failed");
    network.PublishConfirmedSpectatorFrames();
    return true;
}
bool RollbackDriver::Admit(){
    auto& a=runtime.app;auto& net=a.session.netplay;
    if(net.Spectator()){
        // Observers have exact all-seat input. They use the same native
        // lifecycle, but never allocate prediction history or invent a new
        // session when the observed players select Restart.
        const auto target=th08::Scene(a.supervisor.state.target);
        if(a.supervisor.state.active==i32(th08::Scene::Game)&&
           (target==th08::Scene::Restart||target==th08::Scene::SpellRestart)){
            if(!net.CanRetire()){a.session.network_waiting=true;return true;}
            if(!Commit())return false;
            network.FinishSpectator();a.session.network_waiting=true;
        }
        return true;
    }
    if(bound&&!world.CanAdvance()){
        // Do not execute the next scene's resource destructors until the frame
        // that selected that scene is reconciled and confirmed.
        if(!net.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit())return false;
        world.Clear();textures.Clear();bound=false;
        if(!net.SetWorldReady(false))return Fail("world retirement failed");
    }
    const auto target=th08::Scene(a.supervisor.state.target);
    if(!generation_transition_pending&&a.supervisor.state.active==i32(th08::Scene::Game)&&
       (target==th08::Scene::Restart||target==th08::Scene::SpellRestart)){
        if(!network.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit()||!network.Retire())return Fail("generation retirement failed");
        SessionSetup next;
        if(!net.BeginNextRun(next,a.session.random.seed)||!network.BeginGeneration())return Fail("generation bootstrap failed");
        if(!runtime.replay_archive.NextGeneration(net.Generation()))return Fail("Replay generation failed");
        next.started=true;a.session.multiplayer_session=next;
        a.session.random={u16(next.seed),u16(next.seed),0};
        audio.Reset();files.Reset();generation=net.Generation();generation_transition_pending=true;
        a.session.network_waiting=true;return true;
    }
    if(!net.Playback()&&!bound&&Stable()){
        if(!world.Bind(runtime)||!textures.Bind(runtime)||!net.SetWorldReady(true))return Fail("production owner bootstrap failed");
        bound=true;
    }
    return true;
}
bool RollbackDriver::RunFrame(bool render){
    auto& a=runtime.app;auto& session=a.session;auto& net=session.netplay;
    const auto frame=net.NextFrame();auto decision=net.Prepare(frame);
#ifdef TH_MULTIPLAYER_FIXTURES
    // Measurement control only: keep the native simulation/render workload,
    // but wait for exact input instead of predicting. Step still captures and
    // transmits input before admission; this is not a shipped netplay policy.
    if(diagnostic_exact_only&&decision.predictedMask){session.network_waiting=true;return true;}
#endif
    if(!decision.canAdvance){session.network_waiting=true;return true;}
    if(frame==0){
        u8 route=0;if(!DecodeRouteBootstrap(decision.inputs[0],route))return Fail("missing route bootstrap");
        session.multiplayer_route_state=route;
    }
    if(runtime.replay_archive.Recording()&&!runtime.replay_archive.BeginFrame(frame))return Fail("Replay frame stamp failed");
    const auto confirmed=net.ConfirmedThrough();
    checkpoint_open=bound&&(confirmed==Netplay::INVALID_FRAME||frame>confirmed);
#ifdef TH_MULTIPLAYER_FIXTURES
    checkpoint_open=checkpoint_open||(bound&&diagnostic_always_snapshot);
    const auto capture_start=emscripten_get_now();
#endif
    // No future correction can target an exact, contiguous confirmed prefix.
    // Release its older records before running without an open checkpoint, so
    // native first-write hooks see no outstanding rewindable ownership.
    if(bound&&!checkpoint_open){world.DiscardBefore(frame+1);textures.DiscardBefore(frame+1);}
    const bool extend=checkpoint_open&&world.CanExtend(frame,checkpoint_span);
    checkpoint_previous_bytes=extend?world.BytesForFrame(frame-1)+textures.BytesForFrame(frame-1):0;
#ifdef TH_MULTIPLAYER_FIXTURES
    checkpoint_previous_owner={};
    if(extend){const auto prior=world.DiagnosticBytesForFrame(frame-1);for(u32 i=0;i<5;++i)checkpoint_previous_owner[i]=prior[i];checkpoint_previous_owner[5]=textures.BytesForFrame(frame-1);}
#endif
    if(checkpoint_open){
        if(!world.BeginFrame(frame,extend))return Fail("begin world frame failed");
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto texture_started=emscripten_get_now();
#endif
        if(!textures.BeginFrame(frame,extend))return Fail("begin world frame failed");
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_texture_begin_ms+=emscripten_get_now()-texture_started;
#endif
    }
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_capture_ms+=emscripten_get_now()-capture_start;
    if(checkpoint_open&&!extend)++diagnostic_snapshots;else if(bound)++diagnostic_skipped;
#endif
    if(!audio.BeginFrame(frame)||!files.BeginFrame(frame))return Fail("begin output frame failed");
    open=true;session.network_frame=decision;session.network_frame_open=true;
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::Begin(runtime,frame,decision.predictedMask,correcting);
#endif
    if(a.in_game()){
        auto gameplay=decision.inputs;if(frame==0)gameplay[0]=StripRouteBootstrap(gameplay[0]);
        if(!a.game.commit_frame_inputs(gameplay.data(),session.player_count))return Fail("committed input handoff failed");
    }
    if(decision.predictedMask)++predicted;
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto update_start=emscripten_get_now();
#endif
    const bool updated=a.update();
    if(!updated||runtime.capture_failed)return FailNativeUpdate(frame,updated);
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_update_ms+=emscripten_get_now()-update_start;
#endif
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::Event("frame.after_update");
#endif
    if(generation_transition_pending&&a.supervisor.state.target==i32(th08::Scene::Game))generation_transition_pending=false;
    if(!render)return true;
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto draw_start=emscripten_get_now();
#endif
    const bool drawn=a.draw();
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_draw_ms+=emscripten_get_now()-draw_start;
#endif
    return drawn&&FinishFrame();
}
bool RollbackDriver::FinishFrame(){
    if(failed||!open)return false;
    auto& session=runtime.app.session;auto& net=session.netplay;const auto frame=net.NextFrame();
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::End(runtime);
#endif
    if((checkpoint_open&&(!world.EndFrame()||!textures.EndFrame()))||!audio.EndFrame()||!files.EndFrame())return Fail("end frame ownership failed");
    if(!net.MarkSimulated(frame,session.network_frame))return Fail("simulated input mismatch");
    const auto stage=u32(runtime.app.game.globals.stage);
    if(runtime.replay_archive.Recording()&&!runtime.replay_archive.Stamp(frame,stage,session.numbers.score))return Fail("Replay stage stamp failed");
    if(net.Playback()&&!runtime.replay_archive.AdvancePlayback(frame,stage))return Fail("Replay native stage diverged");
    if(checkpoint_open){
        const auto size=world.BytesForFrame(frame)+textures.BytesForFrame(frame);
        max_bytes=std::max(max_bytes,u32(size));
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_snapshot_bytes+=size-checkpoint_previous_bytes;
        const auto owners=world.DiagnosticBytesForFrame(frame);
        for(u32 i=0;i<5;++i)diagnostic_owner_bytes[i]+=owners[i]-checkpoint_previous_owner[i];
        diagnostic_owner_bytes[5]+=textures.BytesForFrame(frame)-checkpoint_previous_owner[5];
#endif
    }
    checkpoint_open=false;
    open=false;session.network_frame_open=false;++runtime.multiplayer_logic_frame;
    if(!correcting)corrected_present_pending=false;
    return correcting||Commit();
}
bool RollbackDriver::Correct(){
    auto& net=runtime.app.session.netplay;const auto requested=net.RollbackFrame();
    if(requested==Netplay::INVALID_FRAME)return true;
    const auto first=world.CheckpointStart(requested);
    const auto end=net.NextFrame();
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto restore_start=emscripten_get_now();
#endif
    if(!bound||first==Netplay::INVALID_FRAME||!net.BeginCorrection(first,checkpoint_span)||!world.UndoTo(first)||!textures.UndoTo(first)||
       !audio.DiscardFrom(first)||!files.DiscardFrom(first))return Fail("rollback restore failed");
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_restore_ms+=emscripten_get_now()-restore_start;
#endif
    runtime.correction_present_suppressed=true;correcting=true;++corrections;
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::Restored(runtime,first);
#endif
    bool last_visual_suppressed=false;
    while(net.NextFrame()<end&&world.CanAdvance()){
        const auto before=net.NextFrame();
        runtime.correction_visual_suppressed=skip_resim_visual&&before+1<end;
        runtime.app.renderer.rollback_visual_geometry_suppressed=skip_resim_geometry&&runtime.correction_visual_suppressed;
        const bool ok=RunFrame(true);
        last_visual_suppressed=runtime.correction_visual_suppressed;
        runtime.correction_visual_suppressed=false;
        runtime.app.renderer.rollback_visual_geometry_suppressed=false;
        if(!ok||net.NextFrame()==before){correcting=false;runtime.correction_present_suppressed=false;return Fail("resimulation stalled");}
        ++resimulated;
    }
    const bool boundary=net.NextFrame()!=end;
    if(boundary&&last_visual_suppressed&&runtime.app.active()&&
       !runtime.app.draw(1.0f,true,true,false)){
        correcting=false;runtime.correction_present_suppressed=false;return Fail("correction boundary redraw failed");
    }
#ifdef TH_MULTIPLAYER_FIXTURES
    if(boundary&&last_visual_suppressed&&runtime.app.active())++diagnostic_boundary_visual_redraws;
#endif
    if(!net.EndCorrection(boundary)){correcting=false;runtime.correction_present_suppressed=false;return Fail("correction frontier failed");}
    correcting=false;runtime.correction_present_suppressed=false;
    corrected_present_pending=true;
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto correction_ms=emscripten_get_now()-restore_start;
    diagnostic_correction_ms+=correction_ms;
    diagnostic_correction_max_ms=std::max(diagnostic_correction_max_ms,correction_ms);
#endif
    return Commit();
}
bool RollbackDriver::CaptureLocalInput(){
    auto& session=runtime.app.session;auto& net=session.netplay;
    const auto frame=net.NextCaptureFrame();
    if(!net.CanCapture()||net.HasCapture(frame))return false;
    const auto touch=u16(file_device().supplemental_input());
    const auto physical=runtime.input.controller(InputController::keyboard(runtime.keys,false)|touch,runtime.pad,session.display_config);
    auto sample=runtime.device_sample(physical);
    if(frame==0&&net.Setup().local_player==0){
        const auto route=route_state(session,net.Setup());
        if(net.Setup().input_delay){
            if(!net.CaptureLeadInBootstrap(RouteBootstrap({},route)))return false;
        }else sample=RouteBootstrap(sample,route);
    }
    return net.CaptureLocal(frame,sample);
}
bool RollbackDriver::Step(bool render){
    auto& session=runtime.app.session;auto& net=session.netplay;
    if(failed||open||session.network_frame_open||!runtime.prepared||runtime.capture_failed)return false;
    session.network_waiting=false;
    if(!Pump())return false;
    if(network.SpectatorFinished()){session.network_waiting=true;return true;}
    if(!net.CanStart()){session.network_waiting=true;return true;}
    if(net.Playback()&&(runtime.replay_finished||runtime.replay_archive.Complete())){
        runtime.replay_finished=true;session.network_waiting=true;return true;
    }
    if(!initialized){audio.Reset(net.NextFrame());files.Reset(net.NextFrame());
        if(!runtime.begin_replay_recording())return Fail("Replay recording bootstrap failed");
        if(!runtime.bind_audio_events(&audio))return Fail("audio sink binding failed");
        initialized=true;generation=net.Generation();}
    // TH07's early once-only send rule: network threads can transmit while
    // this endpoint restores/replays. Restrict it to the current live world;
    // loading, retirement and generation bootstrap retain their admission fence.
    u32 sent_frame=Netplay::INVALID_FRAME,sent_generation=net.Generation();
    bool early_input=bound&&world.CanAdvance()&&!net.ReadOnly();
#ifdef TH_MULTIPLAYER_FIXTURES
    early_input=early_input&&diagnostic_early_input;
#endif
    if(early_input&&net.CanCapture()){
        if(!CaptureLocalInput())return Fail("local capture failed");
        sent_frame=net.NextCaptureFrame()-1;
        if(!network.Captured(sent_frame))return Fail("captured input send failed");
    }
    if(!Correct()||!Commit()||!Admit())return false;
    if(session.network_waiting)return PresentCorrection();
    // Bounded observer catch-up executes the original semantic Update/Draw
    // jobs, with no device resampling and no intermediate physical Present.
    // The device audio clock advances only in the outer SDL callback.
    for(u32 extra=0;net.Spectator()&&extra<3&&Stable()&&network.SpectatorBacklog()>4;++extra){
        if(!network.ConsumeSpectator())return Fail("observer catch-up input rejected");
        runtime.correction_present_suppressed=true;
        const auto before=net.NextFrame();const bool ok=RunFrame(true);
        runtime.correction_present_suppressed=false;
        if(!ok||net.NextFrame()==before)return Fail("observer catch-up stalled");
        if(!Admit())return false;
        if(session.network_waiting)return true;
    }
    const auto frame=net.NextFrame();
    if(!net.ReadOnly()&&(sent_frame==Netplay::INVALID_FRAME||net.Generation()!=sent_generation)&&net.CanCapture()){
        if(!CaptureLocalInput())return Fail("local capture failed");
        sent_frame=net.NextCaptureFrame()-1;
        if(!network.Captured(sent_frame))return Fail("captured input send failed");
    }
    if(!net.HasLocalFrame(frame)){
        if(net.Playback()){
            const auto* inputs=runtime.replay_archive.PlaybackFrame(frame);
            if(!inputs||!net.FeedPlayback(frame,*inputs))return Fail("Replay input rejected");
        }else if(net.Spectator()){
            if(!network.SpectatorBacklog()){session.network_waiting=true;return true;}
            if(!network.ConsumeSpectator())return Fail("observer input consumption failed");
        }else{session.network_waiting=true;return PresentCorrection();}
    }
    if(!RunFrame(render))return false;
    return !session.network_waiting||PresentCorrection();
}
void RollbackDriver::Shutdown(){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::Disable(runtime);
#endif
    if(!initialized&&!bound&&!network.Enabled())return;
    runtime.correction_present_suppressed=false;
    runtime.correction_visual_suppressed=false;
    // Shutdown may arrive while the displayed state is still predicted. Never
    // serialize it as a completed personal record.
    runtime.discard_network_shutdown_writes=true;
    world.Clear();textures.Clear();bound=false;
    runtime.bind_audio_events(nullptr);audio.Reset();files.Reset();network.Close();
    runtime.app.session.network_frame_open=false;open=initialized=false;
}
}
