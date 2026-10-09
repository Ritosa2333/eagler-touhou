#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"
#include <algorithm>
#include <cmath>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#endif
namespace th11::multiplayer {
namespace {
std::uint64_t clock_us(){
#ifdef __EMSCRIPTEN__
    return std::uint64_t(emscripten_get_now()*1000);
#else
    return std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
Netplay::SessionChannelConfig channel_policy(){
    Netplay::SessionChannelConfig p;p.adonisPhase=true;p.adonisPredictionFrames=0;p.inputResendMs=16;return p;
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
bool NetplayRuntime::Configure(const SessionSetup& setup,unsigned delay){
    if(!setup.valid()||delay>9)return Fail("Invalid TH11 multiplayer setup");
    Netplay::SessionConfig gate;gate.sessionId=setup.session_id;gate.seed=setup.seed;gate.gameplayAbi=GameplayAbi(setup);
    gate.gameId=11;gate.playerCount=std::uint8_t(setup.player_count);gate.localPlayer=std::uint8_t(setup.local_player);
    Netplay::CoreConfig core;core.sessionId=setup.session_id;core.playerCount=gate.playerCount;core.localPlayer=gate.localPlayer;
    core.inputDelay=std::uint8_t(delay);core.allowPrediction=false;
    // Common core requires a positive retained-decision window even in
    // lockstep. This is input/ACK history only, never a prediction allowance.
    core.maxRollbackFrames=12;core.predictableButtons=0;core.maxDirectionPredictionFrames=0;core.directTouchIsAbsolute=true;
    if(!gate_.Reset(gate)||!core_.Reset(core))return Fail("TH11 lockstep core rejected setup");
    setup_=setup;configured_=true;retired_=false;return true;
}
void NetplayRuntime::Clear(){
    transport_.Close();channel_.Clear();calibration_.Clear();gate_.Clear();core_.Clear();
    configured_=network_=playback_=spectator_=retired_=false;timing_ready_=timing_sent_=publish_failed_=false;
    setup_={};base_session_=now_ms_=initial_wait_=spectator_deadline_=publish_deadline_=0;generation_=publish_frame_=receive_frame_=0;
    phase_debt_ms_=0;error_.clear();spectator_frames_.clear();publish_tail_.clear();status_.fill(0);
}
bool NetplayRuntime::Reset(const SessionSetup& setup){Clear();base_session_=setup.session_id;return Configure(setup,setup.input_delay);}
bool NetplayRuntime::Connect(const char* url){
    if(!configured_||network_||ReadOnly()||!url||!*url)return Fail("Invalid TH11 player connection");
    if(!transport_.Connect(url,std::uint8_t(setup_.local_player),std::uint8_t(setup_.player_count)))return Fail("TH11 transport could not connect");
    now_ms_=clock_us()/1000;network_=true;
    calibration_.Prepare(gate_.Config(),Netplay::AdonisMode::Delay,setup_.automatic,setup_.requested_delay,setup_.prediction_reserve);return true;
}
bool NetplayRuntime::ConnectSpectator(const char* url,const char* id){
    if(!configured_||network_||!url||!*url||!id||!*id)return Fail("Invalid TH11 spectator admission");
    if(!transport_.ConnectSpectator(url,id,std::uint8_t(setup_.player_count)))return Fail("TH11 spectator could not connect");
    spectator_=network_=true;now_ms_=clock_us()/1000;spectator_deadline_=now_ms_+90'000;return true;
}
bool NetplayRuntime::BeginPlayback(const SessionSetup& setup){Clear();if(!Configure(setup,0))return false;playback_=timing_ready_=true;return true;}
bool NetplayRuntime::CanStart()const{return configured_&&error_.empty()&&(playback_||(spectator_?timing_ready_:!calibration_.Waiting()&&gate_.CanStart()));}
std::uint32_t NetplayRuntime::NextFrame()const{const auto f=core_.LastSimulatedFrame();return f==Netplay::INVALID_FRAME?0:f+1;}
bool NetplayRuntime::NeedsCapture()const{return CanStart()&&!ReadOnly()&&!retired_&&!core_.HasLocalCapture(NextFrame());}
bool NetplayRuntime::Capture(std::uint32_t frame,const Netplay::FrameInput& input){
    if(!CanStart()||ReadOnly()||retired_||frame!=NextFrame()||core_.HasLocalCapture(frame)||
       !Netplay::IsValidFrameInput(input)||!core_.ScheduleLocalInput(frame,input))return Fail("Invalid or repeated TH11 physical capture");
    return !network_||channel_.LocalCaptured(core_,frame,now_ms_)||Fail("TH11 input send failed");
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
    if(transport_.Disconnected())return Fail(transport_.LastError().empty()?"TH11 input connection ended":transport_.LastError().c_str());
    if(calibration_.Waiting()&&channel_.Retiring()&&!channel_.Active()&&!channel_.PumpRetirement(now_ms_))return Fail(channel_.ErrorText());
    if(!calibration_.Pump(clock_us(),ready))return Fail(calibration_.Error());
    if(calibration_.NeedsApply()){
        const auto choice=calibration_.Startup().Selected();
        if(choice.prediction||choice.delay>9||NextFrame()!=0)return Fail("TH11 received a non-lockstep calibration result");
        auto setup=setup_;setup.input_delay=choice.delay;
        if(!Configure(setup,choice.delay)||!channel_.BeginSession(gate_.Config(),now_ms_,channel_policy()))return Fail("TH11 calibration commit failed");
        calibration_.Applied();timing_ready_=true;
    }
    if(calibration_.Waiting())return true;
    const bool active=ready&&InitialInputsReady()&&!retired_;
    if(!channel_.Pump(gate_,core_,now_ms_,active))return Fail(channel_.ErrorText());
    if(ready&&CanStart()&&!InitialInputsReady()){
        if(!initial_wait_)initial_wait_=now_ms_;
        if(now_ms_-initial_wait_>=45'000)return Fail("Timed out waiting for TH11 frame-zero inputs");
    }else initial_wait_=0;
    Publish();return error_.empty();
}
Netplay::FrameDecision NetplayRuntime::Prepare(std::uint32_t frame)const{
    return CanStart()&&InitialInputsReady()&&!retired_?core_.PrepareFrame(frame):Netplay::FrameDecision{};
}
bool NetplayRuntime::MarkSimulated(std::uint32_t frame,const Netplay::FrameDecision& d){
    if(!CanStart()||!d.canAdvance||d.predictedMask||!core_.MarkSimulated(frame,d)||core_.HasRollbackRequest())return Fail("TH11 attempted an unconfirmed logical frame");
    Publish();return true;
}
bool NetplayRuntime::ConfirmedInputs(std::uint32_t f,std::array<Netplay::FrameInput,3>& out)const{return core_.ConfirmedInputs(f,&out);}
bool NetplayRuntime::Feed(std::uint32_t frame,const Netplay::FrameInput* inputs,unsigned count){
    if(frame!=NextFrame()||count!=setup_.player_count||!inputs)return Fail("Invalid TH11 recorded frame");
    for(unsigned s=0;s<count;++s)if(!Netplay::IsValidFrameInput(inputs[s]))return Fail("Invalid TH11 recorded input");
    if(!core_.ScheduleLocalInput(frame,inputs[setup_.local_player]))return Fail("TH11 recorded local lane rejected");
    for(unsigned s=0;s<count;++s)if(s!=setup_.local_player){const auto r=core_.SubmitRemoteInput(std::uint8_t(s),frame,inputs[s]);if(r!=Netplay::RemoteInputResult::Accepted&&r!=Netplay::RemoteInputResult::Duplicate)return Fail("TH11 recorded remote lane rejected");}
    return true;
}
bool NetplayRuntime::FeedPlayback(std::uint32_t f,const Netplay::FrameInput* inputs,unsigned count){return playback_&&Feed(f,inputs,count);}
bool NetplayRuntime::DrainSpectator(){
    std::vector<std::uint8_t> bytes;
    for(unsigned n=0;n<128&&transport_.Poll(&bytes);++n){
        if(Netplay::AdonisSpectatorTiming::IsPacket(bytes.data(),bytes.size())){
            Netplay::AdonisSpectatorTiming timing;
            if(timing_ready_||receive_frame_||!Netplay::AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),timing)||
               timing.game!=11||timing.mode!=1||timing.prediction||timing.sessionId!=setup_.session_id||timing.automatic!=setup_.automatic)return Fail("Invalid TH11 spectator timing");
            auto setup=setup_;setup.input_delay=timing.delay;
            if(GameplayAbi(setup)!=timing.gameplayAbi||!Configure(setup,0))return Fail("TH11 spectator contract mismatch");
            timing_ready_=true;spectator_deadline_=now_ms_+15'000;continue;
        }
        Netplay::SpectatorFramePacket packet;
        if(!timing_ready_||spectator_frames_.size()>=8192||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
           packet.sessionId!=setup_.session_id||packet.gameplayAbi!=GameplayAbi(setup_)||packet.playerCount!=setup_.player_count||packet.frame!=receive_frame_)
            return Fail("Invalid TH11 spectator input stream");
        spectator_frames_.push_back(packet);++receive_frame_;spectator_deadline_=now_ms_+15'000;
    }
    if(spectator_frames_.empty()){
        if(transport_.Failed())return Fail(transport_.LastError().c_str());
        if(now_ms_>=spectator_deadline_)return Fail("TH11 spectator stream ended or stalled; players are unaffected");
    }return true;
}
bool NetplayRuntime::FeedSpectator(std::uint32_t frame){
    if(!spectator_||!timing_ready_||spectator_frames_.empty())return false;
    const auto& packet=spectator_frames_.front();if(packet.frame!=frame||!Feed(frame,packet.inputs.data(),packet.playerCount))return false;
    spectator_frames_.pop_front();return true;
}
void NetplayRuntime::Publish(){
    if(!network_||ReadOnly()||generation_||setup_.local_player||publish_failed_||!transport_.HasSpectators())return;
    const auto through=core_.LastSimulatedFrame();if(through==Netplay::INVALID_FRAME)return;
    if(transport_.SpectatorState()<0||(through>=publish_frame_&&through-publish_frame_>=Netplay::INPUT_HISTORY_SIZE)){publish_failed_=true;transport_.StopSpectators();return;}
    if(!timing_sent_){
        auto bytes=Netplay::MakeAdonisSpectatorTiming(calibration_.Startup(),gate_.Config(),setup_.automatic).Encode();
        if(bytes.empty()){publish_failed_=true;transport_.StopSpectators();return;}
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
    if(ReadOnly()||generation_||setup_.local_player||publish_failed_||!transport_.HasSpectators())return;
    const auto through=core_.LastSimulatedFrame();if(through==Netplay::INVALID_FRAME)return;
    const auto fail=[&](){publish_failed_=true;publish_tail_.clear();publish_deadline_=0;transport_.StopSpectators();};
    if(through>=publish_frame_&&through-publish_frame_>=Netplay::INPUT_HISTORY_SIZE){fail();return;}
    if(!timing_sent_){
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
    if(retired_)return true;const auto last=core_.LastSimulatedFrame();if(last==Netplay::INVALID_FRAME)return false;
    if(ReadOnly()){retired_=true;return true;}
    if(network_){if(!channel_.FlushRetirementFence(core_,last,now_ms_))return Fail("TH11 retirement input flush failed");
        if(!channel_.CanRetire(core_,last))return false;
        if(!channel_.Retire(core_,last,now_ms_))return Fail("TH11 retirement failed");}
    Publish();FreezePublicationTail();retired_=true;return true;
}
bool NetplayRuntime::BeginNextRun(std::uint32_t seed){
    if(!retired_||ReadOnly()||seed>65535||generation_==0xffffffffu)return Fail("Invalid TH11 next generation");
    const auto previous=gate_.Config();
    auto setup=setup_;setup.seed=seed;setup.input_delay=setup.requested_delay;setup.session_id=base_session_^(std::uint64_t(generation_+1)*0x9e3779b97f4a7c15ull);
    if(!Configure(setup,setup.input_delay))return false;++generation_;phase_debt_ms_=0;initial_wait_=0;timing_ready_=timing_sent_=false;publish_frame_=0;
    if(network_)calibration_.Prepare(gate_.Config(),Netplay::AdonisMode::Delay,setup.automatic,setup.requested_delay,setup.prediction_reserve,&previous);return true;
}
double NetplayRuntime::PacedElapsed(double elapsed){
    if(!std::isfinite(elapsed)||elapsed<0)return 0;if(!network_||ReadOnly())return elapsed;
    phase_debt_ms_+=channel_.TakeAdonisDelayMs();const double used=std::min(elapsed*1000,phase_debt_ms_);phase_debt_ms_-=used;return elapsed-used*.001;
}
const std::uint32_t* NetplayRuntime::Status(){
    status_.fill(0);status_[0]=1;status_[1]=configured_;status_[2]=CanStart();status_[3]=NextFrame();status_[4]=Confirmed();
    status_[5]=setup_.player_count;status_[6]=setup_.local_player;status_[7]=setup_.input_delay;status_[8]=0;status_[9]=network_;
    status_[10]=spectator_;status_[11]=playback_;status_[12]=generation_;status_[13]=retired_;status_[14]=unsigned(spectator_?spectator_frames_.size():publish_tail_.size());
    status_[15]=channel_.PacketsSent();status_[16]=channel_.PacketsReceived();status_[17]=channel_.RepairsSent();status_[18]=channel_.PacketsIgnored();
    status_[19]=transport_.IsOpen();const std::string mode=transport_.Mode();status_[20]=mode=="rtc"?1:mode=="relay"?2:spectator_?3:0;
    status_[21]=InitialInputsReady();status_[22]=publish_failed_;status_[23]=*Error()!=0;return status_.data();
}
}
