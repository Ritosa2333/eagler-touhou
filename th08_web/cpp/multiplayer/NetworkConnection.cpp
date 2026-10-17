#include "NetworkConnection.hpp"
#include <emscripten/emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <eagler/netplay/InputRepairBudget.hpp>
namespace th08::multiplayer {
namespace {
const char* spectator_stage_label(std::uint32_t encoded){
    static constexpr const char* labels[]{"Stage 1","Stage 2","Stage 3","Stage 4A","Stage 4B",
        "Stage 5","Final A","Final B","Extra"};
    const auto stage=encoded&0xffu;return stage<9?labels[stage]:nullptr;
}
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
const char* NetworkConnection::SpectatorState(){return spectator_view.State(spectator_playback,spectator_stage_label);}
bool NetworkConnection::Connect(const char* relay){
    if(enabled||net.ReadOnly()||!relay||!*relay||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    spectator_publish_failed=false;spectator_timing_sent=false;spectator_deadline=0;spectator_error="";
    if(!transport.Connect(relay,u8(net.Setup().local_player),u8(net.Setup().player_count),true))return false;
    spectator_publisher_started=false;
    if(net.PreparingWorld())calibration.Prepare(net.Config(),net.Mode(),net.Setup().input_delay_auto,net.InputDelay(),net.Setup().prediction_reserve);
    else if(!channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode()))){transport.Close();return false;}
    enabled=true;invalid_input=false;capture_pump_pending=false;phase_debt_ms=0;input_gap_pacing.Reset(Now());return true;
}
bool NetworkConnection::ConnectSpectator(const char* relay,const char* id){
    if(enabled||net.ReadOnly()||!relay||!*relay||!id||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    const auto length=std::strlen(id);if(length<8||length>64)return false;
    for(std::size_t i=0;i<length;++i){const auto c=id[i];
        if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-'))return false;
    }
    spectator_publish_failed=false;spectator_timing_sent=false;spectator_deadline=0;spectator_error="";
    if(!transport.ConnectSpectator(relay,id,u8(net.Setup().player_count),true))return false;
    if(!net.BeginSpectator()){transport.Close();return false;}
    enabled=true;invalid_input=false;capture_pump_pending=false;spectator_finished=false;spectator_error="";input_gap_pacing.Reset(Now());
    spectator_playback.Clear();spectator_view.Clear();return true;
}
bool NetworkConnection::Pump(bool expects_input,bool capture_follows){
    // A new independent pump ends an unconsumed capture promise. Only
    // PumpForCapture may consume it without another receive/send pass.
    if(capture_pump_pending&&!FlushDeferredInputs())return false;
    if(!enabled)return true;
    if(net.Spectator()){
        if(!spectator_deadline)spectator_deadline=Now()+45'000;
        std::vector<u8> bytes;
        for(unsigned read=0;read<128&&transport.Poll(&bytes);++read){
            if(Netplay::AdonisSpectatorTiming::IsPacket(bytes.data(),bytes.size())){
                Netplay::AdonisSpectatorTiming timing;
                if(net.spectator_timing_ready_||spectator_playback.History().Received()||net.LastFrame()!=Netplay::INVALID_FRAME||
                   !Netplay::AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),timing)||timing.game!=8||
                   timing.sessionId!=net.Config().sessionId||timing.mode!=net.Setup().adonis_mode||timing.automatic!=net.Setup().input_delay_auto)return false;
                if(!net.apply_spectator_timing(timing.delay,timing.prediction,timing.gameplayAbi))return false;
                spectator_deadline=Now()+15'000;continue;
            }
            if(Netplay::IsSpectatorStageEvent(bytes.data(),bytes.size())){
                Netplay::SpectatorStageEvent event;
                if(!net.spectator_timing_ready_||!Netplay::DecodeSpectatorStageEvent(bytes.data(),bytes.size(),&event)||
                   !spectator_playback.Receive(event)){
                    spectator_error="Invalid, out-of-order or excessive spectator stage history";return false;
                }
                spectator_deadline=Now()+15'000;continue;
            }
            Netplay::SpectatorFramePacket packet;
            if(!net.spectator_timing_ready_||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
               !spectator_playback.Receive(packet)){
                spectator_error="Invalid, out-of-order or excessive spectator history";return false;
            }
            spectator_deadline=Now()+15'000;
        }
        // Before the first valid bootstrap, bound how long we wait for the
        // spectator stream to become usable. After Ready, an idle source is a
        // valid state (for example, the players may pause); socket close/error
        // and malformed packets remain authoritative transport failures.
        if(spectator_playback.Ready())spectator_deadline=0;
        if(spectator_deadline&&Now()>=spectator_deadline){spectator_error="Spectator input stream stalled; players are unaffected";return false;}
        return !transport.Failed();
    }
    if(calibration.Waiting()&&channel.Retiring()&&!channel.Active()&&!channel.PumpRetirement(Now()))return false;
    if(!calibration.Pump(std::uint64_t(emscripten_get_now()*1000),expects_input))return false;
    if(calibration.NeedsApply()){
        const auto choice=calibration.Startup().Selected();
        if(!net.ApplyMeasuredTiming(choice.delay,choice.prediction)||
           !channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode(),choice.prediction)))return false;
        calibration.Applied();input_gap_pacing.Reset(Now());
    }
    if(calibration.Waiting()){input_gap_pacing.Reset(Now());return true;}
    const auto now=Now();
    const auto next_capture=net.NextCaptureFrame(),latest_capture=channel.LatestCapture();
    const bool captureFollows=capture_follows&&expects_input&&net.Mode()==Netplay::AdonisMode::Rollback&&
        net.CanCapture()&&!net.HasCapture(next_capture)&&
        (latest_capture==Netplay::INVALID_FRAME||next_capture==latest_capture+1);
    if(!channel.Pump(net.gate_,net.core_,now,expects_input,captureFollows)||invalid_input){
        // Keep the original failure while completing any valid old tail owed
        // before a watchdog or title-input validation rejected this pump.
        channel.FlushDeferredInputs(net.core_,Now());return false;
    }
    // Measure a continuous input shortage only after draining received packets.
    // Rewinding simulation does not create additional prediction room.
    input_gap_pacing.Update(now,net.MissingRemoteInputs(),
        net.CanStart()&&!net.ReadOnly()&&net.Mode()==Netplay::AdonisMode::Rollback&&expects_input);
    capture_pump_pending=captureFollows;
    return true;
}
bool NetworkConnection::PumpForCapture(bool expects_input){
    if(capture_pump_pending){
        capture_pump_pending=false;
        return !invalid_input&&!transport.Failed()&&!calibration.Failed()&&
            channel.Error()==Netplay::SessionChannel::Failure::None;
    }
    const bool ok=Pump(expects_input,true);
    // This call is already inside Step; only an earlier callback pump creates
    // a promise another caller may consume. The actual send obligation stays
    // in SessionChannel until Captured or FlushDeferredInputs completes it.
    capture_pump_pending=false;return ok;
}
bool NetworkConnection::FlushDeferredInputs(){
    capture_pump_pending=false;
    return !enabled||net.Spectator()||channel.FlushDeferredInputs(net.core_,Now());
}
bool NetworkConnection::RecordSpectatorStage(u32 frame,u32 stage,u32 schema,const std::vector<u8>& initializer,
        const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs,u8 boss_slots){
    if(!enabled||net.Spectator()||net.Generation()!=0||net.Setup().local_player!=0||spectator_publish_failed)return true;
    if(!spectator_publisher_started){
        if(initializer.empty())return true;
        if(!schema||!spectator_publisher.Reset('8',u8(net.Setup().player_count),
            net.Config().sessionId,net.Config().gameplayAbi,frame)){
            spectator_publish_failed=true;transport.StopSpectators();return true;
        }
        spectator_publisher_started=true;
    }
    Netplay::SpectatorStagePublisher::Frame record;record.frame=frame;record.stage=stage;
    record.schema=schema;record.initializer=initializer;record.inputs=inputs;record.hasInputs=true;record.bossSlots=boss_slots;
    if(!spectator_publisher.Record(std::move(record))){
        spectator_publish_failed=true;transport.StopSpectators();return true;
    }
    return true;
}
void NetworkConnection::PublishConfirmedSpectatorFrames(){
    if(!enabled||net.Spectator()||net.Generation()!=0||net.Setup().local_player!=0||!spectator_publisher_started||
       spectator_publish_failed)return;
    const auto last=net.LastFrame(),confirmed=net.ConfirmedThrough();
    if(last==Netplay::INVALID_FRAME||confirmed==Netplay::INVALID_FRAME)return;
    const auto through=std::min(last,confirmed);
    if(transport.SpectatorState()<0){spectator_publish_failed=true;transport.StopSpectators();return;}
    if(net.PreparingWorld()&&!spectator_timing_sent){
        auto bytes=Netplay::MakeAdonisSpectatorTiming(calibration.Startup(),net.Config(),net.Setup().input_delay_auto).Encode();
        if(bytes.empty()){spectator_publish_failed=true;transport.StopSpectators();return;}
        if(!transport.SendSpectator(bytes.data(),bytes.size()))return;spectator_timing_sent=true;
    }
    // Stage checkpoints and Boss nodes are queued only after the complete
    // native frame has reconciled. A relay-only failure remains viewer-local.
    const bool published=spectator_publisher.Publish(through,last,
        [&](u32 frame,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){return net.ConfirmedInputs(frame,inputs);},
        [&](const std::vector<u8>& bytes){return transport.SendSpectator(bytes.data(),bytes.size());});
    if(!published&&transport.SpectatorState()<0){spectator_publish_failed=true;transport.StopSpectators();}
}
void NetworkConnection::FinishSpectator(){
    if(!net.Spectator())return;
    transport.Close();enabled=false;spectator_finished=true;
    spectator_playback.Clear();spectator_view.Clear();
}
bool NetworkConnection::Captured(u32 frame){
    capture_pump_pending=false;
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
bool NetworkConnection::CanRetire()const{
    if(!net.CanRetire()||(enabled&&!channel.CanRetire(net.core_,net.LastFrame())))return false;
    // Publication is bounded per pump. Queue the final confirmed frame before
    // replacing the input history, so viewers can consume the native Restart.
    return !enabled||net.Spectator()||net.Generation()!=0||net.Setup().local_player!=0||
        spectator_publish_failed||transport.SpectatorState()<=0||spectator_publisher.Published()>net.LastFrame();
}
bool NetworkConnection::Retire(){
    if(!FlushDeferredInputs()||!CanRetire()||(enabled&&!channel.Retire(net.core_,net.LastFrame(),Now()))||!net.Retire())return false;
    retired_config=net.Config();return true;
}
bool NetworkConnection::BeginGeneration(){capture_pump_pending=false;phase_debt_ms=0;input_gap_pacing.Reset(Now());
    // A retry is a new native run. Never carry its prior stage identity,
    // published sequence or viewer controls into the next generation.
    spectator_publisher={};spectator_publisher_started=false;spectator_playback.Clear();spectator_view.Clear();
    spectator_publish_failed=false;spectator_timing_sent=false;
    if(!enabled)return true;
    // Keep the previous terminal ACKs alive during measurement. The shared
    // calibrator discards only valid packets from this actually retired epoch.
    if(net.PreparingWorld()){calibration.Prepare(net.Config(),net.Mode(),net.Setup().input_delay_auto,net.InputDelay(),net.Setup().prediction_reserve,&retired_config);return true;}
    return channel.BeginSession(net.Config(),Now(),channel_policy(net.Mode()));}
void NetworkConnection::Close(){transport.Close();channel.Clear();calibration.Clear();retired_config={};enabled=false;invalid_input=false;capture_pump_pending=false;phase_debt_ms=0;input_gap_pacing.Reset(0);spectator_playback.Clear();spectator_view.Clear();spectator_publisher={};spectator_publisher_started=false;spectator_finished=false;spectator_publish_failed=false;spectator_timing_sent=false;spectator_deadline=0;spectator_error="";}
double NetworkConnection::IntervalScale()const{
    const auto phase=channel.IntervalScale();
    return enabled&&net.CanStart()&&!net.ReadOnly()&&net.Mode()==Netplay::AdonisMode::Rollback&&input_gap_pacing.Scale()>1
        ?std::max(phase,input_gap_pacing.Scale()):phase;
}
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
