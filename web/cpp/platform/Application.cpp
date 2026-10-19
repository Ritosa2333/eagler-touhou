#include "../game/CallbackNames.hpp"
#include "../game/HighRefresh.hpp"
#include "../game/PresentationAudit.hpp"
#include "Application.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../sdl/ThpracUi.hpp"
#include "Renderer.hpp"
#endif
#include <new>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/RecoveryBudget.hpp"
#include "../multiplayer/LiveMotionObservation.hpp"
#include "../multiplayer/CallbackWorkTiming.hpp"
#include "../multiplayer/SpectatorStageState.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#endif
#endif
namespace th10::browser {
namespace {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
using TailPresentation=host::NetworkTailPresentation;
namespace work_timing=multiplayer::callback_work_timing;
using AuthoredIdentity=Application::AuthoredPresentationBoundary::Identity;
AuthoredIdentity authored_identity(const Application& app){
    const auto* world=app.world;
    const auto* session=world?world->actors.session:nullptr;
    const auto* current=world?world->backgrounds.current:nullptr;
    const auto* previous=world?world->backgrounds.previous:nullptr;
    const auto address=[](const void* pointer){return reinterpret_cast<std::uintptr_t>(pointer);};
    return {{address(world),address(session),address(current),address(previous),
             address(current?current->file:nullptr),address(previous?previous->file:nullptr),
             address(current?current->animation_file:nullptr),address(previous?previous->animation_file:nullptr),
             address(app.state.current_stage)},
            app.state.netplay_runtime.Generation(),session?session->stage_identifier:0u,app.state.game.stage};
}
bool same_authored_identity(const AuthoredIdentity& before,const AuthoredIdentity& after){
    for(unsigned i=0;i<9;++i)if(before.owners[i]!=after.owners[i])return false;
    return before.generation==after.generation&&before.stage_identifier==after.stage_identifier&&
           before.stage_number==after.stage_number;
}
u32 authored_boundary_flags(const Application& app){
    u32 flags=0;
    const auto& runtime=app.state.netplay_runtime;
    const auto* world=app.world;
    if(app.stopped||!app.initialized||app.error||!world||
       (world&&(world->error||world->backgrounds.error)))flags|=TailPresentation::UnavailableApplication;
    if(!app.multiplayer_active()||!runtime.NetworkEnabled()||!runtime.CanStart()||
       !runtime.InitialInputsReady()||runtime.Mode()!=Netplay::AdonisMode::Rollback||
       runtime.Playback()||runtime.Spectator()||runtime.Retired()||app.multiplayer_replay_scope)
        flags|=TailPresentation::NotLiveForward;
    if(app.value.screen!=7||app.state.pending_screen!=7||app.value.frame_gate||
       app.value.loading_state||app.value.resource_loader.running||app.loading_pause||
       app.multiplayer_generation_pending||app.state.quitting||
       (world&&(world->loading||world->resource_drawing>=0||world->resource_updating>=0)))
        flags|=TailPresentation::LoadingOrTransition;
    if(world&&world->actors.session){
        const auto& session=*world->actors.session;
        if(session.session_flags&0x74u)flags|=TailPresentation::FrozenOrPaused;
        if((session.session_flags&0x808u)||session.elapsed.current<=0)
            flags|=TailPresentation::LoadingOrTransition;
        if(world->actors.results&&world->actors.results->state!=0)flags|=TailPresentation::FrozenOrPaused;
    }
    if(!(app.engine.speed>0.f))flags|=TailPresentation::FrozenOrPaused;
    if(!world||TailPresentation::StageHasAuthoredBoundary(world->backgrounds.current))
        flags|=TailPresentation::CurrentStageBoundary;
    if(world&&world->backgrounds.previous&&TailPresentation::StageHasAuthoredBoundary(world->backgrounds.previous))
        flags|=TailPresentation::PreviousStageBoundary;
    const auto& capture=*reinterpret_cast<const AnmCapture*>(app.engine.manager.header);
    if(capture.reserved_000>=0||capture.target_file>=0)flags|=TailPresentation::CaptureRequested;
    if((app.input.player_profiles[0].input.raw_pressed&0x800u)||app.writer_pending)
        flags|=TailPresentation::ScreenshotRequested;
    if(app.reset_frames)flags|=TailPresentation::DeviceReset;
    return flags;
}
double multiplayer_work_now(){
#ifdef __EMSCRIPTEN__
    return emscripten_get_now();
#else
    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
}
#endif
#ifdef TH_SDL3
extern "C" double monotonic();
#else
extern "C" __attribute__((import_module("th10_time"),import_name("monotonic"))) double monotonic();
#endif
template<class T>void dispose(T*& value){if(value){value->~T();std::free(value);value=nullptr;}}
template<class T,class... A>T* create(A&... args){auto* p=std::malloc(sizeof(T));return p?new(p)T(args...):nullptr;}
}
Application::Application(FileSystem& f,Input& i,GameState& s,AnimationEngine& e,Fonts& ft,Audio& a,ScreenEffects& fx):files(f),input(i),state(s),engine(e),fonts(ft),audio(a),effects(fx),value(s.application),captures(e),manager(&e.manager),chain(&e.chain_value),screens(*this),frames(*this),loop(*this),rates(*this),presentation(*this),screenshots(*this),config(*this){
    value.device=reinterpret_cast<void*>(static_cast<uintptr_t>(e.device.handle));value.world_camera=e.world;value.ui_camera=e.ui;value.active_camera=&value.ui_camera;
    engine.register_receiver(*this);effects.quitting=&state.quitting;
}
Application::~Application(){shutdown();for(auto*& entry:entries)if(entry){chain->remove_locked(entry,engine.callback_environment);entry=nullptr;}engine.unregister_receiver(*this);}
bool Application::initialize(){
    if(initialized)return !error;initialized=true;
    if(ApplicationConfiguration{config}.load("th10.cfg",state.configuration,disable_vsync)<0)notice_code=static_cast<i32>(ConfigurationNotice::SaveFailed);
    value.display_flags=state.configuration.display_flags;engine.resources.display=value.display_flags;fonts.display=value.display_flags;
    input.thresholds[0]=state.configuration.axis_x;input.thresholds[1]=state.configuration.axis_y;
    clock_origin=monotonic();statistics=FrameStatistics::create(rates);if(!statistics){error=-1;return false;}
    value.screen=-2;value.pending_screen=0;value.reserved_398=0;
    constexpr CallbackToken callbacks[]={callback_id::ApplicationUpdate,callback_id::ApplicationBeginDraw,callback_id::ApplicationDrawBarrier,callback_id::ApplicationFinishDraw};constexpr i32 priorities[]={1,1,40,50};
    for(u32 i=0;i<4;++i)entries[i]=chain->add(callbacks[i],this,priorities[i],i!=0,true,engine.callback_environment);
    Presentation{presentation}.configure_defaults();
#ifdef TH_NATIVE_PLATFORM
    if(!engine.resources.preload_transition(0)||!engine.resources.preload_transition(1)){error=-3;return false;}
    for(u32 resource=2;resource<6;++resource)engine.resources.preload_transition(resource);
#endif
    return true;
}
Extended Application::time(){return Extended::from_double(monotonic()-clock_origin);}
void Application::sync_views(){
    startup_view=startup?startup->value:nullptr;title_view=title?title->value:nullptr;session_view=world?world->actors.session:nullptr;ending_view=credits?credits->value:nullptr;common_view=startup?startup->common_value:nullptr;
    value.startup=startup_view;value.loading_animations=startup?startup->loading_file:nullptr;
    effects.controller_flags=session_view?&session_view->session_flags:nullptr;
}
bool Application::ensure_world(){
    if(world)return true;if(!startup||!startup->shared||!startup->scores){error=-2;return false;}
    world=create<World>(state,engine,*startup->shared,fonts,input,audio,*startup->scores,effects);if(!world)error=-1;else world->always_hitbox=always_hitbox;return world!=nullptr;
}
void Application::advance_loading(){
    if(startup)startup->advance_loading();sync_views();
    if(title)title->advance_loading();if(world)world->advance_loading();if(credits)credits->advance_loading();sync_views();
    if((startup&&startup->error)||(title&&title->error)||(world&&world->error)||(credits&&credits->error))error=-3;
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void Application::multiplayer_reset_authored_boundary(){
    auto& boundary=multiplayer_authored_boundary;
    boundary.flags=TailPresentation::Unobserved;
    boundary.frame=Netplay::INVALID_FRAME;
    boundary.tracking=state.netplay_runtime.NetworkTailPresentationEnabled();
    if(!boundary.tracking)return;
    // Reject a transition performed by Update even if the new world looks
    // stable by the time Draw begins. No saved owner is dereferenced later.
    boundary.identity=authored_identity(*this);
    boundary.flags|=authored_boundary_flags(*this);
}
void Application::multiplayer_begin_authored_boundary(){
    auto& boundary=multiplayer_authored_boundary;
    if(!boundary.tracking)return;
    if(!(boundary.flags&TailPresentation::NoAuthoredDraw))boundary.flags|=TailPresentation::RepeatedAuthoredDraw;
    boundary.flags&=~u32(TailPresentation::NoAuthoredDraw);
    boundary.flags|=authored_boundary_flags(*this);
    if(!same_authored_identity(boundary.identity,authored_identity(*this)))
        boundary.flags|=TailPresentation::SceneIdentityChanged;
    if(!multiplayer_frame_open||multiplayer_pending_frame==Netplay::INVALID_FRAME||
       multiplayer_waiting||state.netplay_runtime.Correcting()||!world||world->rollback_resimulating||
       high_refresh::render_only||engine.suppress_rollback_sprite_output||
       engine.netplay_frame!=multiplayer_pending_frame)
        boundary.flags|=TailPresentation::NotLiveForward;
    boundary.frame=multiplayer_pending_frame;
}
void Application::multiplayer_observe_authored_boundary(bool submitted){
    auto& boundary=multiplayer_authored_boundary;
    if(!boundary.tracking||(boundary.flags&TailPresentation::NoAuthoredDraw))return;
    boundary.flags|=authored_boundary_flags(*this);
    if(!same_authored_identity(boundary.identity,authored_identity(*this)))
        boundary.flags|=TailPresentation::SceneIdentityChanged;
    if(submitted&&boundary.frame!=Netplay::INVALID_FRAME&&multiplayer_forward_advanced&&
       !multiplayer_frame_open&&!state.netplay_runtime.Correcting()&&world&&!world->rollback_resimulating&&
       !engine.rollback_state&&state.netplay_runtime.LastSimulatedFrame()==boundary.frame)
        boundary.flags&=~u32(TailPresentation::IncompleteAuthoredFrame);
}
bool Application::multiplayer_active()const{
    const auto& session=state.multiplayer_session;
    return session.sessionId&&session.started&&world&&!world->loading&&world->actors.session&&
           state.netplay_runtime.Configured();
}

multiplayer::RemotePlayerPresentation::Progress Application::remote_player_presentation_progress()const{
    multiplayer::RemotePlayerPresentation::Progress progress;
    const auto& runtime=state.netplay_runtime;
    progress.correcting=runtime.Correcting();
    progress.frame_open=multiplayer_frame_open;
    progress.resimulating=world&&world->rollback_resimulating;
    progress.usable=!stopped&&!error&&multiplayer_active()&&!multiplayer_replay_scope&&
        !runtime.Playback()&&!runtime.Spectator()&&!runtime.Retired()&&
        state.pending_screen==value.screen&&!world->error&&
        !(world->actors.session->session_flags&0x74u);
    return progress;
}
void Application::freeze_remote_player_presentation(){
    if(!world)return;
    world->remote_player_presentation.Freeze(world->remote_player_presentation_identity(),
        remote_player_presentation_progress(),state.netplay_runtime.LastSimulatedFrame(),
        multiplayer_rollbacks,[&](multiplayer::RemotePlayerPresentation::Snapshots& output){
            world->read_remote_player_presentation(output);
        });
}
void Application::publish_remote_player_presentation(){
    if(!world)return;
    const auto progress=remote_player_presentation_progress();
    if(progress.Complete())world->remote_player_presentation_revision=multiplayer_rollbacks;
    world->remote_player_presentation.Publish(world->remote_player_presentation_identity(),
        progress,state.netplay_runtime.LastSimulatedFrame(),multiplayer_rollbacks,
        multiplayer_forward_advanced,[&](multiplayer::RemotePlayerPresentation::Snapshots& output){
            world->read_remote_player_presentation(output);
        });
}
void Application::invalidate_remote_player_presentation(){
    multiplayer_remote_endpoint_presented=true;
    if(world){world->remote_player_presentation.Invalidate();world->remote_player_presentation_revision=0;}
}

bool Application::multiplayer_pump_network(bool capture_follows){
    // rAF keeps pumping even while the shell is suspended after a fatal
    // error. Preserve the first failure instead of replacing it with a later
    // confirmed-input timeout from the now-stopped simulation.
    if(error)return false;
    if(state.netplay_runtime.PumpNetwork(multiplayer_active(),
        capture_follows&&state.pending_screen==value.screen))return true;
    const auto& runtime=state.netplay_runtime;
    const auto& channel=runtime.Channel();
    const auto frame=[](u32 value)->i32{
        return value==Netplay::INVALID_FRAME?-1:i32(value);
    };
    // Preserve the transport fault with the exact per-seat frontiers at the
    // failing pump. The peer may itself have stopped first; a generic -5
    // cannot distinguish that from lost delivery after the fact.
    std::snprintf(multiplayer_failure_detail,sizeof(multiplayer_failure_detail),
        "%s (seat %u, next %u, confirmed [%d,%d,%d], peer next [%d,%d,%d], "
        "capture %d, sent %u, recv %u, repair %u, queued %zu, screen %d->%d)",
        runtime.NetworkError(),runtime.Config().localPlayer,runtime.NextFrame(),
        frame(runtime.ConfirmedThrough(0)),frame(runtime.ConfirmedThrough(1)),
        frame(runtime.ConfirmedThrough(2)),
        frame(channel.PeerFrame(0)),frame(channel.PeerFrame(1)),frame(channel.PeerFrame(2)),
        frame(channel.LatestCapture()),channel.PacketsSent(),channel.PacketsReceived(),
        channel.RepairsSent(),runtime.Transport().BufferedAmount(),value.screen,state.pending_screen);
    error=-5;invalidate_remote_player_presentation();return false;
}

void Application::multiplayer_fail(const char* reason){
    if(error)return;
    invalidate_remote_player_presentation();
    const auto& runtime=state.netplay_runtime;
    std::snprintf(multiplayer_failure_detail,sizeof(multiplayer_failure_detail),
        "%s (frame %u, confirmed %u, rollback %u)",reason,runtime.NextFrame(),
        runtime.ConfirmedThroughAllRemotes(),runtime.RollbackFrame());
    error=-4;
}

u32 Application::multiplayer_spectator_catchup_budget()const{
    if(!multiplayer_active()||!state.netplay_runtime.Spectator()||!world||world->loading)return 1;
    const auto& playback=state.netplay_runtime.StagePlaybackController();
    if(!state.netplay_runtime.StagePlaybackReady())return 1;
    return playback.FastForwarding()?4:1;
}

i32 Application::multiplayer_update(){
    auto& runtime=state.netplay_runtime;
    if(runtime.PreparingWorld()){
        const auto started=state.multiplayer_session.started;state.multiplayer_session=runtime.Setup();state.multiplayer_session.started=started;
        if(!state.multiplayer_replay.Recording()&&!state.multiplayer_replay.Playing()){
            if(!state.multiplayer_replay.Begin(state.multiplayer_session,state.configuration)){error=-6;return -1;}
            // The measured contract is committed after resource loading. Its
            // first Replay checkpoint must still precede frame-zero activation.
            world->begin_replay_checkpoint();
        }
    }
    auto& rollback=world->rollback;
    if(multiplayer_frame_open){multiplayer_fail("Previous frame still open");return -1;}
    const auto capture_local=[&](u32 frame){
        if(runtime.HasLocalCapture(frame))return true;
        Netplay::FrameInput local{};
        const auto buttons=static_cast<u16>(InputDevices{input}.sample());
        if(world->local_player>=world->player_count||!world->pilots[world->local_player].player||
           !multiplayer::InputLanes::BuildLocalFrame(state.multiplayer_local_analog,buttons,local)){
            multiplayer_fail("Capture local controls");return false;
        }
        if(!runtime.CaptureLocal(frame,local)){multiplayer_fail("Store local input");return false;}
        return true;
    };
    // Send the once-latched local input BEFORE expensive reconciliation so
    // the peer need not predict our CPU work as additional network latency.
    // Replay, spectator and transition paths never sample physical controls.
    if(runtime.CanCapture()&&state.pending_screen==value.screen&&
       !capture_local(runtime.NextFrame()))return -1;
    if(!runtime.FlushNetworkInputs()){multiplayer_fail("Flush captured input tail");return -1;}
    if(!runtime.InitialInputsReady()){multiplayer_waiting=true;return 1;}

    if(runtime.HasRollbackRequest()){
        if(!runtime.AllowsRollback()||!rollback){multiplayer_fail("Undo requested without rollback storage");return -1;}
        const auto rollbackFrame=runtime.RollbackFrame();
        const auto last=runtime.LastSimulatedFrame();
        if(last!=Netplay::INVALID_FRAME&&rollbackFrame<=last){
            const auto end=runtime.PredictionFrontier();
            std::uint32_t replayFrom=rollbackFrame;
            // Freeze the complete endpoint before the first restore. A nested
            // request during chunked correction must not publish old history.
            if(!runtime.Correcting()){
                publish_touch_input_frontier();freeze_remote_player_presentation();
            }
            if(!rollback->RestoreTo(rollbackFrame,&replayFrom)){multiplayer_fail("Restore checkpoint");return -1;}
            if(!runtime.RewindSimulationTo(replayFrom)){multiplayer_fail("Rewind input history");return -1;}
            if(!world->audio_events.DiscardFrom(replayFrom)){multiplayer_fail("Rewind audio history");return -1;}
            runtime.BeginCorrection(end);
            multiplayer_corrected_presentation_pending=false;
            ++multiplayer_rollbacks;
        }else runtime.ClearRollbackRequest();
    }

    if(runtime.Correcting()){
        // Live rAF may return only between complete historical transactions.
        // The synchronous stopped-loop oracle deliberately keeps its original
        // one-completed-tick contract and uses this same recovery body.
        unsigned replayed=0;
        while(runtime.NextFrame()<runtime.CorrectionEnd()){
            // A corrected lifecycle boundary retires the abandoned future.
            // Captured input remains owned by the core; graph destruction is
            // still performed by the reconciled/confirmed fence below.
            if(state.pending_screen!=value.screen)break;
            if(multiplayer_callback_started>=0&&replayed&&
               !multiplayer::RecoveryBudget::CanStartTick(replayed,
                   runtime.CorrectionEnd()-runtime.NextFrame(),
                   static_cast<std::uint64_t>(std::max(0.,multiplayer_work_now()-
                       multiplayer_callback_started)*1'000'000.))){
                ++multiplayer_recovery_yields;
                multiplayer_waiting=true;return 1;
            }
            const auto frame=runtime.NextFrame();
            auto spectatorStage=world->spectator_stage_tag();
            std::vector<u8> spectatorInitializer;
            if(frame==0&&runtime.SpectatorCheckpointRequired(frame,spectatorStage)){
                multiplayer::SpectatorStageState checkpoint;
                if(world->capture_spectator_checkpoint(frame,checkpoint))
                    multiplayer::EncodeSpectatorStageState(checkpoint,spectatorInitializer);
            }
            const auto decision=runtime.Prepare(frame);
            const bool replayActive=state.multiplayer_replay.Recording()||
                                    state.multiplayer_replay.Playing();
            world->rollback_resimulating=true;
            if(!decision.canAdvance||(replayActive&&
               !state.multiplayer_replay.Stamp(frame,u32(state.game.stage)))||
               !world->begin_netplay_frame(frame)){
                world->rollback_resimulating=false;multiplayer_fail("Begin resimulated frame");return -1;
            }
            world->capture_replay_checkpoint_precommit(state.multiplayer_replay.Base()+frame);
            if(!multiplayer::InputLanes::Commit(state.input_lanes,decision.inputs.data(),
                                                state.multiplayer_session.playerCount)){
                world->clear_replay_checkpoint_precommit();
                world->rollback_resimulating=false;multiplayer_fail("Commit resimulated input");return -1;
            }
            const i32 result=[&]{
                work_timing::PhaseScope work_phase(work_timing::state,work_timing::Phase::HistoryUpdate);
                return engine.update_all();
            }();
            world->clear_replay_checkpoint_precommit();
            if(result==0||result==-1||!multiplayer_resimulate_draw()||
               !world->end_netplay_frame()||!runtime.MarkSimulated(frame,decision)){
                world->rollback_resimulating=false;multiplayer_fail("Finish resimulated frame");return -1;
            }
            if(world->spectator_activation_checkpoint_frame==frame){
                spectatorStage=world->spectator_activation_checkpoint_stage;
                spectatorInitializer=world->spectator_activation_checkpoint_wire;
            }
            runtime.RecordSpectatorFrame(frame,spectatorStage,spectatorInitializer,decision.inputs,
                                         world->spectator_boss_registration_mask);
            world->rollback_resimulating=false;
            ++multiplayer_resimulated_frames;++replayed;
        }
        runtime.EndCorrection();
        multiplayer_corrected_presentation_pending=true;
        // A fully recovered callback may attempt its one already-due forward
        // tick even after history used the budget. Do not permanently spend
        // every callback repairing old work without producing a new endpoint.
    }

    // Publish title metadata only after the pending rollback has been applied
    // to the native world for this callback.
    runtime.PublishConfirmedSpectatorHistoryAfterReconcile();

    {
        work_timing::PhaseScope work_phase(work_timing::state,work_timing::Phase::ConfirmedCommit);
        if(!world->commit_audio()){multiplayer_fail("Commit audio before tick");return -1;}
        if(!world->commit_replay()){multiplayer_fail("Commit Replay before tick");return -1;}
    }
    // Reconcile first: late input can cancel a predicted transition. Clearing
    // the journal as soon as confirmation arrives loses that required restore.
    // Whole resource lifetimes only cross a fully reconciled, confirmed fence.
    if(state.pending_screen!=value.screen){
        if(runtime.Spectator()&&runtime.StagePlaybackReady()){
            // Let the native startup and title screens finish their resource
            // setup. The spectator never enters an ordinary game from those
            // screens: the only gameplay transition is the checkpoint-backed
            // title -> game transition below.
            const bool bootScreen=value.screen==-2||value.screen==0||
                value.screen==1||value.screen==2;
            const bool bootTransition=bootScreen&&
                (state.pending_screen==0||state.pending_screen==1||
                 state.pending_screen==2||state.pending_screen==4);
            const bool checkpointTransition=value.screen==4&&state.pending_screen==7&&
                world&&!world->actors.session&&state.multiplayer_session.configured&&
                state.multiplayer_session.started&&runtime.StagePlaybackReady();
            if(bootTransition||checkpointTransition){
                frames.transition(value);
                if(error){multiplayer_fail("Start spectator stage");return -1;}
                return 1;
            }
            multiplayer_waiting=true;return 1;
        }
        // A screen boundary stops normal gameplay capture. Explicitly flush
        // the current input/ACK tail over the reliable lane before waiting,
        // otherwise one lost final RTC fast packet can strand both endpoints
        // at this fence and eventually surface as network error -5.
        if(!runtime.PrepareTransitionFence()){
            multiplayer_waiting=true;return 1;
        }
        if(world->scores.multiplayer_active()&&!world->scores.replay_read_only()&&
           !world->scores.checkpoint_multiplayer(world->calendar.timestamp())){error=-6;return -1;}
        engine.rollback_state=nullptr;engine.netplay_frame=Netplay::INVALID_FRAME;
        if(rollback)rollback->Clear();
        if(!world->backgrounds.collect_retired(Netplay::INVALID_FRAME)){multiplayer_fail("Release retired backgrounds");return -1;}
        if(state.pending_screen==10||state.pending_screen==13){
            const bool spectator=runtime.Spectator();
            if(!runtime.RetireRun()){multiplayer_fail("Retire session");return -1;}
            if(spectator){
                // Spectator admission belongs to one relay run. A Retry/new
                // generation needs a fresh lobby start and spectator grant.
                state.pending_screen=4;
            }else multiplayer_generation_pending=true;
        }
        const i32 result=engine.update_all();
        if(world&&!world->loading&&rollback&&runtime.AllowsRollback()&&!rollback->Reset()){multiplayer_fail("Reset rollback history");return -1;}
        if(result&&result!=-1)presentation_audit::simulation_tick();
        return result;
    }

    if(runtime.Spectator()&&runtime.StagePlaybackReady()){
        // While native startup/title screens are still alive there is no
        // checkpoint-restored world to advance. Never consume relay frames
        // against that screen (or accidentally start a default Stage1 run).
        if(value.screen!=7||state.pending_screen!=7||!world||!world->actors.session){
            multiplayer_waiting=true;return 1;
        }
        auto& playback=runtime.StagePlaybackController();
        const auto restore=[&](const Netplay::SpectatorStageEvent& event){
            if(event.kind!=Netplay::SpectatorEventKind::Checkpoint||
               event.schema!=multiplayer::kSpectatorStageSchema||!world)return false;
            multiplayer::SpectatorStageState checkpoint;
            if(!multiplayer::DecodeSpectatorStageState(event.state,checkpoint)||
               checkpoint.checkpoint.firstFrame!=event.frame||
               (checkpoint.checkpoint.label&255u)!=event.stage||
               !world->prepare_spectator_checkpoint(checkpoint)){
                multiplayer_fail("Restore spectator stage checkpoint");return false;
            }
            return true;
        };
        const auto tick=[&](const Netplay::SpectatorFramePacket& packet,bool seeking){
            // The shared controller restores a new checkpoint before its first
            // input, including during uninterrupted live following.
            if(!world||world->loading||state.pending_screen!=value.screen)return false;
            world->suppress_spectator_audio(seeking);
            if(!world->begin_netplay_frame(packet.frame)){
                world->suppress_spectator_audio(false);return false;
            }
            if(!multiplayer::InputLanes::Commit(state.input_lanes,packet.inputs.data(),
                                                state.multiplayer_session.playerCount)){
                world->end_netplay_frame();return false;
            }
            engine.snapshot_presentation();
            const i32 result=engine.update_all();
            // STD fades, Player body positions and authored ANM caches are
            // written by Draw. Execute it once for every historical input,
            // including paused seeks and multi-input catch-up batches.
            const bool drawn=result!=0&&result!=-1&&multiplayer_resimulate_draw();
            const bool ended=world->end_netplay_frame();
            if(!drawn||!ended||world->error||world->backgrounds.error)return false;
            presentation_audit::simulation_tick();
            if(!world->commit_audio())return false;
            multiplayer_waiting=false;
            return true;
        };
        const double catchupStarted=multiplayer_work_now();
        const bool stepped=playback.Step(restore,tick,[&]{
            return multiplayer_work_now()-catchupStarted>=4.0;
        },multiplayer_spectator_catchup_budget());
        if(error)return -1;
        if(world->error||world->backgrounds.error){
            multiplayer_fail("Advance restored spectator stage");return -1;
        }
        // The packet callback already ran each authored Draw. The outer
        // clock must only display that completed state through render-only
        // presentation, including when no input was consumed while paused.
        multiplayer_waiting=true;
        (void)stepped;
        return 1;
    }

    const auto frame=runtime.NextFrame();
    multiplayer_spectator_pending_frame=frame;
    multiplayer_spectator_pending_stage=world->spectator_stage_tag();
    multiplayer_spectator_pending_initializer.clear();
    if(frame==0&&runtime.SpectatorCheckpointRequired(frame,multiplayer_spectator_pending_stage)){
        multiplayer::SpectatorStageState checkpoint;
        if(world->capture_spectator_checkpoint(frame,checkpoint))
            multiplayer::EncodeSpectatorStageState(checkpoint,multiplayer_spectator_pending_initializer);
    }
    if(runtime.Spectator()){
        if(!runtime.FeedSpectator(frame)){multiplayer_waiting=true;return 1;}
    }else if(runtime.Playback()){
        if(state.multiplayer_replay.Complete()){multiplayer_waiting=true;return 1;}
        const auto* inputs=state.multiplayer_replay.PlaybackFrame(frame,u32(state.game.stage));
        if(!inputs||!runtime.FeedPlayback(frame,inputs->data(),state.multiplayer_session.playerCount)){error=-6;return -1;}
    }else if(!runtime.HasLocalCapture(frame)){
        if(!capture_local(frame))return -1;
    }
    const auto decision=runtime.Prepare(frame);
    if(!decision.canAdvance){multiplayer_waiting=true;return 1;}

    engine.snapshot_presentation();
    const bool replayActive=state.multiplayer_replay.Recording()||
                            state.multiplayer_replay.Playing();
    if((replayActive&&!state.multiplayer_replay.Stamp(frame,u32(state.game.stage)))||
       !world->begin_netplay_frame(frame)){
        multiplayer_fail("Begin forward frame");return -1;
    }
    world->capture_replay_checkpoint_precommit(state.multiplayer_replay.Base()+frame);
    if(!multiplayer::InputLanes::Commit(state.input_lanes,decision.inputs.data(),
                                        state.multiplayer_session.playerCount)){
        world->clear_replay_checkpoint_precommit();
        multiplayer_fail("Commit forward input");return -1;
    }
    const i32 result=[&]{
        work_timing::PhaseScope work_phase(work_timing::state,work_timing::Phase::ForwardUpdate);
        return engine.update_all();
    }();
    world->clear_replay_checkpoint_precommit();
    if(result==0||result==-1){world->end_netplay_frame();multiplayer_fail("Update forward frame");return -1;}
    multiplayer_pending_decision=decision;
    multiplayer_pending_frame=frame;
    multiplayer_frame_open=true;
    if(result&&result!=-1)presentation_audit::simulation_tick();
    return result;
}

bool Application::multiplayer_resimulate_draw(){
    if(!world||(!world->rollback_resimulating&&!state.netplay_runtime.StagePlaybackReady()))return false;
    if(engine.device.begin_scene()<0)return false;
    auto& animations=*manager;
    animations.batch_quads=0;
    animations.vertex_write=animations.batch_start=animations.vertex_buffer;
    graphics_state=255;
    ApplicationLoop::disable_fog(value,engine.fog_enabled,loop);
    const auto& request=*reinterpret_cast<const AnmCapture*>(engine.manager.header);
    // Captures are serviced by Present, not historical Draw. Keep full
    // output nevertheless whenever an authored pixel capture is pending.
    engine.suppress_rollback_sprite_output=request.reserved_000<0&&request.target_file<0;
    {
        work_timing::PhaseScope work_phase(work_timing::state,work_timing::Phase::HistorySemanticDraw);
        engine.draw_all();
    }
    engine.flush();
    engine.suppress_rollback_sprite_output=false;
    engine.device.texture(nullptr);
    engine.device.end_scene();
    return true;
}

bool Application::multiplayer_finalize_frame(){
    if(!multiplayer_frame_open)return true;
    const auto frame=multiplayer_pending_frame;
    const auto decision=multiplayer_pending_decision;
    multiplayer_frame_open=false;
    multiplayer_pending_frame=Netplay::INVALID_FRAME;
    if(!world){multiplayer_fail("Finalize rollback frame: world missing");return false;}
    const bool audio_open=world->audio_events.IsOpen();
    const bool journal_open=engine.rollback_state!=nullptr;
    if(!world->end_netplay_frame()){
        const auto* rollback=world->rollback.get();
        const auto captured_bytes=rollback?rollback->CapturedBytes(frame):0;
        const auto captured_blocks=rollback?rollback->CapturedBlocks(frame):0;
        char reason[192]{};
        std::snprintf(reason,sizeof(reason),
            "Finalize rollback frame: audio open %u failed %u (%u:%s, %u cmds/%zu B), journal open %u failed %u, bytes %zu blocks %zu",
            u32(audio_open),u32(world->audio_events.Failed()),world->audio_events.FailureCode(),
            world->audio_events.FailureReason(),world->audio_events.CapturedCommands(frame),
            world->audio_events.CapturedBytes(frame),u32(journal_open),
            u32(rollback&&rollback->Failed()),captured_bytes,captured_blocks);
        multiplayer_fail(reason);return false;
    }
    if(!state.netplay_runtime.MarkSimulated(frame,decision)){
        multiplayer_fail("Finalize rollback frame: input history rejected frame");return false;
    }
    if(multiplayer_spectator_pending_frame!=frame){
        multiplayer_fail("Finalize spectator record: input frame mismatch");return false;
    }
    if(world->spectator_activation_checkpoint_frame==frame){
        multiplayer_spectator_pending_stage=world->spectator_activation_checkpoint_stage;
        multiplayer_spectator_pending_initializer=world->spectator_activation_checkpoint_wire;
    }
    state.netplay_runtime.RecordSpectatorFrame(frame,multiplayer_spectator_pending_stage,
        multiplayer_spectator_pending_initializer,decision.inputs,world->spectator_boss_registration_mask);
    multiplayer_spectator_pending_frame=Netplay::INVALID_FRAME;
    multiplayer_spectator_pending_initializer.clear();
    multiplayer_forward_advanced=true;
    multiplayer_corrected_presentation_pending=false;
    if(world->error||world->backgrounds.error){multiplayer_fail("World or background state");return false;}
    if(state.netplay_runtime.Playback()&&!state.multiplayer_replay.Played(frame)){error=-6;return false;}
    work_timing::PhaseScope work_phase(work_timing::state,work_timing::Phase::ConfirmedCommit);
    if(!world->commit_audio()){multiplayer_fail("Commit audio after tick");return false;}
    if(!world->commit_replay()){multiplayer_fail("Commit Replay after tick");return false;}
    const auto confirmed=state.netplay_runtime.ConfirmedThroughAllRemotes();
    if(confirmed!=Netplay::INVALID_FRAME){
        const auto frontier=std::min(confirmed,frame)+1;
        // Drop every checkpoint which could contain the retired graph before
        // physical reclamation. The retirement queue itself is not journaled.
        if(world->rollback)world->rollback->DiscardBefore(frontier);
        if(!world->backgrounds.collect_retired(frontier)){multiplayer_fail("Collect confirmed backgrounds");return false;}
    }
    return true;
}
#endif
i32 Application::step(bool scheduled_tick,double callback_started){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Even stopped, failed, skipped and input-blocked steps revoke the last
    // step's eligibility. Historical Draw never calls the begin hook.
    multiplayer_reset_authored_boundary();
#endif
    if(stopped){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        invalidate_remote_player_presentation();
#endif
        return error?2:1;
    }
    if(!initialized&&!initialize())return 2;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    multiplayer_remote_endpoint_presented=true;
    multiplayer_callback_started=callback_started;
    multiplayer_forward_advanced=false;
    if(multiplayer_replay_scope){
        const bool escape=input.snapshot.virtual_keys[27]!=0;
        if(escape&&!multiplayer_replay_escape&&world&&world->actors.session){
            // Viewer controls are local lifecycle operations, never extra
            // player inputs inserted into the recorded authoritative stream.
            state.multiplayer_session.started=false;state.pending_screen=4;
            engine.rollback_state=nullptr;engine.netplay_frame=Netplay::INVALID_FRAME;
            world->rollback.reset();state.input_lanes={};
        }
        multiplayer_replay_escape=escape;
    }
    // A live display callback already pumped before its clock/input work.
    // A second pump here would send the deferred old tail before capture.
    if(callback_started<0&&!multiplayer_pump_network()){stopped=true;return 2;}
#endif
    advance_loading();if(error){stopped=true;return 2;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    auto& session=state.multiplayer_session;
    // Fresh multiplayer launches are configured before their first tick.
    // Once both opening draws exist, the real-time title wait may continue
    // loading/pumping but must not grow ANM/Common state at each local poll.
    // Keep this after advance_loading(): that owner completes the wait.
    if(session.configured&&!session.started&&title&&title->loading&&
       startup_view&&startup_view->elapsed>=2)return 0;
    if(multiplayer_generation_pending&&world&&!world->loading&&world->actors.session){
        if(multiplayer::live_motion::enabled)multiplayer::live_motion::Invalidate(this);
        touch_input_frontier.Invalidate();
        invalidate_remote_player_presentation();
        if(!state.netplay_runtime.BeginNextRun(session,engine.script_random.seed)){multiplayer_fail("Begin session generation");return 2;}
        if(!state.multiplayer_replay.NextGeneration(state.netplay_runtime.Generation())){error=-6;return 2;}
        state.multiplayer_cheat_movement_used=false;
        state.input_lanes={};input.player_profiles[0].input={};
        for(u32 seat=0;seat<world->player_count;++seat)world->pilots[seat].input_keys=0;
        world->audio_events.Reset();world->rollback.reset();
        engine.rollback_state=nullptr;engine.netplay_frame=Netplay::INVALID_FRAME;
        multiplayer_generation_pending=false;
    }
    // The fresh graph exists, but no tick or authored Draw may run before the
    // peers agree on this generation's session, seed and gameplay contract.
    if(session.sessionId&&session.started&&world&&!world->loading&&world->actors.session&&
       !state.netplay_runtime.CanStart())return 0;
    if(session.configured&&!session.started&&value.screen==4&&title&&!title->loading){
        if(session.sessionId&&!state.netplay_runtime.CanStart()&&!state.netplay_runtime.PreparingWorld())return 0;
        multiplayer::SpectatorStageState spectatorCheckpoint{};
        if(state.netplay_runtime.Spectator()){
            const auto& segments=state.netplay_runtime.StagePlaybackController().History().Segments();
            if(!state.netplay_runtime.StagePlaybackReady()||segments.empty())return 0;
            const auto& first=segments.front().checkpoint;
            if(!multiplayer::DecodeSpectatorStageState(first.state,spectatorCheckpoint)){
                error=-6;return 2;
            }
        }
        if(session.sessionId&&!state.netplay_runtime.Playback()&&!state.netplay_runtime.Spectator()&&
           !state.netplay_runtime.PreparingWorld()&&!state.multiplayer_replay.Begin(session,state.configuration)){error=-6;return 2;}
        // Title animation has an endpoint-local duration. Construct the match
        // only from the agreed seed, never the title's consumed random stream.
        engine.script_random.seed=engine.visual_random.seed=u16(session.seed);
        engine.script_random.calls=engine.visual_random.calls=0;
        if(!ensure_world())return 2;
        if(!startup->scores->multiplayer_active()){
            const bool records=state.netplay_runtime.Playback()||state.netplay_runtime.Spectator()?
                startup->scores->begin_replay():startup->scores->begin_multiplayer();
            if(!records){error=-6;return 2;}
        }
        state.multiplayer_cheat_movement_used=false;
        world->player_count=session.playerCount;
        world->local_player=state.netplay_runtime.Spectator()?Netplay::MAX_PLAYERS:session.localPlayer;
        for(u32 seat=0;seat<session.playerCount;++seat){
            state.pilot_games[seat].character=i32(session.loadouts[seat].character);
            state.pilot_games[seat].shot_type=i32(session.loadouts[seat].shot);
        }
        state.game.difficulty=i32(session.difficulty);
        const bool spectator=state.netplay_runtime.Spectator();
        state.game.stage=spectator?i32(spectatorCheckpoint.checkpoint.label&255u):
                         session.difficulty==4?7:1;
        state.game.reserved_040=spectator?spectatorCheckpoint.checkpoint.reservedStage:u32(state.game.stage);
        state.game.flags=spectator?(spectatorCheckpoint.checkpoint.pilots[0].flags&~0xbu):0;
        if(spectator){
            world->spectator_checkpoint_pending=spectatorCheckpoint;
            world->spectator_checkpoint_restore_pending=true;
        }
        state.current_stage=menu_data(state.chinese).stages+state.game.stage;
        engine.script_random.seed=engine.visual_random.seed=u16(session.seed);
        engine.script_random.calls=engine.visual_random.calls=0;
        state.pending_screen=7;session.started=true;
    }
#endif
    value.ui_camera=engine.ui;value.world_camera=engine.world;
    // A platform presentation clock may own the 60 Hz accumulator. Run one
    // original tick when requested, while retaining the real monotonic clock
    // for frame cost, FPS statistics, screenshots and other device services.
    // This is a platform-clock deadline, not a gameplay calculation. Do the
    // subtraction in native double precision: the game's x87 single mode can
    // round a microsecond away after 32 seconds and accidentally skip a tick.
    if(scheduled_tick)clock.next_frame_time=time().to_double()-0.000001;
    const i32 result=clock.step(loop);sync_views();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer_replay_scope&&value.screen==4&&(!world||!world->actors.session))multiplayer_finish_replay();
    if(!multiplayer_replay_scope&&value.screen==4&&(!world||!world->actors.session)&&
       state.multiplayer_session.started&&startup&&startup->scores)
        startup->scores->end_multiplayer();
    // An offline MP viewer cannot fall into an ordinary single-seat attract
    // demo after idling at its native title/Replay menu.
    if(!state.multiplayer_session.configured)state.inactive_frames=0;
#endif
    if(error){stopped=true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        invalidate_remote_player_presentation();
#endif
        return 2;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Stopped-loop callers have no browser presentation owner. Live rAF uses
    // the matching StagePlaybackReady branch in ApplicationHost instead.
    if(callback_started<0&&state.netplay_runtime.StagePlaybackReady())
        presentation_draw(1.f,false,false);
#endif
    if(writer_pending){writer_pending=false;Screenshot{screenshots}.write();}
    if(result){stopped=true;save();}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // A recovered endpoint is publishable even when no additional forward
    // tick could run. Correcting/open-frame guards reject partial history.
    publish_touch_input_frontier();
    publish_remote_player_presentation();
#endif
    return result;
}
bool Application::presentation_draw(float alpha,bool interpolate,bool world_interpolate){
    if(stopped||!initialized||error)return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // A received checkpoint can precede construction of its visible Stage
    // graph. Primitive VM storage is uninitialized until restart/restore.
    if(state.netplay_runtime.StagePlaybackReady()&&(!world||!world->backgrounds.current||
       !world->backgrounds.current->instruction))return false;
    if(state.netplay_runtime.Correcting()||multiplayer_frame_open||
       (world&&world->rollback_resimulating))return false;
#endif
    const auto saved_world=engine.world,saved_ui=engine.ui;auto* saved_active=engine.active;
    const auto saved_value_world=value.world_camera,saved_value_ui=value.ui_camera;auto* saved_value_active=value.active_camera;
    const u32 saved_screen_space=engine.screen_space,saved_fog=engine.fog_enabled,saved_graphics=graphics_state;
    const auto saved_reserved=manager->reserved_050,saved_submitted=manager->submitted_draws,saved_started=manager->started_scripts,saved_flushed=manager->flushed_batches;
    AnmVm saved_characters{},saved_small{};i32 saved_text_count=0,saved_early_count=0;u32 saved_text_color=0;Vec2 saved_text_scale{};i32 saved_text_camera=0,saved_text_shadow=0;
    if(common_view){saved_characters=common_view->characters;saved_small=common_view->small_characters;saved_text_count=common_view->text_count;saved_early_count=common_view->early_text_count;saved_text_color=common_view->color;saved_text_scale=common_view->scale;saved_text_camera=common_view->camera;saved_text_shadow=common_view->shadow;}
    high_refresh::begin(alpha,interpolate,true,world_interpolate);presentation_audit::begin_sample(alpha);
    engine.flush();value.active_camera=&value.ui_camera;configure_camera(value.ui_camera,true);engine.device.viewport(value.active_camera->viewport);value.screen_space=1;
    bool presented=false;
    if(engine.device.begin_scene()>=0){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(multiplayer::live_motion::enabled)
            multiplayer::live_motion::BeginDraw(*this,multiplayer::live_motion::DrawKind::Presentation);
#endif
        auto& animations=*manager;animations.batch_quads=0;animations.vertex_write=animations.batch_start=animations.vertex_buffer;graphics_state=255;
        u32 fog=saved_fog;ApplicationLoop::disable_fog(value,fog,loop);engine.draw_all();engine.flush();engine.device.texture(nullptr);engine.device.end_scene();
#ifdef TH_ENABLE_THPRAC
        if(auto* renderer=touhou::sdl::current())ThpracUi::render(*this,*renderer);
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(multiplayer::live_motion::enabled)multiplayer::live_motion::SealDraw(*this);
#endif
        presented=engine.device.present_frame()>=0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(presented)multiplayer_remote_endpoint_presented=!interpolate||!world_interpolate||alpha>=1.f;
#endif
    }
    engine.world=saved_world;engine.ui=saved_ui;engine.active=saved_active;engine.screen_space=saved_screen_space;engine.fog_enabled=saved_fog;graphics_state=saved_graphics;
    manager->reserved_050=saved_reserved;manager->submitted_draws=saved_submitted;manager->started_scripts=saved_started;manager->flushed_batches=saved_flushed;
    if(common_view){common_view->characters=saved_characters;common_view->small_characters=saved_small;common_view->text_count=saved_text_count;common_view->early_text_count=saved_early_count;common_view->color=saved_text_color;common_view->scale=saved_text_scale;common_view->camera=saved_text_camera;common_view->shadow=saved_text_shadow;}
    value.world_camera=saved_value_world;value.ui_camera=saved_value_ui;value.active_camera=saved_value_active;presentation_audit::end_frame();high_refresh::end();return presented;
}
void Application::presentation_frame(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Called by the host only after a real committed Present. Never consume
    // merely because an authored Draw queued an endpoint in deferred mode.
    if(multiplayer_remote_endpoint_presented&&world&&remote_player_presentation_progress().Complete()&&
       world->remote_player_presentation_revision==multiplayer_rollbacks)
        world->remote_player_presentation.Consume(state.netplay_runtime.LastSimulatedFrame(),multiplayer_rollbacks);
#endif
    const double now=time().to_double();if(!presentation_origin){presentation_origin=now;presentation_frames=0;}++presentation_frames;const double elapsed=now-presentation_origin;if(elapsed<.5)return;
    presentation_fps=float(double(presentation_frames)/elapsed);presentation_origin=now;presentation_frames=0;
}
i32 Application::draw_statistics(){
    if(high_refresh::render_only){
        if(state.pending_screen!=14&&common_view){
            const u32 color=presentation_fps<30?0xff5050ff:presentation_fps<40?0xffa0a0ff:0xffffffff;
            for(i32 i=common_view->text_count-1;i>=0;--i){auto& entry=common_view->text[i];if(entry.position.x==590&&entry.position.y==470){std::snprintf(entry.text,sizeof(entry.text),"%.2ffps",presentation_fps);entry.color=color;break;}}
        }return 1;
    }
    statistics->actual_ticks=state.active_time;statistics->expected_ticks=state.total_time;const auto result=statistics->draw(rates);state.active_time=statistics->actual_ticks;state.total_time=statistics->expected_ticks;if(world)world->measured_fps=statistics->frames_per_second;return result;
}
void Application::save(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer_replay_scope)return;
#endif
    config.save("th10.cfg",state.configuration);if(startup&&startup->scores)startup->scores->save();
}
void Application::shutdown(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer::live_motion::enabled){
        multiplayer::live_motion::Stop(this);multiplayer::live_motion::Invalidate(this);
    }
    touch_input_frontier.Invalidate();
    invalidate_remote_player_presentation();
#endif
    if(writer_pending){writer_pending=false;Screenshot{screenshots}.write();}
    // Wrappers reference Startup's common resources and score store. Release
    // every dependent owner before the store, including retained replay pools.
    if(initialized)save();dispose(title);dispose(credits);dispose(world);sync_views();
    if(statistics){statistics->shutdown(rates);std::free(statistics);statistics=nullptr;}
    dispose(startup);sync_views();stopped=true;
}
void Application::configure_camera(Camera& camera,bool flat){
    const bool ui=&camera==&value.ui_camera;auto& native=ui?engine.ui:engine.world;native=camera;engine.active=&native;engine.screen_space=ui?1:0;engine.configure_camera(flat);camera=native;value.active_camera=&camera;
}
#ifndef TH_NATIVE_PLATFORM
bool Application::invoke(CallbackToken token,void*,i32& result){
    switch(token){
    case callback_id::ApplicationUpdate:result=ApplicationFrame{value,frames}.update();return true;
    case callback_id::ApplicationBeginDraw:value.ui_camera=engine.ui;result=ApplicationFrame{value,frames}.begin_draw();return true;
    case callback_id::ApplicationDrawBarrier:result=1;return true;
    case callback_id::ApplicationFinishDraw:result=ApplicationFrame{value,frames}.finish_draw();return true;
    case callback_id::FrameStatisticsDraw:result=draw_statistics();return true;
    default:return false;
    }
}
#endif
AppScreens::AppScreens(Application& a):owner(a){
    game=&a.state.game;current_game=&a.session_view;current_title=&a.title_view;current_ending=&a.ending_view;current_startup=&a.startup_view;current_replay=&a.state.replay;stages=menu_data(a.state.chinese).stages;current_stage=&a.state.current_stage;registry=&a.engine.manager.registry;loading_ids=a.loading_ids;return_menu=&a.state.return_screen;loading_pause=&a.loading_pause;
}
void AppScreens::enter_lock(ApplicationState&,u32){}void AppScreens::leave_lock(ApplicationState&,u32){}
StartupScreen* AppScreens::create_startup_screen(ApplicationState&){auto& a=owner;a.startup=create<Startup>(a.state,a.engine,a.files,a.audio);if(!a.startup||!a.startup->initialize()){dispose(a.startup);a.error=-1;}a.sync_views();return a.startup_view;}
void AppScreens::destroy_screens(ApplicationState&){owner.shutdown();}
void AppScreens::create_title(){auto& a=owner;if(!a.ensure_world())return;a.title=create<Title>(a.state,a.engine,*a.startup->shared,a.fonts,a.input,a.audio,*a.startup->scores,a.effects);if(a.title){a.title->startup=a.startup->value;a.title->results=&a.world->results_services();if(!a.title->initialize()){dispose(a.title);a.error=-1;}}else a.error=-1;a.sync_views();}
void AppScreens::destroy_title(TitleMenu*){dispose(owner.title);owner.sync_views();}
void AppScreens::create_game(i32 mode){auto& a=owner;if(!a.ensure_world())return;
#ifdef TH_ENABLE_THPRAC
    // th08 GameApplication::enter_game parity: when a game is entered from the
    // title (screen 4), re-derive the live practice run for THIS launch from the
    // launch flags instead of trusting the previous run. A normal launch has no
    // practice flag, so active and the trainer cheats are cleared and a stale
    // thprac run can never be applied to it; a practice launch/restart keeps
    // them. Replay mode derives its run from the PRAC block in the replay.
    if(a.value.previous_screen==4){
        auto& p=a.state.practice;
        p.cheats=0;p.assisted=false;p.replay=mode!=0;
        p.active=p.enabled&&p.run.mode==1&&(p.replay||(a.state.game.flags&0x10)!=0);
    }
#endif
    a.world->new_game=a.value.new_game;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer::live_motion::enabled)multiplayer::live_motion::Invalidate(&a);
    a.touch_input_frontier.Invalidate();
    a.invalidate_remote_player_presentation();
#endif
    if(!a.world->start(mode))a.error=-1;a.sync_views();}
void AppScreens::destroy_game(GameSession*){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer::live_motion::enabled)multiplayer::live_motion::Invalidate(&owner);
    owner.touch_input_frontier.Invalidate();
    owner.invalidate_remote_player_presentation();
#endif
    if(owner.world)owner.world->stop_session();owner.sync_views();}
void AppScreens::create_ending(){auto& a=owner;if(!a.ensure_world())return;a.credits=create<Credits>(*a.world,a.captures);if(!a.credits||!a.credits->initialize()){dispose(a.credits);a.error=-1;}a.sync_views();}
void AppScreens::destroy_ending(Ending*){dispose(owner.credits);owner.sync_views();}
void AppScreens::destroy_startup(StartupScreen*){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(multiplayer::live_motion::enabled)multiplayer::live_motion::Invalidate(&owner);
    owner.touch_input_frontier.Invalidate();
    owner.invalidate_remote_player_presentation();
#endif
    dispose(owner.credits);dispose(owner.world);dispose(owner.startup);owner.sync_views();}
void AppScreens::destroy_replay(Replay* replay){if(owner.world)owner.world->release_replay(replay);else __builtin_trap();owner.sync_views();}
u32 AppScreens::create_loading_animation(AnmFile& file,i32 script){auto& e=owner.engine;return e.manager.create(file,script,15,AnimationPlacement::WorldBack,e,e);}
// Screen resource owners use typed cooperative loading tasks; no raw callback
// address or opaque worker argument is executed by this adapter.
u32 AppScreens::begin_thread(CallbackToken,void*,u32,u32&){__builtin_trap();}
u32 AppScreens::wait_thread(u32,u32){return 0;}void AppScreens::close_thread(u32){}void AppScreens::sleep(u32){owner.audio.pump();}
}

namespace th10::browser {
void Application::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(callback_id::ApplicationUpdate,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);return ApplicationFrame{s.value,s.frames}.update();});
 b.bind(callback_id::ApplicationBeginDraw,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);s.value.ui_camera=s.engine.ui;return ApplicationFrame{s.value,s.frames}.begin_draw();});
 b.bind(callback_id::ApplicationDrawBarrier,this,[](void*,void*,i32){return 1;});
 b.bind(callback_id::ApplicationFinishDraw,this,[](void* p,void*,i32){auto& s=*static_cast<Application*>(p);return ApplicationFrame{s.value,s.frames}.finish_draw();});
 b.bind(callback_id::FrameStatisticsDraw,this,[](void* p,void*,i32){return static_cast<Application*>(p)->draw_statistics();});
}
}
