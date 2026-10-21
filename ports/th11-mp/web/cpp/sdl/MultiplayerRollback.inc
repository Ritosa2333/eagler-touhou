// Included inside Application, exclusively in the multiplayer source set.
// One complete transaction owns Update, text pixels, authored Draw and Seal.
struct MultiplayerRollbackTick {
    u32 frame=0,stage=0,score=0;bool completed=false;
    std::unique_ptr<WorldRollbackFrame> world;
    std::unique_ptr<TextureRollbackFrame> texture;
    std::vector<BattleEvent> sounds;
};
Netplay::RollbackJournal rollback_journal;
std::unique_ptr<WorldRollback> rollback_world;
std::deque<std::unique_ptr<MultiplayerRollbackTick>> rollback_frames;
touhou::input::TouchState multiplayer_touch_frontier{};
bool multiplayer_touch_valid=false,rollback_network_wait=false,rollback_cpu_wait=false;
bool rollback_transaction_advanced=false;
u32 rollback_barrier=Netplay::INVALID_FRAME,rollback_published=0,multiplayer_touch_frame=0;
u32 rollback_restores=0,rollback_history_ticks=0,rollback_peak_depth=0;
u32 rollback_exact_history_ticks=0;
u32 rollback_peak_bytes=0,rollback_peak_blocks=0,rollback_revisions=0;
u32 rollback_committed_score=0;bool rollback_committed_completed=false;
double rollback_callback_ms=-1;
std::array<u32,24> rollback_status{};
// Optional wall-time bins, excluded from all game/checkpoint state. Disabled by
// default; diagnostics enable explicitly and account for their own observation.
bool rollback_profile_enabled=false;
std::array<double,16> rollback_profile{};
struct RollbackTimer {
    double* value;double begin;
    explicit RollbackTimer(double* target):value(target),begin(target?emscripten_get_now():0){}
    ~RollbackTimer(){if(value)*value+=emscripten_get_now()-begin;}
};
RollbackTimer rollback_timer(unsigned bin){return RollbackTimer(rollback_profile_enabled?&rollback_profile[bin]:nullptr);}
bool rollback_fail(const char* message){
    if(rollback_world)rollback_world->EndRollbackFrame();
    graphics.EndTextureRollbackFrame();graphics.CancelTextureRollbackRestore();
    graphics.backend.cancel_present();
    if(error.empty())error=message;return false;
}
void rollback_release(){
    graphics.backend.flush_raster();graphics.backend.deferredRaster=false;
    graphics.backend.residentUploads=false;
    graphics.backend.queuedUploads=false;
    if(rollback_world)rollback_world->EndRollbackFrame();
    graphics.EndTextureRollbackFrame();graphics.CancelTextureRollbackRestore();
    graphics.backend.cancel_present();
    // All raw records disappear before leases. Never leave raw undo pointers
    // alive while destroying the typed frame that keeps their storage alive.
    rollback_journal.Clear();rollback_frames.clear();rollback_world.reset();
    rollback_barrier=Netplay::INVALID_FRAME;rollback_published=0;
    multiplayer_touch_valid=false;rollback_network_wait=rollback_cpu_wait=false;
    rollback_restores=rollback_history_ticks=rollback_peak_depth=0;
    rollback_exact_history_ticks=0;
    rollback_peak_bytes=rollback_peak_blocks=rollback_revisions=rollback_committed_score=0;rollback_committed_completed=false;
    graphics.ClearTextureRollbackBoundaryRequest();
}
bool rollback_initialize(){
    rollback_release();
    if(!netplay.AllowsRollback()||netplay.ReadOnly())return true;
    if(!rollback_journal.Reset({14,32*1024*1024,65536,true,true}))
        return rollback_fail("TH11 rollback history allocation failed");
    graphics.backend.residentUploads=true;
    graphics.backend.queuedUploads=true;
    graphics.backend.deferredRaster=true;
    rollback_world=std::make_unique<WorldRollback>(session);return true;
}
void refresh_multiplayer_touch_frontier(){
    touhou::input::TouchState value;
    if(netplay.ReadOnly())value.context=3;
    else if(session.state.phase==GameSessionPhase::ending)value.context=2;
    else if(session.state.phase==GameSessionPhase::game_over)value.context=netplay.Setup().local_player==0?0:3;
    else if(session.state.phase==GameSessionPhase::stage&&multiplayer_world){
        if(session.battle&&session.battle->dialogue&&session.battle->dialogue->active)value.context=2;
        else {
            const auto seat=session.multiplayer_seat(netplay.Setup().local_player);
            value.context=1;value.instance=int(session.state.stage);
            value.ready=seat.active&&!seat.ghost&&seat.life_state==1;
            value.x=seat.x;value.y=seat.y;value.fast=seat.fast_speed;value.slow=seat.slow_speed;
            value.min_x=-184;value.max_x=184;value.min_y=32;value.max_y=432;
        }
    }
    multiplayer_touch_frontier=value;multiplayer_touch_valid=true;multiplayer_touch_frame=netplay.NextFrame();
}
bool rollback_capture_renderer(){
    return rollback_capture_values(rollback_journal,renderer.offset,renderer.viewport,renderer.camera,
        renderer.fog_origin,renderer.fog_channels,renderer.fog_start,renderer.tint_enabled,renderer.tint,
        renderer.multiplayer_animations,renderer.multiplayer_hud_background,renderer.multiplayer_opacity,
        graphics.backend.state);
}
bool rollback_restore(u32 first){
    auto measured=rollback_timer(3);
    const auto begin=std::find_if(rollback_frames.begin(),rollback_frames.end(),[&](const auto& frame){return frame->frame==first;});
    if(begin==rollback_frames.end()||!(*begin)->world||first<rollback_published)
        return rollback_fail("TH11 correction has no retained checkpoint");
    std::vector<const TextureRollbackFrame*> textures;
    for(auto it=begin;it!=rollback_frames.end();++it){
        if(!(*it)->world||!rollback_world->ValidateRollbackFrame(*(*it)->world,(*it)->frame))
            return rollback_fail("TH11 rollback resource identity changed");
        textures.push_back((*it)->texture.get());
    }
    renderer.flush();graphics.backend.flush();graphics.backend.cancel_present();
    if(!graphics.PrepareTextureRollbackRestore(textures.data(),textures.size(),first))
        return rollback_fail("TH11 texture restore preflight failed");
    u32 restored=Netplay::INVALID_FRAME;
    if(!rollback_journal.UndoTo(first,&restored)||restored!=first||
       !rollback_world->RestoreRollbackFrame(*(*begin)->world,restored)||
       !graphics.FinishTextureRollbackRestore())
        return rollback_fail("TH11 complete world restore failed");
    // invalidate() flushes. Both queues are already empty, so this cannot
    // submit vertices whose owner was just rewound.
    renderer.invalidate();
    rollback_frames.erase(begin,rollback_frames.end());
    graphics.ClearTextureRollbackBoundaryRequest();++rollback_revisions;return true;
}
bool rollback_commit(bool reconciled_prefix=false){
    auto measured=rollback_timer(9);
    if((netplay.Correcting()&&!reconciled_prefix)||netplay.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
    if(netplay.Correcting()&&!netplay.InputsReconciledBeforeNext())return rollback_fail("TH11 unconfirmed prefix retirement rejected");
    const auto confirmed=netplay.Confirmed(),last=netplay.LastFrame();
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
    const auto through=std::min(confirmed,last);
    // Fresh one-tick checkpoints only. Retire raw records before typed leases,
    // then grant the publication watermark. No output can rewind afterwards.
    rollback_journal.DiscardBefore(through+1);
    for(auto& frame:rollback_frames)if(frame->frame<=through){frame->world.reset();frame->texture.reset();}
    if(!(reconciled_prefix?netplay.CommitReconciledPrefix(through):netplay.SetCommitLimit(through)))return rollback_fail("TH11 confirmed output watermark rejected");
    while(!rollback_frames.empty()&&rollback_frames.front()->frame<=through){
        const auto& frame=*rollback_frames.front();
        if(frame.frame!=rollback_published)return rollback_fail("TH11 confirmed output has a frame hole");
        for(const auto& event:frame.sounds){
            switch(event.kind){
            case BattleEventKind::Sound:
                if(event.positional)audio.effects.positioned(event.value,event.position.x);else audio.effects.enqueue(event.value);break;
            case BattleEventKind::StopSound:audio.effects.stop(event.value);break;
            case BattleEventKind::StageMusic:if(!play_stage_music(event.value))return false;break;
            case BattleEventKind::BossMusic:if(!request_music(stage_music(event.value,true))){error=audio.error;return false;}break;
            case BattleEventKind::MusicResume:pause_music(false);break;
            case BattleEventKind::MusicFade:audio.fade_music(i32(event.position.x*60.f));break;
            default:break;
            }
        }
        // Fade/effect processing advances once per confirmed logical frame,
        // including silent frames, never once per resimulation.
        if(!audio.update()){error=audio.error;return false;}
        std::array<Netplay::FrameInput,3> inputs{};
        if(!(reconciled_prefix?netplay.CommittedPrefixInputs(frame.frame,inputs):netplay.ConfirmedInputs(frame.frame,inputs))||
           !multiplayer_replay.Append(frame.frame,frame.stage,inputs.data(),netplay.Setup().player_count))
            return rollback_fail("TH11 corrected Replay append failed");
        rollback_committed_score=frame.score;rollback_committed_completed=frame.completed;
        ++rollback_published;rollback_frames.pop_front();
    }
    netplay.PublishConfirmedSpectatorHistoryAfterReconcile();
    return true;
}
bool rollback_seal(MultiplayerRollbackTick& frame){
    auto measured=rollback_timer(8);
    if(!frame.world)return true;
    rollback_world->EndRollbackFrame();
    const bool textures=graphics.EndTextureRollbackFrame();
    const bool sealed=rollback_journal.EndFrame();
    if(!textures||!sealed||rollback_journal.Failed())return rollback_fail("TH11 rollback checkpoint capacity or ownership failure");
    rollback_peak_bytes=std::max(rollback_peak_bytes,u32(rollback_journal.BytesForFrame(frame.frame)));
    rollback_peak_blocks=std::max(rollback_peak_blocks,u32(rollback_journal.BlocksForFrame(frame.frame)));
    return true;
}
bool rollback_abort(u32 frame,const char* reason){
    const bool boundary=rollback_world->BoundaryRequested()||graphics.TextureRollbackBoundaryRequested();
    if(!boundary||rollback_frames.empty()||!rollback_frames.back()->world)
        return rollback_fail(reason);
    if(!rollback_seal(*rollback_frames.back())||!rollback_restore(frame))return false;
    // The current frame has not been MarkSimulated. Only undo its title state;
    // the input core and its already captured local sample must not move.
    error.clear();rollback_barrier=frame;rollback_network_wait=true;return true;
}
bool rollback_run_frame(bool historical){
    rollback_transaction_advanced=false;const auto frame=netplay.NextFrame();
    const auto decision=netplay.Prepare(frame);
    if(!decision.canAdvance){rollback_network_wait=true;return true;}
    if(frame>=multiplayer::ReplayArchive::MaxFrames)return rollback_fail("TH11 rollback frame exceeds Replay capacity");
    std::array<MultiplayerInput,3> input{};
    if(!multiplayer::InputLanes::Decode(decision.inputs.data(),netplay.Setup().player_count,input))
        return rollback_fail("TH11 rollback input decode failed");
    auto record=std::make_unique<MultiplayerRollbackTick>();record->frame=frame;record->stage=session.state.stage;
    std::vector<u8> spectator_initializer;u32 spectator_stage=record->stage;
    if(netplay.SpectatorPublisherOwner())multiplayer_capture_spectator_checkpoint(frame,spectator_stage,spectator_initializer);
    // Replayed exact frames still live inside a correction. Retain a fresh
    // checkpoint until the complete old frontier has been reconstructed.
    const bool capture=true;
    if(capture){
        auto measured=rollback_timer(4);
        record->world=std::make_unique<WorldRollbackFrame>();record->texture=std::make_unique<TextureRollbackFrame>();
        renderer.flush();graphics.backend.flush();
        if(!rollback_journal.BeginFrame(frame)||
           !rollback_world->BeginRollbackFrame(rollback_journal,*record->world)||
           !graphics.BeginTextureRollbackFrame(rollback_journal,*record->texture)||
           !rollback_capture_renderer())return rollback_fail("TH11 complete checkpoint capture failed");
    }
    rollback_frames.push_back(std::move(record));auto& current=*rollback_frames.back();
    {auto measured=rollback_timer(5);
    eagler::input::GlobalTouchMotionTraceBank().set_context(session.multiplayer_frame+1,
        historical?eagler::input::TouchMotionTraceBank::resimulation:netplay.Playback()?eagler::input::TouchMotionTraceBank::replay:eagler::input::TouchMotionTraceBank::forward,
        historical||netplay.ReadOnly()?std::numeric_limits<float>::quiet_NaN():touch_motion_sensitivity());
    if(!session.update_multiplayer(input))return rollback_abort(frame,"TH11 rollback native Update failed");}
    current.sounds=std::move(session.battle->events);session.battle->events.clear();
    {auto measured=rollback_timer(6);
    if(!text_events())return rollback_abort(frame,"TH11 rollback text raster failed");}
    if(!historical)frame_statistics.sample(double(SDL_GetTicks())*.001,true);
    frame_text.clear();const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);
    {auto measured=rollback_timer(7);
    if(!session.draw(renderer,&frame_text))return rollback_abort(frame,"TH11 rollback authored Draw failed");}
    current.score=u32(std::max(0,session.economy.score_units));current.completed=multiplayer_completed();
    if(!rollback_seal(current))return false;
    if(!netplay.MarkSimulated(frame,decision))return rollback_fail(netplay.Error());
    if(netplay.SpectatorPublisherOwner()&&!netplay.RecordSpectatorFrame(frame,spectator_stage,spectator_initializer,decision.inputs)){
        // A spectator history failure is isolated from the confirmed player simulation.
    }
    if(historical){++rollback_history_ticks;if(rollback_profile_enabled)++rollback_profile[14];}
    rollback_transaction_advanced=true;multiplayer_last_stage=current.stage;
    return true;
}
bool multiplayer_tick(bool forward_due=true){
    multiplayer_advanced=false;rollback_network_wait=rollback_cpu_wait=false;
    if(!netplay.FlushDeferredInputs())return rollback_fail(netplay.Error());
    if(netplay.Spectator())return multiplayer_spectator_playback_tick(forward_due);
    if(!netplay.AllowsRollback()||netplay.ReadOnly()){
        if(!forward_due)return multiplayer_pump();
        const bool ok=multiplayer_exact_tick();if(ok&&multiplayer_advanced)refresh_multiplayer_touch_frontier();return ok;
    }
    // The rAF owner has already drained the transport before setting the
    // callback budget epoch. JS message events cannot run again inside this
    // synchronous Wasm call. Capture still sends new input immediately, and
    // explicit terminal/retirement pumps below remain unconditional.
    if((rollback_callback_ms<0&&!multiplayer_pump())||!multiplayer_world)return error.empty();
    if(multiplayer_finish_pending||!netplay.CanStart()){rollback_network_wait=true;return true;}
    if(!rollback_world)return rollback_fail("TH11 rollback world was not initialized");
    if(!multiplayer_replay.Recording()){
        if(!multiplayer_replay.Begin(netplay.Setup(),u64(std::time(nullptr))))return rollback_fail("Unable to reserve TH11 corrected Replay");
        session.recording_timestamp=multiplayer_replay.Timestamp();
    }
    bool corrected=false;unsigned replayed=0;
    const double started=rollback_callback_ms>=0?rollback_callback_ms:emscripten_get_now();
    for(;;){
        const auto request=netplay.RollbackFrame();
        if(request!=Netplay::INVALID_FRAME){
            const bool restarting=netplay.Correcting();const auto frontier=netplay.PredictionFrontier();
            if(!rollback_restore(request)||!(restarting?netplay.RestartCorrection(request):netplay.BeginCorrection(request)))
                return rollback_fail("TH11 correction input frontier rejected");
            rollback_barrier=Netplay::INVALID_FRAME;++rollback_restores;
            rollback_peak_depth=std::max(rollback_peak_depth,frontier-request);corrected=true;
        }
        if(!netplay.Correcting())break;
        if(rollback_barrier==netplay.NextFrame()){
            if(!netplay.InputsReconciledBeforeNext()){rollback_network_wait=true;return true;}
            if(!netplay.EndCorrection(true))return rollback_fail("TH11 corrected lifecycle boundary rejected");
            break;
        }
        if(netplay.NextFrame()==netplay.PredictionFrontier()){
            if(!netplay.EndCorrection())return rollback_fail("TH11 correction completion rejected");break;
        }
        if(!multiplayer::FrameSchedule::can_replay_next(replayed,netplay.PredictionFrontier()-netplay.NextFrame(),emscripten_get_now()-started)){rollback_cpu_wait=true;return true;}
        if(netplay.CanReplayConfirmed()){
            // Actual prefix, not just an exact current slot: no unknown input
            // before this frame may still invalidate the reconstructed world.
            // Retire bytes before their leases, then use the complete original
            // lifecycle transaction without taking a checkpoint it cannot need.
            if(!rollback_commit(true)||!multiplayer_exact_tick(true))return false;
            rollback_transaction_advanced=multiplayer_advanced;multiplayer_advanced=false;
            if(rollback_transaction_advanced){
                rollback_published=netplay.NextFrame();++rollback_history_ticks;++rollback_exact_history_ticks;
                if(rollback_profile_enabled)++rollback_profile[14];
            }
            if(multiplayer_finish_pending||!rollback_world->CanCapture()){
                // An actual Pause/result/resource transition can replace the
                // stable combat graph before the former predicted frontier.
                // Finish at this fully reconciled boundary. Remaining already
                // captured local inputs belong to the ordinary exact driver,
                // never to a combat checkpoint on a paused or retired world.
                if(!netplay.EndCorrection(true))return rollback_fail("TH11 confirmed history lifecycle fence rejected");
                corrected=true;break;
            }
        }else if(!rollback_run_frame(true))return false;
        if(!rollback_transaction_advanced){
            if(rollback_barrier==netplay.NextFrame())continue;
            return true;
        }
        ++replayed;corrected=true;
    }
    if(!rollback_commit())return false;
    if(corrected){
        refresh_multiplayer_touch_frontier();
        // A cancelled Draw can have touched the offscreen surface before its
        // resource barrier. Only a completed Draw may replace the canvas.
        if(rollback_barrier==Netplay::INVALID_FRAME)graphics.present();
    }
    if(multiplayer_finish_pending)return multiplayer_pump();
    if(netplay.NextFrame()>=multiplayer::ReplayArchive::MaxFrames){
        // Keep receiving, reconciling and publishing the final allowed frame.
        // Capacity is a graceful terminal boundary, not a prediction failure.
        if(!rollback_frames.empty()||!netplay.InputsReconciledBeforeNext()||
           netplay.CommittedThrough()!=multiplayer::ReplayArchive::MaxFrames-1){rollback_network_wait=true;return true;}
        multiplayer_finish_pending=true;multiplayer_restart_pending=false;return multiplayer_pump();
    }
    if(!forward_due)return true;
    const bool barrier=rollback_barrier==netplay.NextFrame();
    if(barrier||!rollback_world->CanCapture()){
        if(!rollback_frames.empty()||!netplay.InputsReconciledBeforeNext()){rollback_network_wait=true;return true;}
        if(!netplay.SetWorldReady(false))return rollback_fail("TH11 exact boundary admission failed");
        const auto decision=netplay.Prepare(netplay.NextFrame());
        if(!decision.canAdvance||decision.predictedMask){rollback_network_wait=true;return true;}
        const bool ok=multiplayer_exact_tick();
        if(ok&&multiplayer_advanced){rollback_barrier=Netplay::INVALID_FRAME;rollback_published=netplay.NextFrame();refresh_multiplayer_touch_frontier();}
        return ok;
    }
    if(!netplay.SetWorldReady(true))return rollback_fail("TH11 stable world admission failed");
    const auto decision=netplay.Prepare(netplay.NextFrame());
    if(decision.canAdvance&&!decision.predictedMask&&rollback_frames.empty()){
        // No retained future and exact all-seat input: the original full
        // lifecycle transaction needs neither a snapshot nor delayed output.
        if(!netplay.SetWorldReady(false))return rollback_fail("TH11 confirmed frame admission failed");
        const bool ok=multiplayer_exact_tick();
        if(ok&&multiplayer_advanced){rollback_published=netplay.NextFrame();refresh_multiplayer_touch_frontier();}
        return ok;
    }
    if(!rollback_run_frame(false))return false;
    if(!rollback_transaction_advanced)return true;
    multiplayer_advanced=true;
    if(!rollback_commit())return false;
    refresh_multiplayer_touch_frontier();graphics.present();return true;
}
