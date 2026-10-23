#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#endif
namespace th15::multiplayer {
namespace {
std::uint64_t clock_us(){
#ifdef __EMSCRIPTEN__
    return std::uint64_t(emscripten_get_now()*1000);
#else
    return std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
Netplay::SessionChannelConfig channel_policy(unsigned mode,unsigned prediction=0){
    Netplay::SessionChannelConfig p;p.adonisPhase=mode!=0;p.adonisPredictionFrames=p.adonisPhase?prediction:0;p.inputResendMs=16;return p;
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
    if(!setup.valid()||delay>9)return Fail("Invalid TH15 multiplayer setup");
    Netplay::SessionConfig gate;gate.sessionId=setup.session_id;gate.seed=setup.seed;gate.gameplayAbi=GameplayAbi(setup);
    gate.gameId=15;gate.playerCount=std::uint8_t(setup.player_count);gate.localPlayer=std::uint8_t(setup.local_player);
    Netplay::CoreConfig core;core.sessionId=setup.session_id;core.playerCount=gate.playerCount;core.localPlayer=gate.localPlayer;
    core.inputDelay=std::uint8_t(delay);core.allowPrediction=!read_only&&setup.mode!=1;
    core.maxRollbackFrames=SessionSetup::MaxRollbackFrames;
    // Shot, focus and short direction holds may be predicted. Bomb, pause,
    // transfer and menu actions are supplied only by actual remote input.
    core.predictableButtons=0xf9;core.directionButtons=0xf0;core.maxDirectionPredictionFrames=3;core.directTouchIsAbsolute=true;
    if(!gate_.Reset(gate)||!core_.Reset(core))return Fail("TH15 input core rejected setup");
    setup_=setup;configured_=true;retired_=world_ready_=false;
    next_=0;correction_end_=commit_limit_=Netplay::INVALID_FRAME;
    next_capture_=0;capture_clock_.Reset();capture_now_us_=0;capture_ui_=false;return true;
}
void NetplayRuntime::Clear(){
    stage_publisher_={};stage_playback_.Clear();stage_view_.Clear();stage_publication_=stage_received_=false;
    next_capture_=0;capture_clock_.Reset();capture_now_us_=0;capture_ui_=false;
    transport_.Close();channel_.Clear();calibration_.Clear();gate_.Clear();core_.Clear();
    configured_=network_=playback_=spectator_=retired_=false;timing_ready_=timing_sent_=publish_failed_=false;
    setup_={};base_session_=now_ms_=initial_wait_=spectator_deadline_=publish_deadline_=0;generation_=publish_frame_=receive_frame_=0;
    phase_debt_ms_=0;error_.clear();spectator_frames_.clear();publish_tail_.clear();status_.fill(0);
    world_ready_=false;next_=0;correction_end_=commit_limit_=Netplay::INVALID_FRAME;
}
bool NetplayRuntime::Reset(const SessionSetup& setup){Clear();base_session_=setup.session_id;return Configure(setup,setup.input_delay);}
bool NetplayRuntime::Connect(const char* url){
    if(!configured_||network_||ReadOnly()||!url||!*url)return Fail("Invalid TH15 player connection");
    if(!transport_.Connect(url,std::uint8_t(setup_.local_player),std::uint8_t(setup_.player_count),true))return Fail("TH15 transport could not connect");
    now_ms_=clock_us()/1000;network_=true;
    if(setup_.mode==0){
        if(!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(0)))return Fail("TH15 direct session bootstrap failed");
        timing_ready_=true;
    }else calibration_.Prepare(gate_.Config(),Netplay::AdonisMode(setup_.mode),setup_.automatic,setup_.requested_delay,setup_.prediction_reserve);
    return true;
}
bool NetplayRuntime::ConnectSpectator(const char* url,const char* id){
    if(!configured_||network_||!url||!*url||!id||!*id)return Fail("Invalid TH15 spectator admission");
    if(!Configure(setup_,0,true))return false;
    if(!transport_.ConnectSpectator(url,id,std::uint8_t(setup_.player_count),true))return Fail("TH15 spectator could not connect");
    spectator_=network_=true;timing_ready_=setup_.mode==0;now_ms_=clock_us()/1000;spectator_deadline_=now_ms_+90'000;return true;
}
bool NetplayRuntime::BeginPlayback(const SessionSetup& setup){Clear();if(!Configure(setup,0,true))return false;playback_=timing_ready_=true;return true;}
bool NetplayRuntime::CanStart()const{return configured_&&error_.empty()&&(playback_||(spectator_?timing_ready_:!calibration_.Waiting()&&gate_.CanStart()));}
std::uint32_t NetplayRuntime::NextFrame()const{if(spectator_&&stage_received_)return stage_playback_.Position();return next_;}
bool NetplayRuntime::NeedsCapture()const{return CanStart()&&!ReadOnly()&&!retired_&&!Correcting()&&
    capture_clock_.CanCapture(next_capture_,NextFrame(),capture_ui_,capture_now_us_)&&!core_.HasLocalCapture(next_capture_);}
bool NetplayRuntime::Capture(std::uint32_t frame,const Netplay::FrameInput& input){
    if(!NeedsCapture()||frame!=next_capture_||
       !Netplay::IsValidFrameInput(input)||!core_.ScheduleLocalInput(frame,input))return Fail("Invalid or repeated TH15 physical capture");
    ++next_capture_;capture_clock_.Captured(capture_now_us_);
    return !network_||channel_.LocalCaptured(core_,frame,now_ms_)||Fail("TH15 input send failed");
}
bool NetplayRuntime::InitialInputsReady()const{
    if(ReadOnly()||!network_||NextFrame()!=0)return true;
    if(!core_.HasLocalCapture(0))return false;
    for(unsigned s=0;s<setup_.player_count;++s){const auto f=core_.ConfirmedThrough(std::uint8_t(s));if(f==Netplay::INVALID_FRAME||f<setup_.input_delay)return false;}return true;
}
bool NetplayRuntime::Pump(bool ready){
    if(!network_)return error_.empty();now_ms_=clock_us()/1000;
    if(spectator_)return retired_||DrainSpectator();
    DrainPublicationTail();
    if(retired_&&(transport_.Failed()||transport_.Disconnected()))return true;
    if(transport_.Disconnected())return Fail(transport_.LastError().empty()?"TH15 input connection ended":transport_.LastError().c_str());
    if(calibration_.Waiting()&&channel_.Retiring()&&!channel_.Active()&&!channel_.PumpRetirement(now_ms_))return Fail(channel_.ErrorText());
    if(!calibration_.Pump(clock_us(),ready))return Fail(calibration_.Error());
    if(calibration_.NeedsApply()){
        const auto choice=calibration_.Startup().Selected();
        if(setup_.mode==0||(setup_.mode==1&&choice.prediction)||choice.prediction>2||choice.delay>9||NextFrame()!=0||next_capture_)return Fail("TH15 received an invalid calibration result");
        auto setup=setup_;setup.input_delay=choice.delay;setup.measured_prediction=choice.prediction;
        if(!Configure(setup,choice.delay)||!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(setup.mode,choice.prediction)))return Fail("TH15 calibration commit failed");
        calibration_.Applied();timing_ready_=true;
    }
    if(calibration_.Waiting())return true;
    const bool active=ready&&InitialInputsReady()&&!retired_;
    if(!channel_.Pump(gate_,core_,now_ms_,active))return Fail(channel_.ErrorText());
    if(ready&&CanStart()&&!InitialInputsReady()){
        if(!initial_wait_)initial_wait_=now_ms_;
        if(now_ms_-initial_wait_>=45'000)return Fail("Timed out waiting for TH15 frame-zero inputs");
    }else initial_wait_=0;
    Publish();return error_.empty();
}
Netplay::FrameDecision NetplayRuntime::Prepare(std::uint32_t frame)const{
    if(!CanStart()||!InitialInputsReady()||retired_||frame!=next_||frame==Netplay::INVALID_FRAME||core_.HasRollbackRequest())return {};
    const auto confirmed=Confirmed();
    if(AllowsRollback()&&!world_ready_&&(confirmed==Netplay::INVALID_FRAME||frame>confirmed))return {};
    return core_.PrepareFrame(frame);
}
bool NetplayRuntime::MarkSimulated(std::uint32_t frame,const Netplay::FrameDecision& d){
    const auto expected=Prepare(frame);
    if(!expected.canAdvance||!d.canAdvance||expected.predictedMask!=d.predictedMask||expected.inputs!=d.inputs||
       !core_.MarkSimulated(frame,d))return Fail("TH15 logical frame decision changed before commit");
    ++next_;
    if(!AllowsRollback()&&!SetCommitLimit(frame))return Fail("TH15 exact output frontier rejected");
    Publish();return true;
}
bool NetplayRuntime::ConfirmedInputs(std::uint32_t f,std::array<Netplay::FrameInput,3>& out)const{
    return CanStart()&&!Correcting()&&!core_.HasRollbackRequest()&&commit_limit_!=Netplay::INVALID_FRAME&&f<=commit_limit_&&core_.ConfirmedInputs(f,&out);
}
bool NetplayRuntime::InputsReconciledBeforeNext()const{
    const auto confirmed=Confirmed();return CanStart()&&!core_.HasRollbackRequest()&&(next_==0||(confirmed!=Netplay::INVALID_FRAME&&confirmed>=next_-1));
}
bool NetplayRuntime::SetWorldReady(bool ready){
    if(!configured_||Correcting()||core_.HasRollbackRequest()||(ready&&!AllowsRollback())||(!ready&&!InputsReconciledBeforeNext()))return false;
    world_ready_=ready;return true;
}
bool NetplayRuntime::BeginCorrection(std::uint32_t first){
    if(!AllowsRollback()||!CanStart()||!world_ready_||Correcting()||first!=core_.RollbackFrame()||first>=next_||
       next_-first>SessionSetup::MaxRollbackFrames||(commit_limit_!=Netplay::INVALID_FRAME&&first<=commit_limit_))return false;
    const auto end=next_;if(!core_.RewindSimulationTo(first))return false;
    correction_end_=end;next_=first;return true;
}
bool NetplayRuntime::RestartCorrection(std::uint32_t first){
    if(!AllowsRollback()||!CanStart()||!world_ready_||!Correcting()||first!=core_.RollbackFrame()||first>=next_||
       next_>correction_end_||correction_end_-first>SessionSetup::MaxRollbackFrames||
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
    std::array<Netplay::FrameInput,3> exact;if(!core_.ConfirmedInputs(through,&exact))return false;
    commit_limit_=through;return true;
}
bool NetplayRuntime::CommitReconciledPrefix(std::uint32_t through){
    if(!Correcting())return SetCommitLimit(through);
    if(!AllowsRollback()||!InputsReconciledBeforeNext()||!next_||next_>correction_end_||through!=next_-1||through!=core_.LastSimulatedFrame()||
       (commit_limit_!=Netplay::INVALID_FRAME&&through<commit_limit_))return false;
    std::array<Netplay::FrameInput,3> exact;if(!core_.ConfirmedInputs(through,&exact))return false;
    commit_limit_=through;return true;
}
bool NetplayRuntime::CommittedPrefixInputs(std::uint32_t frame,std::array<Netplay::FrameInput,3>& out)const{
    return CanStart()&&!core_.HasRollbackRequest()&&commit_limit_!=Netplay::INVALID_FRAME&&frame<=commit_limit_&&core_.ConfirmedInputs(frame,&out);
}
bool NetplayRuntime::Feed(std::uint32_t frame,const Netplay::FrameInput* inputs,unsigned count){
    if(frame!=NextFrame()||count!=setup_.player_count||!inputs)return Fail("Invalid TH15 recorded frame");
    for(unsigned s=0;s<count;++s)if(!Netplay::IsValidFrameInput(inputs[s]))return Fail("Invalid TH15 recorded input");
    if(!core_.ScheduleLocalInput(frame,inputs[setup_.local_player]))return Fail("TH15 recorded local lane rejected");
    for(unsigned s=0;s<count;++s)if(s!=setup_.local_player){const auto r=core_.SubmitRemoteInput(std::uint8_t(s),frame,inputs[s]);if(r!=Netplay::RemoteInputResult::Accepted&&r!=Netplay::RemoteInputResult::Duplicate)return Fail("TH15 recorded remote lane rejected");}
    return true;
}
bool NetplayRuntime::FeedPlayback(std::uint32_t f,const Netplay::FrameInput* inputs,unsigned count){return playback_&&Feed(f,inputs,count);}
bool NetplayRuntime::DrainSpectator(){
    std::vector<std::uint8_t> bytes;
    for(unsigned n=0;n<128&&transport_.Poll(&bytes);++n){
        if(Netplay::AdonisSpectatorTiming::IsPacket(bytes.data(),bytes.size())){
            Netplay::AdonisSpectatorTiming timing;
            if(timing_ready_||receive_frame_||!Netplay::AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),timing)||
               timing.game!=15||timing.mode!=setup_.mode||(setup_.mode==1&&timing.prediction)||timing.prediction>2||timing.sessionId!=setup_.session_id||timing.automatic!=setup_.automatic)return Fail("Invalid TH15 spectator timing");
            auto setup=setup_;setup.input_delay=timing.delay;setup.measured_prediction=timing.prediction;
            if(GameplayAbi(setup)!=timing.gameplayAbi||!Configure(setup,0,true))return Fail("TH15 spectator contract mismatch");
            timing_ready_=true;spectator_deadline_=now_ms_+15'000;continue;
        }
        if(Netplay::IsSpectatorStageEvent(bytes.data(),bytes.size())){
            Netplay::SpectatorStageEvent event;
            if(!timing_ready_||!Netplay::DecodeSpectatorStageEvent(bytes.data(),bytes.size(),&event)||
               event.game!='F'||event.sessionId!=setup_.session_id||event.gameplayAbi!=GameplayAbi(setup_)||
               event.playerCount!=setup_.player_count||!stage_playback_.Receive(event))return Fail("Invalid TH15 spectator stage event");
            stage_received_=true;spectator_frames_.clear();spectator_deadline_=now_ms_+15'000;continue;
        }
        if(stage_received_){
            Netplay::SpectatorFramePacket packet;
            if(!timing_ready_||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
               !stage_playback_.Receive(packet))return Fail("Invalid TH15 spectator stage inputs");
            spectator_deadline_=now_ms_+15'000;continue;
        }
        Netplay::SpectatorFramePacket packet;
        if(!timing_ready_||spectator_frames_.size()>=8192||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
           packet.sessionId!=setup_.session_id||packet.gameplayAbi!=GameplayAbi(setup_)||packet.playerCount!=setup_.player_count||packet.frame!=receive_frame_)
            return Fail("Invalid TH15 spectator input stream");
        spectator_frames_.push_back(packet);++receive_frame_;spectator_deadline_=now_ms_+15'000;
    }
    if(stage_received_){
        if(stage_playback_.History().Ended())return true;
        if(transport_.Failed())return Fail(transport_.LastError().c_str());
        // A quiet confirmed frontier can mean the source deliberately
        // paused. Keep admission/bootstrap bounded, then use transport
        // failure or closure to detect a dead relay instead of game-frame
        // silence.
        if(!stage_playback_.Ready()&&now_ms_>=spectator_deadline_)
            return Fail("TH15 spectator stage stream stalled; players are unaffected");
        return true;
    }
    if(spectator_frames_.empty()){
        if(transport_.Failed())return Fail(transport_.LastError().c_str());
        if(now_ms_>=spectator_deadline_)return Fail("TH15 spectator stream ended or stalled; players are unaffected");
    }return true;
}
bool NetplayRuntime::FeedSpectator(std::uint32_t frame){
    if(!spectator_||!timing_ready_||spectator_frames_.empty())return false;
    const auto& packet=spectator_frames_.front();if(packet.frame!=frame||!Feed(frame,packet.inputs.data(),packet.playerCount))return false;
    spectator_frames_.pop_front();return true;
}
void NetplayRuntime::RecordSpectatorFrame(Netplay::SpectatorStagePublisher::Frame frame){
    if(ReadOnly()||setup_.local_player||publish_failed_)return;
    if(!stage_publication_){
        if(frame.initializer.empty())return;
        if(!stage_publisher_.Reset('F',std::uint8_t(setup_.player_count),setup_.session_id,GameplayAbi(setup_),frame.frame))return;
        stage_publication_=true;
    }
    const auto recorded_frame=frame.frame;
    if(!stage_publisher_.Record(std::move(frame))){std::fprintf(stderr,"TH15 spectator journal rejected frame %u after %u\n",recorded_frame,stage_publisher_.Published());publish_failed_=true;transport_.StopSpectators();}
}
void NetplayRuntime::Publish(){
    if(Correcting()||core_.HasRollbackRequest()||commit_limit_==Netplay::INVALID_FRAME)return;
    if(stage_publication_){
        if(!network_||ReadOnly()||setup_.local_player||publish_failed_)return;
        if(transport_.SpectatorState()<0){publish_failed_=true;transport_.StopSpectators();return;}
        if(!timing_sent_&&setup_.mode!=0){
            auto timing=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
            if(timing.empty()){std::fprintf(stderr,"TH15 spectator timing could not be encoded\n");publish_failed_=true;transport_.StopSpectators();return;}
            if(!transport_.SendSpectator(timing.data(),timing.size()))return;timing_sent_=true;
        }
        const auto begin=clock_us();
        stage_publisher_.Publish(core_.ConfirmedThroughAllRemotes(),commit_limit_,
          [&](auto frame,auto& inputs){return core_.ConfirmedInputs(frame,&inputs);},
          [&](const auto& wire){return clock_us()-begin<2000&&transport_.SendSpectator(wire.data(),wire.size());});
        return;
    }
    if(!network_||ReadOnly()||generation_||setup_.local_player||publish_failed_||!transport_.HasSpectators())return;
    const auto through=commit_limit_;
    if(transport_.SpectatorState()<0||(through>=publish_frame_&&through-publish_frame_>=Netplay::INPUT_HISTORY_SIZE)){publish_failed_=true;transport_.StopSpectators();return;}
    if(!timing_sent_&&setup_.mode!=0){
        auto bytes=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
        if(bytes.empty()){std::fprintf(stderr,"TH15 legacy spectator timing could not be encoded\n");publish_failed_=true;transport_.StopSpectators();return;}
        if(!transport_.SendSpectator(bytes.data(),bytes.size()))return;timing_sent_=true;
    }
    const auto start=clock_us();
    for(unsigned n=0;publish_frame_<=through&&n<32;++n){
        if(n&&clock_us()-start>=2000)break;
        Netplay::SpectatorFramePacket packet;packet.sessionId=setup_.session_id;packet.frame=publish_frame_;packet.gameplayAbi=GameplayAbi(setup_);packet.playerCount=std::uint8_t(setup_.player_count);
        if(!core_.ConfirmedInputs(publish_frame_,&packet.inputs))return;
        std::vector<std::uint8_t> bytes;if(!Netplay::EncodeSpectatorFramePacket(packet,&bytes)||!transport_.SendSpectator(bytes.data(),bytes.size()))return;++publish_frame_;
    }
}
void NetplayRuntime::FreezePublicationTail(){
    if(Correcting()||core_.HasRollbackRequest()||commit_limit_==Netplay::INVALID_FRAME)return;
    if(stage_publication_){
        if(ReadOnly()||setup_.local_player||publish_failed_)return;
        const auto last=commit_limit_;
        const auto retain=[&](const auto& wire){publish_tail_.push_back(wire);return true;};
        const bool complete=stage_publisher_.Publish(core_.ConfirmedThroughAllRemotes(),last,
          [&](auto frame,auto& inputs){return core_.ConfirmedInputs(frame,&inputs);},retain);
        if(!complete||!stage_publisher_.Finish(last+1,retain)){publish_failed_=true;publish_tail_.clear();transport_.StopSpectators();return;}
        publish_deadline_=now_ms_+15'000;DrainPublicationTail();return;
    }
    if(ReadOnly()||generation_||setup_.local_player||publish_failed_||!transport_.HasSpectators())return;
    const auto through=commit_limit_;
    const auto fail=[&](){publish_failed_=true;publish_tail_.clear();publish_deadline_=0;transport_.StopSpectators();};
    if(through>=publish_frame_&&through-publish_frame_>=Netplay::INPUT_HISTORY_SIZE){fail();return;}
    if(!timing_sent_&&setup_.mode!=0){
        auto timing=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
        if(timing.empty()){fail();return;}publish_tail_.push_back(std::move(timing));timing_sent_=true;
    }
    for(;publish_frame_<=through;++publish_frame_){
        Netplay::SpectatorFramePacket packet;packet.sessionId=setup_.session_id;packet.frame=publish_frame_;
        packet.gameplayAbi=GameplayAbi(setup_);packet.playerCount=std::uint8_t(setup_.player_count);
        std::vector<std::uint8_t> bytes;
        if(!core_.ConfirmedInputs(publish_frame_,&packet.inputs)||!Netplay::EncodeSpectatorFramePacket(packet,&bytes)){fail();return;}
        publish_tail_.push_back(std::move(bytes));
    }
    // This bounded confirmed-input tail is independent of the next world's
    // input ring and never delays a player's retirement or generation reset.
    publish_deadline_=now_ms_+15'000;DrainPublicationTail();
}
void NetplayRuntime::DrainPublicationTail(){
    if(!publish_deadline_)return;
    if(now_ms_>=publish_deadline_||transport_.SpectatorState()<0){
        publish_failed_=true;publish_tail_.clear();publish_deadline_=0;transport_.StopSpectators();return;
    }
    const auto begin=clock_us();
    for(unsigned n=0;!publish_tail_.empty()&&n<32;++n){
        if(n&&clock_us()-begin>=2000)break;
        const auto& bytes=publish_tail_.front();if(!transport_.SendSpectator(bytes.data(),bytes.size()))return;
        publish_tail_.pop_front();
    }
    if(publish_tail_.empty()){transport_.StopSpectators(true);publish_deadline_=0;}
}
bool NetplayRuntime::Finish(){
    if(retired_)return true;
    if(Correcting()||core_.HasRollbackRequest())return false;
    if(spectator_&&stage_received_){
        if(!stage_playback_.History().Ended()||stage_playback_.Paused()||stage_playback_.Position()<stage_playback_.History().Received())return false;
        retired_=true;return true;
    }const auto last=core_.LastSimulatedFrame();if(last==Netplay::INVALID_FRAME)return false;
    if(ReadOnly()){retired_=true;return true;}
    if(commit_limit_!=last||!InputsReconciledBeforeNext())return false;
    if(network_){if(!channel_.FlushRetirementFence(core_,last,now_ms_))return Fail("TH15 retirement input flush failed");
        if(!channel_.CanRetire(core_,last))return false;
        if(!channel_.Retire(core_,last,now_ms_))return Fail("TH15 retirement failed");}
    Publish();FreezePublicationTail();retired_=true;return true;
}
bool NetplayRuntime::BeginNextRun(std::uint32_t seed){
    if(!retired_||ReadOnly()||seed>65535||generation_==0xffffffffu)return Fail("Invalid TH15 next generation");
    const auto previous=gate_.Config();
    auto setup=setup_;setup.seed=seed;setup.input_delay=setup.requested_delay;setup.measured_prediction=0;setup.session_id=base_session_^(std::uint64_t(generation_+1)*0x9e3779b97f4a7c15ull);
    if(!Configure(setup,setup.input_delay))return false;++generation_;phase_debt_ms_=0;initial_wait_=0;timing_ready_=timing_sent_=false;publish_frame_=0;
    stage_publisher_={};stage_playback_.Clear();stage_view_.Clear();stage_publication_=stage_received_=false;spectator_frames_.clear();receive_frame_=0;
    if(network_){
        if(setup.mode==0){if(!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy(0)))return Fail("TH15 direct next-generation bootstrap failed");timing_ready_=true;}
        else calibration_.Prepare(gate_.Config(),Netplay::AdonisMode(setup.mode),setup.automatic,setup.requested_delay,setup.prediction_reserve,&previous);
    }return true;
}
double NetplayRuntime::PacedElapsed(double elapsed){
    if(!std::isfinite(elapsed)||elapsed<0)return 0;if(!network_||ReadOnly())return elapsed;
    phase_debt_ms_+=channel_.TakeAdonisDelayMs();const double used=std::min(elapsed*1000,phase_debt_ms_);phase_debt_ms_-=used;return elapsed-used*.001;
}
const std::uint32_t* NetplayRuntime::Status(){
    status_.fill(0);status_[0]=1;status_[1]=configured_;status_[2]=CanStart();status_[3]=NextFrame();status_[4]=Confirmed();
    status_[5]=setup_.player_count;status_[6]=setup_.local_player;status_[7]=setup_.input_delay;status_[8]=AllowsRollback()?SessionSetup::MaxRollbackFrames:0;status_[9]=network_;
    status_[10]=spectator_;status_[11]=playback_;status_[12]=generation_;status_[13]=retired_;status_[14]=unsigned(spectator_?spectator_frames_.size():publish_tail_.size());
    status_[15]=channel_.PacketsSent();status_[16]=channel_.PacketsReceived();status_[17]=channel_.RepairsSent();status_[18]=channel_.PacketsIgnored();
    status_[19]=transport_.IsOpen();const std::string mode=transport_.Mode();status_[20]=mode=="rtc"?1:mode=="relay"?2:spectator_?3:0;
    status_[21]=InitialInputsReady();status_[22]=publish_failed_;status_[23]=*Error()!=0;return status_.data();
}
}
