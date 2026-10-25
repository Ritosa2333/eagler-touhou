#include "RollbackDriver.hpp"
#include "NetworkFailure.hpp"
#include "ResourceTrace.hpp"
#include "SpectatorReplayStart.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../platform/PlatformDevices.hpp"
#include <algorithm>
#include <cstdio>
#include <new>
#include <emscripten/emscripten.h>

namespace th08::multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
extern "C" const u32* multiplayer_replay_gameplay_state(BrowserRuntime* runtime);
void SnapshotCanonicalPlayerRegions();
u32 CanonicalPlayerField(u32 offset);
u32 CanonicalPlayerFieldByteOffset(u32 offset);
u32 CanonicalResourceTraceLocation(u32 offset,u32* out);
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
bool RollbackDriver::FailNetwork(const char* operation){
    if(failed)return false;
    const auto& net=runtime.app.session.netplay;const auto& channel=network.Channel();
    FormatNetworkFailure(network_error,operation,{network.Error(),network.Mode(),net.Generation(),net.NextFrame(),
        net.ConfirmedThrough(),net.RollbackFrame(),channel.LatestCapture(),u32(channel.Error()),
        channel.PacketsSent(),channel.PacketsReceived(),network.Buffered(),network.BufferedInput(),network.BufferedControl()});
    return Fail(network_error);
}
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
bool RollbackDriver::Connect(const char* relay){
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_spectator_checkpoint_frame=Netplay::INVALID_FRAME;
    diagnostic_spectator_checkpoint_canonical.fill(0);
#endif
    return !failed&&network.Connect(relay);
}
bool RollbackDriver::ConnectSpectator(const char* relay,const char* id){
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_spectator_canonical_trace.clear();
    diagnostic_spectator_checkpoint_frame=Netplay::INVALID_FRAME;
    diagnostic_spectator_checkpoint_canonical.fill(0);
#endif
    return !failed&&network.ConnectSpectator(relay,id);
}
bool RollbackDriver::Pump(bool capture_follows){
    return !failed&&(network.Pump(Stable(),capture_follows)||FailNetwork("network channel failed"));
}
bool RollbackDriver::FlushInputs(){
    return network.FlushDeferredInputs()||FailNetwork("deferred input send failed");
}
bool RollbackDriver::Reconcile(){
    if(failed||open||!Pump())return false;
    if(!initialized||!runtime.app.session.netplay.CanStart())return true;
    return (!runtime.app.session.netplay.AllowsRollback()||Correct())&&Commit();
}
bool RollbackDriver::PresentCorrection(){
    if(runtime.app.session.netplay.Correcting())return true;
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
bool RollbackDriver::EnsureJournals(){
    if(!world)world.reset(new(std::nothrow) WorldJournal);
    if(!textures)textures.reset(new(std::nothrow) TextureJournal);
    return bool(world)&&bool(textures);
}
bool RollbackDriver::apply_file_event(const char* path,const u8* data,u32 size){
    return runtime.resources_.put(path,data,size)&&file_device().save(path,data,size);
}
bool RollbackDriver::Commit(){
    auto& net=runtime.app.session.netplay;
    if(net.Correcting()||net.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
    auto through=net.ConfirmedThrough();const auto last=net.LastFrame();
    if(through!=Netplay::INVALID_FRAME&&last!=Netplay::INVALID_FRAME&&bound){
        const auto next=std::min(through,last)+1;world->DiscardBefore(next);textures->DiscardBefore(next);
    }
    // A partially confirmed interval can still rewind its exact prefix.
    // Defer irreversible outputs until the whole checkpoint is retired.
    const auto before=bound?world->FirstCheckpoint():Netplay::INVALID_FRAME;
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
    if(bound&&!world->CanAdvance()){
        // Do not execute the next scene's resource destructors until the frame
        // that selected that scene is reconciled and confirmed.
        if(!net.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit())return false;
        world->Clear();textures->Clear();bound=false;
        if(!net.SetWorldReady(false))return Fail("world retirement failed");
    }
    const auto target=th08::Scene(a.supervisor.state.target);
    if(!generation_transition_pending&&a.supervisor.state.active==i32(th08::Scene::Game)&&
       (target==th08::Scene::Restart||target==th08::Scene::SpellRestart)){
        if(!network.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit())return false;
        if(!network.Retire())return FailNetwork("generation retirement failed");
        SessionSetup next;
        if(!net.BeginNextRun(next,a.session.random.seed))return Fail("generation bootstrap failed");
        if(!network.BeginGeneration())return FailNetwork("generation bootstrap failed");
        if(!runtime.replay_archive.NextGeneration(net.Generation()))return Fail("Replay generation failed");
        next.started=true;a.session.multiplayer_session=next;
        a.session.random={u16(next.seed),u16(next.seed),0};
        spectator_stage_tags.fill({});pending_spectator_frame=Netplay::INVALID_FRAME;
        pending_spectator_initializer.clear();pending_spectator_checkpoint=false;pending_spectator_boss_slots=0;
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_spectator_checkpoint_frame=Netplay::INVALID_FRAME;
        diagnostic_spectator_checkpoint_canonical.fill(0);
#endif
        // The native teardown/loading graph writes records and emits loading
        // audio outside gameplay frames, just as at the initial bootstrap.
        // Rebind the confirmed-output journals only when the new world starts.
        runtime.bind_audio_events(nullptr);audio.Reset();files.Reset();initialized=false;
        generation=net.Generation();generation_transition_pending=true;
        a.session.network_waiting=true;return true;
    }
    if(net.AllowsRollback()&&!bound&&Stable()){
        if(!EnsureJournals()||!world->Bind(runtime)||!textures->Bind(runtime)||!net.SetWorldReady(true))return Fail("production owner bootstrap failed");
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
    if(!net.AllowsRollback()&&decision.predictedMask)return Fail("exact-input mode attempted prediction");
    if(frame==0){
        // P1's route bootstrap is authoritative metadata, not a predictable
        // gameplay control. Wait for its exact frame-zero packet.
        if(decision.predictedMask&1u){session.network_waiting=true;return true;}
        u8 route=0;if(!DecodeRouteBootstrap(decision.inputs[0],route))return Fail("missing route bootstrap");
        session.multiplayer_route_state=route;
        if(net.Setup().version>=6&&a.in_game()){
            // Pre-calibration loading uses neutral route records. The existing
            // authoritative frame-zero bootstrap installs P1's route before
            // any admitted game logic can read the stage-selection flags.
            auto& record=session.clears[net.Setup().characters[0]];
            record.with_retries[0]&=~64u;record.without_retries[0]&=~128u;
            if(route==1)record.with_retries[0]|=64;
            else if(route==2)record.without_retries[0]|=128;
        }
    }
    pending_spectator_frame=Netplay::INVALID_FRAME;pending_spectator_initializer.clear();
    pending_spectator_checkpoint=false;pending_spectator_boss_slots=0;
    if(!net.Spectator()&&net.Setup().local_player==0&&
       !network.SpectatorPublishFailed()){
        const auto stage=u32(a.game.globals.stage)|(u32(session.multiplayer_route_state)<<8);
        const auto& previous=spectator_stage_tags[(frame-1u)%spectator_stage_tags.size()];
        // The first initialized game world starts the spectator stream.
        // Later checkpoints follow stage changes or a missing preceding tag.
        // A non-checkpoint frame is still valid evidence of the same stage.
        const bool checkpoint=a.in_game()&&(!network.SpectatorPublisherStarted()||frame==0||previous.frame!=frame-1u||previous.stage!=stage);
        pending_spectator_frame=frame;pending_spectator_stage=stage;
        pending_spectator_checkpoint=checkpoint;
        u32 capture_failure=0;
        if(checkpoint&&!EncodeSpectatorReplayStart(runtime,pending_spectator_initializer,&capture_failure)){
            pending_spectator_checkpoint=false;pending_spectator_initializer.clear();
            // A stage number can change before its new GameplayLoad owner is
            // ready. Keep the old published stage tag for this transition
            // input so the first matching frame will retry the checkpoint.
            if(capture_failure>=31&&capture_failure<=35){
                if(previous.frame==frame-1u)pending_spectator_stage=previous.stage;
                else pending_spectator_frame=Netplay::INVALID_FRAME;
            }else network.DisableSpectatorPublishing();
        }
#ifdef TH_MULTIPLAYER_FIXTURES
        if(checkpoint&&!pending_spectator_initializer.empty()){
            const auto* hash=multiplayer_replay_gameplay_state(&runtime);
            if(hash&&hash[0]==1){
                diagnostic_spectator_checkpoint_frame=frame;
                std::copy(hash,hash+13,diagnostic_spectator_checkpoint_canonical.begin());
            }
        }
#endif
        a.game.globals.spectator_boss_slots=0;
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
    if(bound&&!checkpoint_open){world->DiscardBefore(frame+1);textures->DiscardBefore(frame+1);}
    const bool extend=checkpoint_open&&world->CanExtend(frame,checkpoint_span);
    checkpoint_previous_bytes=extend?world->BytesForFrame(frame-1)+textures->BytesForFrame(frame-1):0;
#ifdef TH_MULTIPLAYER_FIXTURES
    checkpoint_previous_owner={};
    if(extend){const auto prior=world->DiagnosticBytesForFrame(frame-1);for(u32 i=0;i<5;++i)checkpoint_previous_owner[i]=prior[i];checkpoint_previous_owner[5]=textures->BytesForFrame(frame-1);}
#endif
    if(checkpoint_open){
        if(!world->BeginFrame(frame,extend))return Fail("begin world frame failed");
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto texture_started=emscripten_get_now();
#endif
        if(!textures->BeginFrame(frame,extend))return Fail("begin world frame failed");
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
    if((checkpoint_open&&(!world->EndFrame()||!textures->EndFrame()))||!audio.EndFrame()||!files.EndFrame())return Fail("end frame ownership failed");
    if(!net.MarkSimulated(frame,session.network_frame))return Fail("simulated input mismatch");
    const auto stage=u32(runtime.app.game.globals.stage);
    if(pending_spectator_frame==frame){
        const auto index=frame%spectator_stage_tags.size();
        spectator_stage_tags[index]={frame,pending_spectator_stage};
        const auto schema=pending_spectator_checkpoint?SpectatorReplaySchema:0u;
        if(!network.RecordSpectatorStage(frame,pending_spectator_stage,schema,pending_spectator_initializer,
            session.network_frame.inputs,runtime.app.game.globals.spectator_boss_slots))return Fail("spectator stage record failed");
    }
    if(runtime.replay_archive.Recording()&&!runtime.replay_archive.Stamp(frame,stage,session.numbers.score))return Fail("Replay stage stamp failed");
    if(net.Playback()&&!runtime.replay_archive.AdvancePlayback(frame,stage))return Fail("Replay native stage diverged");
    if(checkpoint_open){
        const auto size=world->BytesForFrame(frame)+textures->BytesForFrame(frame);
        max_bytes=std::max(max_bytes,u32(size));
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_snapshot_bytes+=size-checkpoint_previous_bytes;
        const auto owners=world->DiagnosticBytesForFrame(frame);
        for(u32 i=0;i<5;++i)diagnostic_owner_bytes[i]+=owners[i]-checkpoint_previous_owner[i];
        diagnostic_owner_bytes[5]+=textures->BytesForFrame(frame)-checkpoint_previous_owner[5];
#endif
    }
    checkpoint_open=false;
    open=false;session.network_frame_open=false;++runtime.multiplayer_logic_frame;
    if(!correcting)corrected_present_pending=false;
    return correcting||Commit();
}
bool RollbackDriver::Correct(double callback_start_ms){
    auto& net=runtime.app.session.netplay;const auto requested=net.RollbackFrame();
    if(requested==Netplay::INVALID_FRAME&&!net.Correcting())return true;
    if(!bound||!world||!textures)return Fail("correction requested without rollback storage");
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto slice_start=emscripten_get_now();
    const auto record_cost=[&](){const auto ms=emscripten_get_now()-slice_start;
        diagnostic_correction_ms+=ms;diagnostic_correction_max_ms=std::max(diagnostic_correction_max_ms,ms);};
#endif
    if(requested!=Netplay::INVALID_FRAME){
        const auto first=world->CheckpointStart(requested);
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto restore_start=emscripten_get_now();
#endif
        // Packets arriving between slices can invalidate an already replayed
        // prefix. Rewind it again while retaining the original live frontier.
        if(first==Netplay::INVALID_FRAME||
           !(net.Correcting()?net.RestartCorrection(first,checkpoint_span):net.BeginCorrection(first,checkpoint_span))||
           !world->UndoTo(first)||!textures->UndoTo(first)||!audio.DiscardFrom(first)||!files.DiscardFrom(first))
            return Fail("rollback restore failed");
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_restore_ms+=emscripten_get_now()-restore_start;
#endif
        correction_last_visual_suppressed=false;corrected_present_pending=false;
        runtime.correction_present_suppressed=true;correcting=true;++corrections;
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
        diagnostic::Restored(runtime,first);
#endif
    }
    const auto end=net.CorrectionEnd();
    while(net.NextFrame()<end&&world->CanAdvance()){
        const auto before=net.NextFrame();
        runtime.correction_visual_suppressed=skip_resim_visual&&before+1<end;
        runtime.app.renderer.rollback_visual_geometry_suppressed=skip_resim_geometry&&runtime.correction_visual_suppressed;
        const bool ok=RunFrame(true);
        correction_last_visual_suppressed=runtime.correction_visual_suppressed;
        runtime.correction_visual_suppressed=false;
        runtime.app.renderer.rollback_visual_geometry_suppressed=false;
        if(!ok||net.NextFrame()==before){correcting=false;runtime.correction_present_suppressed=false;return Fail("resimulation stalled");}
        ++resimulated;
        // A history tick is indivisible: all four journals and semantic Draw
        // are closed before yielding. Native rAF supplies the timestamp taken
        // before its network Pump; synchronous reconciliation remains atomic.
        if(callback_start_ms>=0&&net.NextFrame()<end&&world->CanAdvance()&&
           emscripten_get_now()-callback_start_ms>=8.){
            callback_yielded=true;++correction_yields;
#ifdef TH_MULTIPLAYER_FIXTURES
            record_cost();
#endif
            return true;
        }
    }
    const bool boundary=net.NextFrame()!=end;
    if(boundary&&correction_last_visual_suppressed&&runtime.app.active()&&
       !runtime.app.draw(1.0f,true,true,false)){
        correcting=false;runtime.correction_present_suppressed=false;return Fail("correction boundary redraw failed");
    }
#ifdef TH_MULTIPLAYER_FIXTURES
    if(boundary&&correction_last_visual_suppressed&&runtime.app.active())++diagnostic_boundary_visual_redraws;
#endif
    if(!net.EndCorrection(boundary)){correcting=false;runtime.correction_present_suppressed=false;return Fail("correction frontier failed");}
    correcting=false;runtime.correction_present_suppressed=false;
    corrected_present_pending=true;
#ifdef TH_MULTIPLAYER_FIXTURES
    record_cost();
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
        const auto route=bootstrap_route<=2?bootstrap_route:route_state(session,net.Setup());
        if(net.Setup().input_delay){
            if(!net.CaptureLeadInBootstrap(RouteBootstrap({},route)))return false;
        }else sample=RouteBootstrap(sample,route);
    }
    return net.CaptureLocal(frame,sample);
}
bool RollbackDriver::RunSpectatorInput(const Netplay::SpectatorFramePacket& packet,bool historical){
    (void)historical;
    auto& app=runtime.app;auto& session=app.session;
    if(packet.playerCount!=session.player_count)return false;
    session.network_frame={};session.network_frame.canAdvance=true;session.network_frame.inputs=packet.inputs;
    session.network_frame_open=true;
    auto gameplay=packet.inputs;
    // Frame zero carries P1's route selection in an out-of-band analog sample.
    // The checkpoint already restored that route, so mirror the player path:
    // retain the raw packet in network_frame but never apply the metadata as
    // one frame of direct-touch movement in the spectator's native world.
    if(packet.frame==0)gameplay[0]=StripRouteBootstrap(gameplay[0]);
    if(app.in_game()&&!app.game.commit_frame_inputs(gameplay.data(),packet.playerCount))return false;
    u16 supervisor_buttons=packet.inputs[0].buttons;
    for(u32 seat=1;seat<packet.playerCount;++seat)supervisor_buttons|=packet.inputs[seat].buttons&InputButton::Menu;
    app.set_spectator_playback_input(supervisor_buttons);
    return app.update()&&!runtime.capture_failed;
}
bool RollbackDriver::RunSpectatorPlayback(double callback_start_ms){
    auto& session=runtime.app.session;auto& app=runtime.app;
    auto& playback=network.StagePlayback();
    if(!playback.Ready()||!app.active()){
        session.network_waiting=true;return true;
    }
    const auto restore=[&](const Netplay::SpectatorStageEvent& event){
#ifdef TH_MULTIPLAYER_FIXTURES
        diagnostic_spectator_restore_failure=0;
        return RestoreSpectatorReplayStart(runtime,event,&diagnostic_spectator_restore_failure);
#else
        return RestoreSpectatorReplayStart(runtime,event);
#endif
    };
    bool rendered_input=false;
    const auto tick=[&](const Netplay::SpectatorFramePacket& packet,bool historical){
        if(!RunSpectatorInput(packet,historical)){session.network_frame_open=false;return false;}
        // The source's SDL loop performs one ordinary draw after every
        // admitted input frame. TH08 draw retains named ANM and trail state,
        // so batching updates without their matching draws changes gameplay
        // state even though the input stream is exact.
        if(!app.draw(1.0f,false,false)){session.network_frame_open=false;return false;}
        // FinishFrame closes the admission context and advances the logical
        // clock only after the semantic Draw. Keep that ordering during replay
        // so draw-time clock reads match the source; clear it before yielding
        // to GameHost, which must not mistake this read-only tick for a player
        // frame that it should draw or commit again.
        session.network_frame_open=false;
        ++runtime.multiplayer_logic_frame;
#ifdef TH_MULTIPLAYER_FIXTURES
        if(const auto* canonical=multiplayer_replay_gameplay_state(&runtime);canonical&&canonical[0]==1){
            std::array<u32,14> sample{};sample[0]=packet.frame+1;
            std::copy(canonical,canonical+13,sample.begin()+1);
            if(diagnostic_spectator_canonical_trace.size()==4096)
                diagnostic_spectator_canonical_trace.erase(diagnostic_spectator_canonical_trace.begin());
            diagnostic_spectator_canonical_trace.push_back(sample);
        }
#endif
        rendered_input=true;return true;
    };
    const auto should_yield=[&](){return callback_start_ms>=0&&emscripten_get_now()-callback_start_ms>=8.;};
    const bool catching_up=playback.FastForwarding();
    const auto step_budget=catching_up?32u:1u;
    if(!playback.Step(restore,tick,should_yield,step_budget)||playback.Failed()){
        // Stage restoration and replay are optional observer features. A bad
        // checkpoint can stop this viewer, but never turns into a player error.
        session.network_waiting=true;return true;
    }
    // A paused viewer still needs presentation, but this draw must not mutate
    // the native world or emit sounds as a side effect of polling the UI.
    if(!rendered_input&&!app.draw(1.0f,false,true)){
        session.network_waiting=true;return true;
    }
    return true;
}
bool RollbackDriver::Step(bool render,double callback_start_ms){
    auto& session=runtime.app.session;auto& net=session.netplay;
    callback_yielded=false;
    if(failed||open||session.network_frame_open||!runtime.prepared||runtime.capture_failed){FlushInputs();return false;}
    session.network_waiting=false;
    // Resource transitions remain confirmed. Once outside the combat graph,
    // do not let waiting for one UI frame stop the next physical input slot.
    net.SetCapturePhase(initialized&&!generation_transition_pending&&
        (!Stable()||!net.AllowsRollback()),u64(emscripten_get_now()*1000.));
    if(!network.PumpForCapture(Stable())){FailNetwork("network channel failed");FlushInputs();return false;}
    // Only the already initialized live frontier may keep Pump's ordinary
    // tail until the once-only capture below. Every startup/readiness/loading
    // return and every correction continuation flushes before doing work.
    bool capture_before_restore=initialized&&Stable()&&bound&&world->CanAdvance()&&net.CanCapture()&&
        (net.Setup().version<6||net.RollbackFrame()==Netplay::INVALID_FRAME);
#ifdef TH_MULTIPLAYER_FIXTURES
    capture_before_restore=capture_before_restore&&diagnostic_early_input;
#endif
    if(!capture_before_restore&&!FlushInputs())return false;
    if(network.SpectatorFinished()){session.network_waiting=true;return true;}
    if(net.PreparingWorld()&&!Stable()&&(!net.CanStart()||!initialized)){
        // Resources and the native graph may load, but no gameplay frame,
        // device capture or Replay stamp is admitted before the commit. New
        // Replay/spectator streams start at that same loaded world boundary.
            if(session.multiplayer_route_state>2){
                bootstrap_route=route_state(session,net.Setup());
                session.multiplayer_route_state=0;
            }
            const bool updated=runtime.app.update();
            if(!updated||runtime.capture_failed)return FailNativeUpdate(Netplay::INVALID_FRAME,updated);
            return runtime.app.draw()||Fail("startup presentation failed");
    }
    if(!net.CanStart()){
        session.network_waiting=true;return true;
    }
    session.multiplayer_session.input_delay=net.Setup().input_delay;
    session.multiplayer_session.measured_prediction=net.Setup().measured_prediction;
    if(net.Playback()&&(runtime.replay_finished||runtime.replay_archive.Complete())){
        runtime.replay_finished=true;session.network_waiting=true;return true;
    }
    if(!initialized){audio.Reset(net.NextFrame());files.Reset(net.NextFrame());
        if(!net.Spectator()&&!runtime.begin_replay_recording())return Fail("Replay recording bootstrap failed");
        if(!net.Spectator()&&!runtime.bind_audio_events(&audio))return Fail("audio sink binding failed");
        initialized=true;generation=net.Generation();}
    if(net.Spectator())return RunSpectatorPlayback(callback_start_ms);
    // TH07's early once-only send rule: network threads can transmit while
    // this endpoint restores/replays. Restrict it to the current live world;
    // loading, retirement and generation bootstrap retain their admission fence.
    u32 sent_frame=Netplay::INVALID_FRAME,sent_generation=net.Generation();
    bool early_input=bound&&world->CanAdvance()&&!net.ReadOnly()&&
        (net.Setup().version<6||net.RollbackFrame()==Netplay::INVALID_FRAME);
#ifdef TH_MULTIPLAYER_FIXTURES
    early_input=early_input&&diagnostic_early_input;
#endif
    if(early_input&&net.CanCapture()){
        if(!CaptureLocalInput()){Fail("local capture failed");FlushInputs();return false;}
        sent_frame=net.NextCaptureFrame()-1;
        if(!network.Captured(sent_frame)){FailNetwork("captured input send failed");FlushInputs();return false;}
    }
    // Finish the old obligation even if capture was unnecessary or rejected.
    // Restore, replay and every callback budget exit start with no deferred
    // ordinary send; historical ticks only reuse retained physical input.
    if(!FlushInputs())return false;
    if(net.AllowsRollback()&&!Correct(callback_start_ms))return false;
    if(net.Correcting())return true;
    // Complete recovery may proceed through normal admission to the owed
    // forward tick. Its indivisible cost can extend this callback beyond the
    // history slice budget; the SDL host still gates further catch-up ticks.
    if(!Commit()||!Admit())return false;
    if(session.network_waiting)return PresentCorrection();
    // The outer SDL callback owns the shared observer catch-up budget.
    const auto frame=net.NextFrame();
    if(!net.ReadOnly()&&(sent_frame==Netplay::INVALID_FRAME||net.Generation()!=sent_generation)&&net.CanCapture()){
        if(!CaptureLocalInput())return Fail("local capture failed");
        sent_frame=net.NextCaptureFrame()-1;
        if(!network.Captured(sent_frame))return FailNetwork("captured input send failed");
    }
    if(!net.HasLocalFrame(frame)){
        if(net.Playback()){
            const auto* inputs=runtime.replay_archive.PlaybackFrame(frame);
            if(!inputs||!net.FeedPlayback(frame,*inputs))return Fail("Replay input rejected");
        }else{session.network_waiting=true;return PresentCorrection();}
    }
    if(!RunFrame(render))return false;
    return !session.network_waiting||PresentCorrection();
}
void RollbackDriver::Shutdown(){
    spectator_stage_tags.fill({});pending_spectator_frame=Netplay::INVALID_FRAME;
    pending_spectator_initializer.clear();pending_spectator_checkpoint=false;pending_spectator_boss_slots=0;
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    diagnostic::Disable(runtime);
#endif
    if(!initialized&&!bound&&!network.Enabled())return;
    runtime.correction_present_suppressed=false;
    runtime.correction_visual_suppressed=false;
    runtime.app.renderer.rollback_visual_geometry_suppressed=false;
    correcting=callback_yielded=correction_last_visual_suppressed=corrected_present_pending=false;
    // Shutdown may arrive while the displayed state is still predicted. Never
    // serialize it as a completed personal record.
    runtime.discard_network_shutdown_writes=true;
    world.reset();textures.reset();bound=false;
    runtime.bind_audio_events(nullptr);audio.Reset();files.Reset();network.Close();
    runtime.app.session.network_frame_open=false;open=initialized=false;
}
}
