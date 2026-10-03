#include "NetworkConnection.hpp"
#include <emscripten/emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <eagler/netplay/InputRepairBudget.hpp>
namespace th08::multiplayer {
namespace {
Netplay::SessionChannelConfig channel_policy(Netplay::AdonisMode mode,unsigned prediction=0){
    Netplay::SessionChannelConfig policy;
    // Match TH06/07's bounded stalled-tail repair, including each new epoch.
    // Input is still sampled once; a repair only duplicates captured bytes.
    static_assert(Netplay::InputRepairBudget::StalledMs==Netplay::InputRepairBudget::RetryMs);
    policy.repairIntervalMs=Netplay::InputRepairBudget::StalledMs;
    policy.adonisPhase=mode!=Netplay::AdonisMode::Rollback;
    policy.adonisPredictionFrames=prediction;
    if(policy.adonisPhase)policy.inputResendMs=16;
    return policy;
}
}
std::uint64_t NetworkConnection::Now(){return std::uint64_t(emscripten_get_now());}
bool NetworkConnection::Connect(const char* relay){
    if(enabled||net.ReadOnly()||!relay||!*relay||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    if(!transport.Connect(relay,u8(net.Setup().local_player),u8(net.Setup().player_count)))return false;
    if(net.PreparingWorld())calibration.Prepare(net.Config(),net.Mode(),net.Setup().input_delay_auto,net.InputDelay(),net.Setup().prediction_reserve);
    else if(!channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode()))){transport.Close();return false;}
    enabled=true;invalid_input=false;phase_debt_ms=0;return true;
}
bool NetworkConnection::ConnectSpectator(const char* relay,const char* id){
    if(enabled||net.ReadOnly()||!relay||!*relay||!id||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    const auto length=std::strlen(id);if(length<8||length>64)return false;
    for(std::size_t i=0;i<length;++i){const auto c=id[i];
        if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-'))return false;
    }
    if(!transport.ConnectSpectator(relay,id,u8(net.Setup().player_count)))return false;
    if(!net.BeginSpectator()){transport.Close();return false;}
    enabled=true;invalid_input=false;spectator_finished=false;spectator_error="";
    spectator_frames.Clear();return true;
}
bool NetworkConnection::Pump(bool expects_input){
    if(!enabled)return true;
    if(net.Spectator()){
        if(!spectator_deadline)spectator_deadline=Now()+45'000;
        std::vector<u8> bytes;
        for(unsigned read=0;read<128&&transport.Poll(&bytes);++read){
            if(Netplay::AdonisSpectatorTiming::IsPacket(bytes.data(),bytes.size())){
                Netplay::AdonisSpectatorTiming timing;
                if(net.spectator_timing_ready_||spectator_frames.Received()||net.LastFrame()!=Netplay::INVALID_FRAME||
                   !Netplay::AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),timing)||timing.game!=8||
                   timing.sessionId!=net.Config().sessionId||timing.mode!=net.Setup().adonis_mode||timing.automatic!=net.Setup().input_delay_auto)return false;
                if(!net.apply_spectator_timing(timing.delay,timing.prediction,timing.gameplayAbi))return false;
                spectator_deadline=Now()+15'000;continue;
            }
            Netplay::SpectatorFramePacket packet;
            if(!net.spectator_timing_ready_||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
               !spectator_frames.Append(packet,net.Config())){
                spectator_error="Invalid, out-of-order or excessive spectator history";return false;
            }
            spectator_deadline=Now()+15'000;
        }
        if(Now()>=spectator_deadline){spectator_error="Spectator input stream stalled; players are unaffected";return false;}
        return !transport.Failed()&&!spectator_frames.Failed();
    }
    if(!calibration.Pump(std::uint64_t(emscripten_get_now()*1000),expects_input))return false;
    if(calibration.NeedsApply()){
        const auto choice=calibration.Startup().Selected();
        if(!net.ApplyMeasuredTiming(choice.delay,choice.prediction)||
           !channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode(),choice.prediction)))return false;
        calibration.Applied();
    }
    if(calibration.Waiting())return true;
    if(!channel.Pump(net.gate_,net.core_,Now(),expects_input)||invalid_input)return false;
    PublishConfirmedSpectatorFrames();return true;
}
bool NetworkConnection::ConsumeSpectator(){
    const auto* packet=spectator_frames.Front();if(!packet)return false;
    if(!net.FeedSpectator(*packet)){spectator_error="Spectator native input rejected";return false;}
    spectator_frames.Pop();return true;
}
void NetworkConnection::PublishConfirmedSpectatorFrames(){
    if(!enabled||net.Spectator()||net.Generation()!=0||net.Setup().local_player!=0||
       spectator_publish_failed||!transport.HasSpectators())return;
    const auto last=net.LastFrame(),confirmed=net.ConfirmedThrough();
    if(last==Netplay::INVALID_FRAME||confirmed==Netplay::INVALID_FRAME)return;
    const auto through=std::min(last,confirmed);
    if(transport.SpectatorState()<0){spectator_publish_failed=true;transport.StopSpectators();return;}
    if(net.PreparingWorld()&&!spectator_timing_sent){
        auto bytes=Netplay::MakeAdonisSpectatorTiming(calibration.Startup(),net.Config(),net.Setup().input_delay_auto).Encode();
        if(bytes.empty()){spectator_publish_failed=true;transport.StopSpectators();return;}
        if(!transport.SendSpectator(bytes.data(),bytes.size()))return;spectator_timing_sent=true;
    }
    // Losing publication history must not stop or rewrite the players' run.
    if(through>=spectator_publish&&through-spectator_publish>=Netplay::INPUT_HISTORY_SIZE){spectator_publish_failed=true;transport.StopSpectators();return;}
    const auto began=emscripten_get_now();
    for(unsigned sent=0;spectator_publish<=through&&sent<32;++sent){
        if(sent&&emscripten_get_now()-began>=2)break;
        Netplay::SpectatorFramePacket packet;
        if(!net.ConfirmedInputs(spectator_publish,packet.inputs))return;
        packet.sessionId=net.Config().sessionId;packet.gameplayAbi=net.Config().gameplayAbi;
        packet.playerCount=net.Config().playerCount;packet.frame=spectator_publish;
        std::vector<u8> bytes;
        if(!Netplay::EncodeSpectatorFramePacket(packet,&bytes)||!transport.SendSpectator(bytes.data(),bytes.size()))return;
        ++spectator_publish;
    }
}
void NetworkConnection::FinishSpectator(){
    if(!net.Spectator())return;
    transport.Close();enabled=false;spectator_finished=true;
    spectator_frames.Clear();
}
bool NetworkConnection::Captured(u32 frame){
    if(net.Spectator())return false;
    if(!enabled)return true;
    if(net.core_.LocalFrameForCapture(frame)==Netplay::INVALID_FRAME)return false;
    // A correction can reach a stage boundary before the predicted timeline
    // did. The following loading ticks reuse already captured input. Pump owns
    // retransmission of the highest capture; never announce the older replayed
    // frame as a new capture or move the channel's capture frontier backwards.
    if(channel.LatestCapture()!=Netplay::INVALID_FRAME&&frame<channel.LatestCapture())
        return net.core_.HasLocalCapture(frame)&&channel.Error()==Netplay::SessionChannel::Failure::None;
    // SessionChannel owns F -> F+D. Its capture frontier is physical, too.
    return channel.LocalCaptured(net.core_,frame,Now());
}
bool NetworkConnection::CanRetire()const{return net.CanRetire()&&(!enabled||channel.CanRetire(net.core_,net.LastFrame()));}
bool NetworkConnection::Retire(){return CanRetire()&&(!enabled||channel.Retire(net.core_,net.LastFrame(),Now()))&&net.Retire();}
bool NetworkConnection::BeginGeneration(){phase_debt_ms=0;if(!enabled)return true;channel.Clear();
    if(net.PreparingWorld()){calibration.Prepare(net.Config(),net.Mode(),net.Setup().input_delay_auto,net.InputDelay(),net.Setup().prediction_reserve);return true;}
    return channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode()));}
void NetworkConnection::Close(){transport.Close();channel.Clear();calibration.Clear();enabled=false;invalid_input=false;phase_debt_ms=0;spectator_frames.Clear();}
double NetworkConnection::PacedElapsedSeconds(double elapsed){
    if(!std::isfinite(elapsed)||elapsed<0)return 0;
    if(!enabled||net.ReadOnly()||net.Mode()==Netplay::AdonisMode::Rollback)return elapsed;
    phase_debt_ms+=channel.TakeAdonisDelayMs();
    const auto used=std::min(elapsed*1000,phase_debt_ms);
    phase_debt_ms-=used;return elapsed-used/1000;
}
const char* NetworkConnection::Error()const{
    if(*spectator_error)return spectator_error;
    if(invalid_input)return "Unsupported or out-of-window TH08 input";
    if(transport.Failed())return transport.LastError().c_str();
    if(calibration.Failed())return calibration.Error();
    return channel.ErrorText();
}
bool NetworkConnection::Poll(std::vector<u8>* bytes){
    if(!calibration.Poll(bytes))return false;
    Netplay::PacketType type;
    if(Netplay::PeekPacketType(bytes->data(),bytes->size(),&type)&&type==Netplay::PacketType::Input){
        Netplay::InputPacket packet;
        if(Netplay::DecodeInputPacket(bytes->data(),bytes->size(),&packet)&&packet.sessionId==net.Config().sessionId){
            if(packet.inputCount&&(!net.receive_frame(packet.firstInputFrame)||!net.receive_frame(packet.latestFrame)))invalid_input=true;
            for(u32 i=0;i<packet.inputCount;++i)if(!NetplayRuntime::ValidInput(packet.inputs[i]))invalid_input=true;
            if(invalid_input)return false;
        }
    }
    return true;
}
}
