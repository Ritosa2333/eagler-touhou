// Included in BrowserRuntime's private namespace. Transport, physical capture,
// published replays and device output deliberately live outside the game heap.
unsigned mp_advanced=0;u32 mp_committed_score=0;bool mp_committed_complete=false;
#if TH15_WRITE_JOURNAL
namespace journal=Netplay::WriteJournal;
struct MpRollbackFrame {
 u32 frame=0,stage=1;GameInput menu_before;unsigned stage_before=1,spectator_before=0;
 u64 anm_before=0;touhou::sdl::State graphics_before;
 std::vector<int> font_tabs;
 PresentationFrame::Snapshot presentation_before;
 std::vector<rollback::AudioEvent> audio;
 Netplay::SpectatorStagePublisher::Frame spectator;bool has_spectator=false;
 u32 score=0;bool complete=false;
};
std::deque<MpRollbackFrame> mp_frames;
u32 mp_boundary=Netplay::INVALID_FRAME,mp_corrections=0,mp_replayed=0,mp_barriers=0;
void mp_reset_rollback(){
 rollback::End();mp_frames.clear();journal::Clear();mp_boundary=Netplay::INVALID_FRAME;
 if(app)app->graphics.clear_rollback_history();
 mp_corrections=mp_replayed=mp_barriers=mp_advanced=mp_committed_score=0;mp_committed_complete=false;
}
bool mp_journal_error(const char* operation){
 last_error=std::string(operation)+": "+journal::Error();return false;
}
bool mp_restore(u32 first){
 const auto found=std::find_if(mp_frames.begin(),mp_frames.end(),[&](const auto& record){return record.frame==first;});
 if(found==mp_frames.end()){last_error="TH15 rollback frame is outside retained game state";return false;}
 const auto menu=found->menu_before;const auto stage=found->stage_before,spectator=found->spectator_before;
 const auto anm=found->anm_before;const auto state=found->graphics_before;
 app->graphics.backend.cancel_present();
 if(!journal::RestoreTo(first))return mp_journal_error("TH15 state restore failed");
 mp_menu=menu;mp_stage=stage;mp_spectator_stage=spectator;AnmVm::presentation_counter=anm;
 app->fonts.restore_layout(found->font_tabs);
 app->graphics.backend.state=state;app->graphics.rollback_restore(!found->presentation_before);app->renderer.invalidate();
 app->graphics.presentation.restore(std::move(found->presentation_before),app->graphics.backend);
 mp_frames.erase(found,mp_frames.end());mp_boundary=Netplay::INVALID_FRAME;
 return true;
}
bool mp_commit_audio(const std::vector<rollback::AudioEvent>& events){
 TH15_SYSTEM_SCOPE;auto& device=app->audio_device;
 for(const auto& event:events){bool ok=true;using Kind=rollback::AudioKind;
  switch(event.kind){
   case Kind::Sound:ok=device.effects.enqueue(event.first);break;
   case Kind::Positioned:ok=device.effects.positioned(event.first,event.position);break;
   case Kind::Stop:ok=device.effects.stop(event.first);break;
   case Kind::Pan:device.sound_pan(u32(event.first),event.second);break;
   case Kind::Suspend:device.effects.suspend();break;
   case Kind::Resume:device.effects.resume();break;
   case Kind::Music:ok=device.queue_music(event.first,event.second,event.text);break;
   case Kind::Track:ok=device.queue_music_track(event.first,event.text);break;
   case Kind::PrepareMusic:ok=device.prepare_music(event.text);break;
   case Kind::PauseMusic:device.pause_music(event.first!=0);break;
   case Kind::Volume:device.music_volume=event.first;device.effects.master_volume=event.second;device.refresh_volume();break;
   case Kind::ClearWave:device.clear_current_wave();break;
   case Kind::Finish:ok=device.finish_music_requests();break;
   case Kind::Update:device.update();break;
   case Kind::Pump:device.pump();break;
  }
  if(!ok||!device.error.empty()||!device.effects.error.empty()){
   last_error=device.error.empty()?device.effects.error:device.error;return false;
  }
 }
 return true;
}
bool mp_commit_reconciled(){
 if(network.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
 const auto last=network.LastFrame(),confirmed=network.Confirmed();
 if(last==Netplay::INVALID_FRAME||confirmed==Netplay::INVALID_FRAME)return true;
 if(network.Correcting()&&!network.InputsReconciledBeforeNext())return true;
 const auto through=std::min(last,confirmed),previous=network.CommittedThrough();
 if(previous!=Netplay::INVALID_FRAME&&through<=previous)return true;
 // Exact input can no longer change this prefix. Release its undo state before
 // making the output watermark available to the network/spectator publisher.
 journal::DiscardBefore(through+1);
 if(!network.CommitReconciledPrefix(through)){last_error="TH15 confirmed output frontier rejected";return false;}
 while(!mp_frames.empty()&&mp_frames.front().frame<=through){
  auto& record=mp_frames.front();std::array<Netplay::FrameInput,3> exact;
  if(!network.CommittedPrefixInputs(record.frame,exact)){last_error="TH15 output input prefix is not exact";return false;}
  if(mp_recording&&!mp_replay.Append(record.frame,record.stage,exact.data(),network.Setup().player_count)){
   last_error="MP recording capacity exhausted";return false;
  }
  if(!mp_commit_audio(record.audio))return false;
  mp_committed_score=record.score;mp_committed_complete=record.complete;
  if(record.has_spectator){record.spectator.inputs=exact;network.RecordSpectatorFrame(std::move(record.spectator));}
  mp_frames.pop_front();
 }
 network.PublishAfterReconcile();return true;
}
enum class MpStep { Executed, Boundary, Failed };
MpStep mp_execute_rollback(const Netplay::FrameDecision& decision,bool capture_undo){
 const auto frame=network.NextFrame();
 mp_frames.emplace_back();auto& record=mp_frames.back();record.frame=frame;
 record.menu_before=mp_menu;record.stage_before=mp_stage;record.spectator_before=mp_spectator_stage;
 record.anm_before=AnmVm::presentation_counter;record.graphics_before=app->graphics.backend.state;
 record.font_tabs=app->fonts.rollback_layout();
 record.presentation_before=app->graphics.presentation.snapshot();
 app->graphics.presentation.retain_authored=true;
 if(capture_undo){
  if(!journal::BeginFrame(frame)){mp_frames.pop_back();mp_journal_error("TH15 snapshot begin failed");return MpStep::Failed;}
  rollback::Begin(record.audio);
 }
 std::array<MultiplayerInput,3> lanes;
 if(!multiplayer::InputLanes::Decode(decision.inputs.data(),network.Setup().player_count,lanes)){
  rollback::End();if(capture_undo)journal::EndFrame();last_error="Invalid TH15 rollback inputs";return MpStep::Failed;
 }
 u32 menu=0;for(unsigned seat=0;seat<network.Setup().player_count;++seat)menu|=(lanes[seat].held&~0x100u)|(lanes[seat].pause?0x100u:0);
 if(app->title||(app->pause&&app->pause->state.screen!=PauseScreen::Inactive))menu=(lanes[0].held&~0x100u)|(lanes[0].pause?0x100u:0);
 mp_menu.update(menu);const auto activation=app->replay?app->replay->activation_count():0;
 bool ok=false;{
  TH15_GAME_SCOPE;
  if(app->scene()){mp_stage=unsigned(app->progress.stage);app->scene()->battle.mp_input(lanes);}
  record.stage=mp_stage;
  ok=app->step(mp_menu.held,mp_menu.pressed,mp_menu.repeated,60,false);
  if(capture_undo&&(app->pending_load||app->exiting||app->title))rollback::RequireConfirmed();
 }
 const bool boundary=capture_undo&&rollback::boundary;rollback::End();
 if(capture_undo&&!journal::EndFrame()){mp_journal_error("TH15 snapshot seal failed");return MpStep::Failed;}
 if(boundary){
  if(!mp_restore(frame))return MpStep::Failed;mp_boundary=frame;++mp_barriers;return MpStep::Boundary;
 }
 if(!ok){last_error=app->failure;if(capture_undo)mp_restore(frame);return MpStep::Failed;}
 if(network.Setup().local_player==0){
  auto& spectator=record.spectator;spectator.frame=frame;spectator.stage=mp_spectator_stage;spectator.inputs=decision.inputs;spectator.hasInputs=true;
  if(app->scene()&&app->replay&&app->replay->activation_count()!=activation&&unsigned(app->progress.stage)!=mp_spectator_stage){
   if(multiplayer::SpectatorReplayStart::Capture(*app,network.Setup(),record.menu_before,frame,spectator.initializer,mp_spectator_error)){
    spectator.schema=multiplayer::SpectatorStageState::Schema;spectator.stage=mp_spectator_stage=unsigned(app->progress.stage);
   }
  }
  record.has_spectator=mp_spectator_stage!=0;
 }
 if(app->scene()){
  const auto& battle=app->scene()->battle;const auto seat=network.Setup().local_player;
  record.score=u32(battle.mp_enabled&&seat<battle.mp_options.seat_count?battle.pilots[seat]->score.score:battle.score.score);
 }else record.score=u32(app->selection_score.score);
 record.complete=app->completed_recording;
 if(!network.MarkSimulated(frame,decision)){last_error=network.Error();return MpStep::Failed;}
 return MpStep::Executed;
}
bool mp_rollback_tick(u32 held,bool physical){
 if(!journal::Ready())return mp_journal_error("TH15 rollback backend unavailable");
 if(!network.CanStart())return true;
 if(!network.WorldReady()&&!network.SetWorldReady(true)){last_error="TH15 rollback world admission failed";return false;}
 if(!mp_recording){
  if(!mp_replay.Begin(network.Setup(),std::uint64_t(std::time(nullptr)))){last_error="MP recording initialization failed";return false;}
  mp_recording=true;
 }
 const auto begin=emscripten_get_now();unsigned replayed=0;
 for(;;){
  const auto first=network.RollbackFrame();
  if(first!=Netplay::INVALID_FRAME){
   const bool restarting=network.Correcting();
   if(!mp_restore(first)||!(restarting?network.RestartCorrection(first):network.BeginCorrection(first))){
    if(last_error.empty())last_error="TH15 correction cursor rejected restored state";return false;
   }
   ++mp_corrections;
  }
  if(!mp_commit_reconciled())return false;
  if(network.Correcting()&&network.NextFrame()==network.CorrectionEnd()){
   if(!network.EndCorrection()){last_error="TH15 correction completion rejected";return false;}
   if(!mp_commit_reconciled())return false;return true;
  }
  const bool correcting=network.Correcting();const auto frame=network.NextFrame();
  if(mp_boundary==frame){
   const auto confirmed=network.Confirmed();
   if(confirmed==Netplay::INVALID_FRAME||confirmed<frame||!network.InputsReconciledBeforeNext())return true;
   if(correcting&&!network.EndCorrection(true)){last_error="TH15 lifecycle correction fence rejected";return false;}
   const auto exact=network.Prepare(frame);if(!exact.canAdvance||exact.predictedMask)return true;
   // All earlier outputs have committed and this input is exact. The barrier
   // can now create/free GPU resources, save files and enter another stage.
   if(!mp_frames.empty()||journal::FrameCount()){last_error="TH15 lifecycle reached unreconciled state";return false;}
   mp_boundary=Netplay::INVALID_FRAME;
   if(mp_execute_rollback(exact,false)!=MpStep::Executed||!mp_commit_reconciled())return false;
   if(!correcting)++mp_advanced;
   if(app->title){mp_local_results=true;mp_menu={};}
   if(app->exiting&&!network.Finish()&&*network.Error()){last_error=network.Error();return false;}
   return true;
  }
  if(!correcting){
   if(network.NeedsCapture()){
    if(!physical)return true;Netplay::FrameInput sampled;
    if(!multiplayer::InputLanes::Capture(held,(held&0x100)!=0,app->touch.mode,app->touch.x,app->touch.y,sampled)||!network.Capture(network.CaptureFrame(),sampled)){
     last_error=network.Error();return false;
    }
   }
   if(!network.Pump(true)){last_error=network.Error();return false;}
   if(network.RollbackFrame()!=Netplay::INVALID_FRAME)continue;
  }
  const auto decision=network.Prepare(frame);if(!decision.canAdvance)return true;
  const auto result=mp_execute_rollback(decision,true);if(result==MpStep::Failed)return false;
  if(result==MpStep::Boundary)continue;
  if(!correcting){++mp_advanced;return mp_commit_reconciled();}
  ++mp_replayed;++replayed;
  if(!mp_commit_reconciled())return false;
  if(replayed>=4||emscripten_get_now()-begin>=4){
   if(network.NextFrame()==network.CorrectionEnd())continue;
   app->graphics.backend.cancel_present();app->graphics.presentation.reset();return true;
  }
 }
}
const u32* mp_rollback_status(){
 static std::array<u32,20> values{};const auto stats=journal::Stats();
 values={journal::Ready()?1u:0u,network.Correcting()?1u:0u,network.NextFrame(),network.CaptureFrame(),network.CommittedThrough(),
  u32(mp_frames.size()),stats.last_blocks,stats.peak_blocks,mp_corrections,mp_replayed,mp_barriers,
  stats.restores,stats.restored_frames,u32(stats.used_bytes>>10),u32(stats.capacity_bytes>>10),stats.heap_used_kib,stats.heap_capacity_kib,
  mp_boundary,mp_committed_score,mp_committed_complete?1u:0u};return values.data();
}
#else
void mp_reset_rollback(){mp_advanced=mp_committed_score=0;mp_committed_complete=false;}
bool mp_rollback_tick(u32,bool){last_error="This TH15 build has no instrumented rollback state owner";return false;}
const u32* mp_rollback_status(){static const std::array<u32,20> values{};return values.data();}
#endif
