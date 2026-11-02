#include "NetplayRuntime.hpp"
#include "SpectatorStageSchema.hpp"
#include "InputLanes.hpp"
#include <algorithm>
#include <cmath>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#endif
namespace th12::multiplayer {
namespace {
std::uint64_t clock_us(){
#ifdef __EMSCRIPTEN__
    return std::uint64_t(emscripten_get_now()*1000);
#else
    return std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
Netplay::SessionChannelConfig channel_policy(unsigned mode,unsigned allowance=0){
    Netplay::SessionChannelConfig p;p.adonisPhase=mode!=0;p.adonisPredictionFrames=p.adonisPhase?allowance:0;p.inputResendMs=16;return p;
}
bool valid_input(const Netplay::FrameInput& input){
    const std::array<Netplay::FrameInput,3> wire{input,{}, {}};
    std::array<Input,3> native;
    return !input.touchBomb&&InputLanes::Decode(wire.data(),2,native);
}
}
bool NetplayRuntime::Fail(const char* reason){if(error_.empty())error_=reason;return false;}
const char* NetplayRuntime::Error()const{
    if(!error_.empty())return error_.c_str();
    // A read-only consumer owns its buffered tail. Wire EOF is reported only
    // after those exact frames are consumed, unless native gameplay finished.
    if(spectator_||retired_)return "";
    if(calibration_.Failed())return calibration_.Error();
    if(channel_.Error()!=Netplay::SessionChannel::Failure::None)return channel_.ErrorText();
    if(transport_.Failed())return transport_.LastError().c_str();return "";
}
bool NetplayRuntime::Configure(const SessionSetup& setup,unsigned delay,bool read_only){
    if(!setup.valid()||delay>9)return Fail("Invalid TH12 multiplayer setup");
    Netplay::SessionConfig gate;gate.sessionId=setup.session_id;gate.seed=setup.seed;gate.gameplayAbi=GameplayAbi(setup);
    gate.gameId=12;gate.playerCount=std::uint8_t(setup.player_count);gate.localPlayer=std::uint8_t(setup.local_player);
    Netplay::CoreConfig core;core.sessionId=setup.session_id;core.playerCount=gate.playerCount;core.localPlayer=gate.localPlayer;
    core.inputDelay=std::uint8_t(delay);core.allowPrediction=!read_only&&setup.mode!=1;
    core.maxRollbackFrames=SessionSetup::MaxRollbackFrames;
    // TH12 shot=1, focus=8, directions=0xf0. Held controls may lead to
    // rewindable rescue/transfer results; Bomb/C/menu/Pause/Restart are exact.
    core.predictableButtons=0xf9;core.directionButtons=0xf0;core.maxDirectionPredictionFrames=3;
    core.directTouchIsAbsolute=true;
    if(!gate_.Reset(gate)||!core_.Reset(core))return Fail("TH12 input core rejected setup");
    setup_=setup;configured_=true;retired_=world_ready_=false;
    next_=next_capture_=0;correction_end_=commit_limit_=Netplay::INVALID_FRAME;
    capture_clock_.Reset();capture_now_us_=0;capture_ui_=false;
    pacing_statistics_={{1,8,0,0,0,1,1,0}};return true;
}
void NetplayRuntime::Clear(){
    capture_clock_.Reset();capture_now_us_=0;capture_ui_=false;
    transport_.Close();channel_.Clear();calibration_.Clear();gate_.Clear();core_.Clear();
    configured_=network_=playback_=spectator_=retired_=false;timing_ready_=timing_sent_=publish_failed_=spectator_publisher_started_=false;
    setup_={};base_session_=now_ms_=initial_wait_=spectator_deadline_=0;generation_=publish_frame_=receive_frame_=0;
    phase_debt_ms_=0;error_.clear();spectator_frames_.clear();publish_tail_.Clear();status_.fill(0);
    spectator_stage_playback_.Clear();spectator_stage_view_.Clear();spectator_stage_publisher_={};
    spectator_recorded_frames_={};
    world_ready_=false;next_=next_capture_=0;correction_end_=commit_limit_=Netplay::INVALID_FRAME;
}
bool NetplayRuntime::Reset(const SessionSetup& setup){Clear();base_session_=setup.session_id;return Configure(setup,setup.input_delay);}
bool NetplayRuntime::Connect(const char* url){
    if(!configured_||network_||ReadOnly()||!url||!*url)return Fail("Invalid TH12 player connection");
    // The initial native stage can spend several frames in a transition.
    // Do not create a synthetic frame-zero checkpoint there; publication is
    // initialized lazily from the first fully constructed stage world.
    spectator_stage_publisher_={};spectator_publisher_started_=false;publish_failed_=false;
    if(!transport_.Connect(url,std::uint8_t(setup_.local_player),std::uint8_t(setup_.player_count),true))return Fail("TH12 transport could not connect");
    transport_.BeginSpectatorSession(setup_.session_id,setup_.seed);
    now_ms_=clock_us()/1000;network_=true;
    if(setup_.mode==0){
        if(!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(0)))return Fail("TH12 direct session bootstrap failed");
        timing_ready_=true;
    }else calibration_.Prepare(gate_.Config(),Mode(),setup_.automatic,setup_.requested_delay,setup_.prediction_reserve);
    return true;
}
bool NetplayRuntime::ConnectSpectator(const char* url,const char* id){
    if(!configured_||network_||!url||!*url||!id||!*id)return Fail("Invalid TH12 spectator admission");
    if(!Configure(setup_,0,true))return false;
    if(!transport_.ConnectSpectator(url,id,std::uint8_t(setup_.player_count),true))return Fail("TH12 spectator could not connect");
    spectator_stage_playback_.Clear();spectator_stage_view_.Clear();spectator_frames_.clear();
    spectator_=network_=true;timing_ready_=setup_.mode==0;now_ms_=clock_us()/1000;spectator_deadline_=now_ms_+90'000;return true;
}
bool NetplayRuntime::BeginPlayback(const SessionSetup& setup){Clear();if(!Configure(setup,0,true))return false;playback_=timing_ready_=true;return true;}
bool NetplayRuntime::CanStart()const{return configured_&&error_.empty()&&(playback_||(spectator_?timing_ready_:!calibration_.Waiting()&&gate_.CanStart()));}
bool NetplayRuntime::NeedsCapture()const{return CanStart()&&!ReadOnly()&&!retired_&&!Correcting()&&
    capture_clock_.CanCapture(next_capture_,next_,setup_.mode==1||capture_ui_,capture_now_us_)&&!core_.HasLocalCapture(next_capture_);}
bool NetplayRuntime::Capture(std::uint32_t frame,const Netplay::FrameInput& input){
    if(!NeedsCapture()||frame!=next_capture_||!valid_input(input)||!core_.ScheduleLocalInput(frame,input))return Fail("Invalid or repeated TH12 physical capture");
    ++next_capture_;
    capture_clock_.Captured(capture_now_us_);
    return !network_||channel_.LocalCaptured(core_,frame,now_ms_)||Fail("TH12 input send failed");
}
bool NetplayRuntime::InitialInputsReady()const{
    if(ReadOnly()||!network_||NextFrame()!=0)return true;
    if(!core_.HasLocalCapture(0))return false;
    for(unsigned s=0;s<setup_.player_count;++s){const auto f=core_.ConfirmedThrough(std::uint8_t(s));if(f==Netplay::INVALID_FRAME||f<setup_.input_delay)return false;}return true;
}
bool NetplayRuntime::Pump(bool ready,bool capture_follows){
    if(!network_)return error_.empty();now_ms_=clock_us()/1000;
    if(spectator_)return retired_||DrainSpectator();
    DrainPublicationTail();
    if(retired_&&(transport_.Failed()||transport_.Disconnected()))return true;
    if(transport_.Disconnected())return Fail(transport_.LastError().empty()?"TH12 input connection ended":transport_.LastError().c_str());
    if(calibration_.Waiting()&&channel_.Retiring()&&!channel_.Active()&&!channel_.PumpRetirement(now_ms_))return Fail(channel_.ErrorText());
    if(!calibration_.Pump(clock_us(),ready))return Fail(calibration_.Error());
    if(calibration_.NeedsApply()){
        const auto choice=calibration_.Startup().Selected();
        if(setup_.mode==0||(setup_.mode==1&&choice.prediction)||choice.prediction>2||choice.delay>9||NextFrame()!=0||next_capture_)return Fail("TH12 received an invalid calibration result");
        auto setup=setup_;setup.input_delay=choice.delay;setup.measured_prediction=choice.prediction;
        if(!Configure(setup,choice.delay)||!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(setup.mode,choice.prediction)))return Fail("TH12 calibration commit failed");
        calibration_.Applied();timing_ready_=true;
    }
    if(calibration_.Waiting())return true;
    const bool active=ready&&InitialInputsReady()&&!retired_;
    const bool defer=capture_follows&&active&&setup_.mode==0&&NeedsCapture();
    if(!channel_.Pump(gate_,core_,now_ms_,active,defer))return Fail(channel_.ErrorText());
    if(ready&&CanStart()&&!InitialInputsReady()){
        if(!initial_wait_)initial_wait_=now_ms_;
        if(now_ms_-initial_wait_>=45'000)return Fail("Timed out waiting for TH12 frame-zero inputs");
    }else initial_wait_=0;
    return error_.empty();
}
bool NetplayRuntime::FlushDeferredInputs(){
    return !network_||ReadOnly()||channel_.FlushDeferredInputs(core_,now_ms_)||Fail(channel_.ErrorText());
}
Netplay::FrameDecision NetplayRuntime::Prepare(std::uint32_t frame)const{
    if(!CanStart()||!InitialInputsReady()||retired_||frame!=next_||frame==Netplay::INVALID_FRAME||core_.HasRollbackRequest())return {};
    const auto confirmed=Confirmed();
    if(AllowsRollback()&&!world_ready_&&(confirmed==Netplay::INVALID_FRAME||frame>confirmed))return {};
    const auto decision=core_.PrepareFrame(frame);
    if(decision.canAdvance)for(unsigned s=0;s<setup_.player_count;++s)if(!valid_input(decision.inputs[s]))return {};
    return decision;
}
bool NetplayRuntime::MarkSimulated(std::uint32_t frame,const Netplay::FrameDecision& d){
    const auto expected=Prepare(frame);
    if(!expected.canAdvance||!d.canAdvance||d.predictedMask!=expected.predictedMask||d.inputs!=expected.inputs||
       !core_.MarkSimulated(frame,d))return Fail("TH12 logical frame decision changed before commit");
    ++next_;
    // Legacy exact playback and delay paths have no title prediction history.
    // Rollback-capable drivers grant their own post-retirement output limit.
    if(!AllowsRollback()&&!SetCommitLimit(frame))return Fail("TH12 exact output frontier rejected");
    return true;
}
bool NetplayRuntime::ConfirmedInputs(std::uint32_t f,std::array<Netplay::FrameInput,3>& out)const{
    return CanStart()&&!Correcting()&&!core_.HasRollbackRequest()&&commit_limit_!=Netplay::INVALID_FRAME&&f<=commit_limit_&&core_.ConfirmedInputs(f,&out);
}
bool NetplayRuntime::InputsReconciledBeforeNext()const{
    const auto c=Confirmed();return CanStart()&&!core_.HasRollbackRequest()&&(next_==0||(c!=Netplay::INVALID_FRAME&&c>=next_-1));
}
bool NetplayRuntime::SetWorldReady(bool ready){
    if(!configured_||Correcting()||core_.HasRollbackRequest()||(ready&&!AllowsRollback())||(!ready&&!InputsReconciledBeforeNext()))return false;
    world_ready_=ready;return true;
}
bool NetplayRuntime::BeginCorrection(std::uint32_t first){
    if(!AllowsRollback()||!CanStart()||!world_ready_||Correcting()||first!=core_.RollbackFrame()||first>=next_||
       next_-first>setup_.prediction_limit||(commit_limit_!=Netplay::INVALID_FRAME&&first<=commit_limit_))return false;
    const auto end=next_;if(!core_.RewindSimulationTo(first))return false;
    correction_end_=end;next_=first;return true;
}
bool NetplayRuntime::RestartCorrection(std::uint32_t first){
    if(!AllowsRollback()||!CanStart()||!world_ready_||!Correcting()||first!=core_.RollbackFrame()||first>=next_||
       next_>correction_end_||correction_end_-first>setup_.prediction_limit||
       (commit_limit_!=Netplay::INVALID_FRAME&&first<=commit_limit_))return false;
    if(!core_.RewindSimulationTo(first))return false;next_=first;return true;
}
bool NetplayRuntime::EndCorrection(bool lifecycleBoundary){
    if(!Correcting()||core_.HasRollbackRequest()||next_>correction_end_||
       (lifecycleBoundary?!InputsReconciledBeforeNext():next_!=correction_end_))return false;
    correction_end_=Netplay::INVALID_FRAME;return true;
}
bool NetplayRuntime::SetCommitLimit(std::uint32_t through){
    const auto last=core_.LastSimulatedFrame();
    if(!CanStart()||Correcting()||core_.HasRollbackRequest()||through==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME||through>last||
       (commit_limit_!=Netplay::INVALID_FRAME&&through<commit_limit_))return false;
    std::array<Netplay::FrameInput,3> exact;
    if(!core_.ConfirmedInputs(through,&exact))return false;
    commit_limit_=through;return true;
}
bool NetplayRuntime::CommitReconciledPrefix(std::uint32_t through){
    if(!Correcting())return SetCommitLimit(through);
    // The caller has completed Update, semantic Draw and all owner seals.
    // A contiguous all-peer actual prefix cannot acquire a new rollback
    // request; conflicting replacements of actual input remain fatal.
    if(!AllowsRollback()||!InputsReconciledBeforeNext()||!next_||
       next_>correction_end_||through!=next_-1||through!=core_.LastSimulatedFrame()||
       (commit_limit_!=Netplay::INVALID_FRAME&&through<commit_limit_))return false;
    std::array<Netplay::FrameInput,3> inputs;
    if(!core_.ConfirmedInputs(through,&inputs))return false;
    commit_limit_=through;return true;
}
bool NetplayRuntime::CommittedPrefixInputs(std::uint32_t frame,std::array<Netplay::FrameInput,3>& out)const{
    return CanStart()&&!core_.HasRollbackRequest()&&commit_limit_!=Netplay::INVALID_FRAME&&
        frame<=commit_limit_&&core_.ConfirmedInputs(frame,&out);
}
bool NetplayRuntime::CanReplayConfirmed()const{
    if(!Correcting()||next_>=correction_end_||!InputsReconciledBeforeNext())return false;
    const auto decision=Prepare(next_);
    return decision.canAdvance&&!decision.predictedMask;
}
Netplay::RemoteInputResult NetplayRuntime::SubmitRemote(unsigned seat,std::uint32_t frame,const Netplay::FrameInput& input){
    const auto first=RemoteInputFrontier();
    if(!CanStart()||ReadOnly()||seat>=setup_.player_count||!valid_input(input)||frame==Netplay::INVALID_FRAME||
       (frame>=first&&frame-first>=Netplay::INPUT_HISTORY_SIZE-Netplay::MAX_REDUNDANT_INPUTS))return Netplay::RemoteInputResult::InvalidPlayer;
    return core_.SubmitRemoteInput(std::uint8_t(seat),frame,input);
}
bool NetplayRuntime::Feed(std::uint32_t frame,const Netplay::FrameInput* inputs,unsigned count){
    if(frame!=NextFrame()||count!=setup_.player_count||!inputs)return Fail("Invalid TH12 recorded frame");
    for(unsigned s=0;s<count;++s)if(!valid_input(inputs[s]))return Fail("Invalid TH12 recorded input");
    if(!core_.ScheduleLocalInput(frame,inputs[setup_.local_player]))return Fail("TH12 recorded local lane rejected");
    for(unsigned s=0;s<count;++s)if(s!=setup_.local_player){const auto r=core_.SubmitRemoteInput(std::uint8_t(s),frame,inputs[s]);if(r!=Netplay::RemoteInputResult::Accepted&&r!=Netplay::RemoteInputResult::Duplicate)return Fail("TH12 recorded remote lane rejected");}
    return true;
}
bool NetplayRuntime::FeedPlayback(std::uint32_t f,const Netplay::FrameInput* inputs,unsigned count){return playback_&&Feed(f,inputs,count);}
bool NetplayRuntime::DrainSpectator(){
    std::uint64_t session;std::uint32_t seed;
    if(transport_.TakeSpectatorSession(&session,&seed)){
        auto setup=setup_;setup.session_id=session;setup.seed=seed;
        if(!Configure(setup,0,true))return false;
        spectator_frames_.clear();spectator_stage_playback_.Clear();
        receive_frame_=0;timing_ready_=setup_.mode==0;spectator_deadline_=now_ms_+90'000;
    }
    std::vector<std::uint8_t> bytes;
    for(unsigned n=0;n<128&&transport_.Poll(&bytes);++n){
        if(Netplay::IsSpectatorStageEvent(bytes.data(),bytes.size())){
            if(!timing_ready_||!ReceiveSpectatorWire(bytes.data(),bytes.size()))return false;
            spectator_deadline_=now_ms_+15'000;continue;
        }
        if(Netplay::AdonisSpectatorTiming::IsPacket(bytes.data(),bytes.size())){
            Netplay::AdonisSpectatorTiming timing;
            if(timing_ready_||receive_frame_||!Netplay::AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),timing)||
               timing.game!=12||!setup_.mode||timing.mode!=setup_.mode||(setup_.mode==1&&timing.prediction)||timing.sessionId!=setup_.session_id||timing.automatic!=setup_.automatic)return Fail("Invalid TH12 spectator timing");
            auto setup=setup_;setup.input_delay=timing.delay;setup.measured_prediction=timing.prediction;
            if(GameplayAbi(setup)!=timing.gameplayAbi||!Configure(setup,0,true))return Fail("TH12 spectator contract mismatch");
            timing_ready_=true;spectator_deadline_=now_ms_+15'000;continue;
        }
        Netplay::SpectatorFramePacket packet;
        if(!timing_ready_||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
           packet.sessionId!=setup_.session_id||packet.gameplayAbi!=GameplayAbi(setup_)||packet.playerCount!=setup_.player_count||
           (spectator_stage_playback_.History().Segments().empty()&&packet.frame!=receive_frame_))
            return Fail("Invalid TH12 spectator input stream");
        if(spectator_stage_playback_.History().Segments().empty()){
            if(spectator_frames_.size()>=8192)return Fail("TH12 spectator input buffer limit");
            spectator_frames_.push_back(packet);++receive_frame_;
        }else if(!spectator_stage_playback_.Receive(packet))return Fail("Invalid TH12 spectator stage input");
        spectator_deadline_=now_ms_+15'000;
    }
    if(spectator_frames_.empty()){
        if(transport_.Failed())return Fail(transport_.LastError().c_str());
        // A quiet confirmed frontier is not a transport failure: the source
        // may be paused while the read-only viewer remains live-following.
        // Keep the bounded admission/bootstrap deadline until Ready, then use
        // transport health/closure as the liveness signal.
        if(!spectator_stage_playback_.Ready()&&now_ms_>=spectator_deadline_)
            return Fail("TH12 spectator stream ended or stalled; players are unaffected");
    }return true;
}
bool NetplayRuntime::FeedSpectator(std::uint32_t frame){
    if(!spectator_||!timing_ready_||spectator_frames_.empty())return false;
    const auto& packet=spectator_frames_.front();if(packet.frame!=frame||!Feed(frame,packet.inputs.data(),packet.playerCount))return false;
    spectator_frames_.pop_front();return true;
}
bool NetplayRuntime::ReceiveSpectatorWire(const std::uint8_t* bytes,std::size_t size){
    Netplay::SpectatorStageEvent event{};
    if(!Netplay::DecodeSpectatorStageEvent(bytes,size,&event)||event.game!='C'||
       event.sessionId!=setup_.session_id||event.gameplayAbi!=GameplayAbi(setup_)||
       event.playerCount!=setup_.player_count||
       (event.kind==Netplay::SpectatorEventKind::Checkpoint&&event.schema!=SpectatorStageSchema)||
       !spectator_stage_playback_.Receive(event))return Fail("Invalid TH12 spectator stage stream");
    return true;
}
bool NetplayRuntime::SpectatorCheckpointRequired(std::uint32_t frame,std::uint32_t stage)const{
    if(!network_||spectator_||playback_||frame==Netplay::INVALID_FRAME)return false;
    if(!frame)return true;
    const auto& previous=spectator_recorded_frames_[(frame-1)%spectator_recorded_frames_.size()];
    return previous.frame!=frame-1||previous.stage!=stage;
}
bool NetplayRuntime::RecordSpectatorFrame(std::uint32_t frame,std::uint32_t stage,
        const std::vector<std::uint8_t>& initializer,
        const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){
    if(!network_||spectator_||playback_||setup_.local_player)return true;
    if(!spectator_publisher_started_){
        // Before the first stable checkpoint the title may still be creating
        // the stage's native graph. Those frames are deliberately outside the
        // spectator stream; the first real initializer defines its origin.
        if(initializer.empty())return true;
        if(initializer.size()>Netplay::SpectatorStageMaxStateBytes||
           !spectator_stage_publisher_.Reset('C',std::uint8_t(setup_.player_count),setup_.session_id,GameplayAbi(setup_),frame)){
            publish_failed_=true;transport_.StopSpectators();return true;
        }
        spectator_publisher_started_=true;
    }
    const bool checkpoint=SpectatorCheckpointRequired(frame,stage);
    if(checkpoint!=!initializer.empty()||initializer.size()>Netplay::SpectatorStageMaxStateBytes){
        publish_failed_=true;transport_.StopSpectators();return true;
    }
    Netplay::SpectatorStagePublisher::Frame record;record.frame=frame;record.stage=stage;
    record.schema=initializer.empty()?0:SpectatorStageSchema;record.initializer=initializer;record.inputs=inputs;record.hasInputs=true;
    if(!spectator_stage_publisher_.Record(std::move(record))){publish_failed_=true;transport_.StopSpectators();return true;}
    spectator_recorded_frames_[frame%spectator_recorded_frames_.size()]={frame,stage};return true;
}
void NetplayRuntime::PublishConfirmedSpectatorFrames(){
    if(!network_||ReadOnly()||retired_||setup_.local_player||!spectator_publisher_started_||publish_failed_||
       Correcting()||core_.HasRollbackRequest())return;
    const auto confirmed=core_.ConfirmedThroughAllRemotes(),last=core_.LastSimulatedFrame();
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return;
    const auto through=std::min(confirmed,last);
    if(transport_.SpectatorState()<0){publish_failed_=true;transport_.StopSpectators();return;}
    if(setup_.mode&&!timing_sent_){
        auto timing=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
        if(timing.empty()){publish_failed_=true;transport_.StopSpectators();return;}
        if(!transport_.SendSpectator(timing.data(),timing.size()))return;timing_sent_=true;
    }
    const auto publish_through=std::min(through,spectator_stage_publisher_.Published()+31);
    Netplay::SpectatorStagePublisher::PublishFailure failure{};
    const bool ok=spectator_stage_publisher_.Publish(publish_through,last,
        [&](std::uint32_t frame,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){return ConfirmedInputs(frame,inputs);},
        [&](const std::vector<std::uint8_t>& bytes){return transport_.SendSpectator(bytes.data(),bytes.size());},&failure);
    if(!ok&&(!Netplay::SpectatorStagePublisher::Retryable(failure)||transport_.SpectatorState()<0)){
        publish_failed_=true;transport_.StopSpectators();return;
    }
    publish_frame_=spectator_stage_publisher_.Published();
}
void NetplayRuntime::FreezePublicationTail(bool restarting){
    if(ReadOnly()||setup_.local_player||publish_failed_||!spectator_publisher_started_)return;
    std::vector<std::uint8_t> timing;
    if(setup_.mode&&!timing_sent_){
        timing=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
        if(timing.empty()){publish_failed_=true;transport_.StopSpectators();return;}
    }
    if(!publish_tail_.Freeze(spectator_stage_publisher_,core_.ConfirmedThroughAllRemotes(),core_.LastSimulatedFrame(),
        [&](std::uint32_t frame,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){return ConfirmedInputs(frame,inputs);},now_ms_,timing,!restarting)){
        publish_failed_=true;transport_.StopSpectators();return;
    }
    if(setup_.mode)timing_sent_=true;
    publish_frame_=spectator_stage_publisher_.Published();DrainPublicationTail();
}
void NetplayRuntime::DrainPublicationTail(){
    using Result=Netplay::SpectatorPublicationTail::Result;
    const auto result=publish_tail_.Drain(now_ms_,transport_.SpectatorState()<0,
        [&](const auto& bytes){return transport_.SendSpectator(bytes.data(),bytes.size());},clock_us);
    if(result==Result::Complete)transport_.StopSpectators(true);
    if(result==Result::Failed){publish_failed_=true;transport_.StopSpectators();}
}
bool NetplayRuntime::Finish(bool restarting){
    if(retired_)return true;
    if(spectator_){
        const auto& playback=spectator_stage_playback_;
        if(!playback.Ready()||!playback.History().Ended()||playback.Paused()||playback.Seeking()||playback.Position()<playback.History().Received())return false;
        retired_=true;return true;
    }
    const auto last=core_.LastSimulatedFrame();if(last==Netplay::INVALID_FRAME)return false;
    if(Correcting()||core_.HasRollbackRequest()||!InputsReconciledBeforeNext()||commit_limit_!=last)return false;
    if(ReadOnly()){retired_=true;return true;}
    if(network_){if(!channel_.FlushRetirementFence(core_,last,now_ms_))return Fail("TH12 retirement input flush failed");
        if(!channel_.CanRetire(core_,last))return false;
        if(!channel_.Retire(core_,last,now_ms_))return Fail("TH12 retirement failed");}
    PublishConfirmedSpectatorFrames();FreezePublicationTail(restarting);
    retired_=true;return true;
}
bool NetplayRuntime::BeginNextRun(std::uint32_t seed){
    if(!retired_||ReadOnly()||seed>65535||generation_==0xffffffffu)return Fail("Invalid TH12 next generation");
    const auto previous=gate_.Config();
    auto setup=setup_;setup.seed=seed;setup.input_delay=setup.requested_delay;setup.measured_prediction=0;setup.session_id=base_session_^(std::uint64_t(generation_+1)*0x9e3779b97f4a7c15ull);
    if(!Configure(setup,setup.input_delay))return false;++generation_;phase_debt_ms_=0;initial_wait_=0;timing_ready_=timing_sent_=false;publish_frame_=0;
    // A retry is a new spectator identity. Retire every stage-history object
    // from the previous run before the new native world is initialized.
    spectator_stage_publisher_={};spectator_publisher_started_=false;spectator_stage_playback_.Clear();spectator_stage_view_.Clear();
    spectator_recorded_frames_={};spectator_frames_.clear();
    publish_tail_.Clear();publish_failed_=false;
    transport_.BeginSpectatorSession(setup_.session_id,setup_.seed);
    if(network_){
        if(!setup_.mode){if(!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(0)))return Fail("TH12 next direct session failed");timing_ready_=true;}
        else calibration_.Prepare(gate_.Config(),Mode(),setup.automatic,setup.requested_delay,setup.prediction_reserve,&previous);
    }return true;
}
double NetplayRuntime::PacedElapsed(double elapsed){
    if(!std::isfinite(elapsed)||elapsed<0)return 0;if(!network_||ReadOnly())return elapsed;
    if(!setup_.mode){
        // Peer lead uses the shared bounded time sync (0.98..1.02). Missing
        // inputs consume the rollback window, never an extra half-speed clock.
        const auto scale=channel_.IntervalScale();const auto credited=elapsed/scale;
        pacing_statistics_[2]+=elapsed;pacing_statistics_[3]+=credited;
        pacing_statistics_[4]+=credited;
        pacing_statistics_[5]=1;pacing_statistics_[6]=scale;++pacing_statistics_[7];
        return credited;
    }
    phase_debt_ms_+=channel_.TakeAdonisDelayMs();const double used=std::min(elapsed,phase_debt_ms_);phase_debt_ms_-=used;return elapsed-used;
}
const std::uint32_t* NetplayRuntime::Status(){
    status_.fill(0);status_[0]=1;status_[1]=configured_;status_[2]=CanStart();status_[3]=NextFrame();status_[4]=Confirmed();
    status_[5]=setup_.player_count;status_[6]=setup_.local_player;status_[7]=setup_.input_delay;status_[8]=PredictionLimit();status_[9]=network_;
    status_[10]=spectator_;status_[11]=playback_;status_[12]=generation_;status_[13]=retired_;status_[14]=unsigned(spectator_?spectator_frames_.size():publish_tail_.Size());
    status_[15]=channel_.PacketsSent();status_[16]=channel_.PacketsReceived();status_[17]=channel_.RepairsSent();status_[18]=channel_.PacketsIgnored();
    status_[19]=transport_.IsOpen();const std::string mode=transport_.Mode();status_[20]=mode=="rtc"?1:mode=="relay"?2:spectator_?3:0;
    status_[21]=InitialInputsReady();status_[22]=publish_failed_;status_[23]=*Error()!=0;return status_.data();
}
}
