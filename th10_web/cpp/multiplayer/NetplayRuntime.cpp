#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"
#include <algorithm>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#endif

namespace th10::multiplayer {

namespace {
std::uint64_t network_clock(){
#ifdef __EMSCRIPTEN__
    return static_cast<std::uint64_t>(emscripten_get_now());
#else
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
std::uint32_t live_gameplay_contract(const SessionSetup& setup){
    const auto base=GameplayContract(setup);
    return setup.input_delay ? (base ^ 0x49444c00u ^ setup.input_delay) : base;
}
Netplay::CoreConfig core_config(const SessionSetup& setup,std::uint64_t sessionId,std::uint8_t inputDelay){
    Netplay::CoreConfig core{};
    core.sessionId=sessionId;
    core.playerCount=static_cast<std::uint8_t>(setup.playerCount);
    core.localPlayer=static_cast<std::uint8_t>(setup.localPlayer);
    core.inputDelay=inputDelay;
    core.maxRollbackFrames=12;
    // Match the proven TH06/TH07 production policy: rollback depth stays 12,
    // while movement/focus/shoot may be predicted for at most three frames.
    core.predictableButtons=InputLanes::kShoot|InputLanes::kFocus|InputLanes::kDirection;
    core.directionButtons=InputLanes::kDirection;
    core.maxDirectionPredictionFrames=3;
    core.maxDirectTouchDeltaPredictionFrames=0;
    // TH10 mode 2 carries an absolute hundredth-pixel target, not a delta.
    core.directTouchIsAbsolute=true;
    return core;
}
}

bool NetplayRuntime::Connect(const char* relayUrl){
    if(!configured_||playback_||spectator_||network_enabled_||
       core_.LastSimulatedFrame()!=Netplay::INVALID_FRAME||
       !relayUrl||!relayUrl[0])return false;
    if(!transport_.Connect(relayUrl,gate_.Config().localPlayer,gate_.Config().playerCount))return false;
    network_now_=network_clock();
    if(!channel_.BeginSession(gate_.Config(),network_now_)){transport_.Close();return false;}
    network_enabled_=true;return true;
}

bool NetplayRuntime::ConnectSpectator(const char* relayUrl,const char* spectatorId){
    if(!configured_||playback_||spectator_||network_enabled_||
       core_.LastSimulatedFrame()!=Netplay::INVALID_FRAME||
       !relayUrl||!relayUrl[0]||!spectatorId||!spectatorId[0])return false;
    if(!transport_.ConnectSpectator(relayUrl,spectatorId,gate_.Config().playerCount))return false;
    // Spectator packets are already authoritative logical frames. Keep the
    // live-session ABI (including negotiated delay) but never delay them again.
    if(!core_.Reset(core_config(setup_,base_session_id_,0))){transport_.Close();return false;}
    network_now_=network_clock();network_enabled_=true;spectator_=true;
    spectator_retired_=false;spectator_publish_frame_=spectator_receive_frame_=0;
    spectator_frames_.clear();spectator_error_.clear();return true;
}

bool NetplayRuntime::DrainSpectatorFrames(){
    std::vector<u8> wire;
    while(transport_.Poll(&wire)){
        Netplay::SpectatorFramePacket packet{};
        if(!Netplay::DecodeSpectatorFramePacket(wire.data(),wire.size(),&packet)||
           packet.sessionId!=gate_.Config().sessionId||
           packet.gameplayAbi!=gate_.Config().gameplayAbi||
           packet.playerCount!=gate_.Config().playerCount||
           packet.frame!=spectator_receive_frame_){
            spectator_error_="Invalid spectator stream";return false;
        }
        spectator_frames_.push_back(packet);++spectator_receive_frame_;
    }
    if(transport_.Failed()){
        spectator_error_=transport_.LastError();return false;
    }
    return true;
}

void NetplayRuntime::PublishConfirmedSpectatorFrames(){
    if(spectator_||playback_||generation_!=0||setup_.localPlayer!=0||
       !transport_.HasSpectators())return;
    const auto confirmed=core_.ConfirmedThroughAllRemotes();
    const auto last=core_.LastSimulatedFrame();
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return;
    const auto through=std::min(confirmed,last);
    while(spectator_publish_frame_<=through){
        std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> inputs{};
        if(!core_.ConfirmedInputs(spectator_publish_frame_,&inputs))return;
        Netplay::SpectatorFramePacket packet{};
        packet.sessionId=gate_.Config().sessionId;
        packet.frame=spectator_publish_frame_;
        packet.gameplayAbi=gate_.Config().gameplayAbi;
        packet.playerCount=gate_.Config().playerCount;
        packet.inputs=inputs;
        std::vector<u8> wire;
        if(!Netplay::EncodeSpectatorFramePacket(packet,&wire)||
           !transport_.SendSpectator(wire.data(),wire.size()))return;
        ++spectator_publish_frame_;
    }
}

bool NetplayRuntime::InitialInputsReady()const{
    if(!network_enabled_||playback_||spectator_)return true;
    if(core_.LastSimulatedFrame()!=Netplay::INVALID_FRAME)return true;
    if(!core_.HasLocalCapture(0))return false;
    // Neutral delay-prefix slots do not prove a peer finished loading. Require
    // its actual first captured input, at the negotiated delay-frame offset.
    for(std::uint8_t seat=0;seat<setup_.playerCount;++seat){
        const auto confirmed=core_.ConfirmedThrough(seat);
        if(confirmed==Netplay::INVALID_FRAME||confirmed<setup_.input_delay)return false;
    }
    return true;
}

bool NetplayRuntime::PumpNetwork(bool expectsInput){
    if(!network_enabled_)return true;
    network_now_=network_clock();
    if(spectator_)return DrainSpectatorFrames();
    // A loaded endpoint must not arm the gameplay watchdog or simulate frame
    // zero while another endpoint is still creating its native world. Keep
    // real transport/HELLO/input repair pumping throughout this bounded fence.
    const bool initialReady=InitialInputsReady();
    if(!channel_.Pump(gate_,core_,network_now_,expectsInput&&initialReady))return false;
    if(!InitialInputsReady()&&core_.HasLocalCapture(0)){
        if(!initial_wait_started_){initial_wait_started_=true;initial_wait_since_=network_now_;}
        if(network_now_-initial_wait_since_>=45'000){
            initial_wait_error_="Timed out waiting for peers to load frame zero";return false;
        }
    }else initial_wait_started_=false;
    PublishConfirmedSpectatorFrames();return true;
}

const char* NetplayRuntime::NetworkError()const{
    if(!initial_wait_error_.empty())return initial_wait_error_.c_str();
    if(!spectator_error_.empty())return spectator_error_.c_str();
    if(spectator_&&transport_.Failed())return transport_.LastError().c_str();
    if(channel_.Error()==Netplay::SessionChannel::Failure::Transport)return transport_.LastError().c_str();
    return channel_.ErrorText();
}

bool NetplayRuntime::CanRetireRun()const{
    const auto last=core_.LastSimulatedFrame(),confirmed=core_.ConfirmedThroughAllRemotes();
    if(!CanStart()||core_.HasRollbackRequest()||last==Netplay::INVALID_FRAME||
       confirmed==Netplay::INVALID_FRAME||confirmed<last)return false;
    if(spectator_)return true;
    return !network_enabled_||channel_.CanRetire(core_,last);
}

bool NetplayRuntime::PrepareTransitionFence(){
    const auto last=core_.LastSimulatedFrame();
    if(!CanStart()||core_.HasRollbackRequest()||last==Netplay::INVALID_FRAME)return false;
    if(spectator_||!network_enabled_){
        const auto confirmed=core_.ConfirmedThroughAllRemotes();
        return confirmed!=Netplay::INVALID_FRAME&&confirmed>=last;
    }
    // Once the title has selected a different screen it no longer owns a
    // future gameplay frame that can naturally repair the current fast-lane
    // tail. Push the terminal input and ACK over the reliable lane now; both
    // peers keep pumping until SessionChannel's ordinary retirement fence is
    // actually satisfied.
    if(!channel_.FlushRetirementFence(core_,last,network_now_))return false;
    return channel_.CanRetire(core_,last);
}

bool NetplayRuntime::Reset(const SessionSetup& setup, std::uint64_t sessionId) noexcept {
    Clear();
    if(!Configure(setup,sessionId)){Clear();return false;}
    setup_=setup;base_session_id_=sessionId;return true;
}

bool NetplayRuntime::BeginPlayback(SessionSetup& setup) noexcept {
    // An offline input source owns every lane. It does not fabricate peers or
    // send HELLO/READY messages, and can never attach a live network transport.
    auto next=setup;next.sessionId=0x5250591000000001ull;next.started=false;next.input_delay=0;
    if(!Reset(next,next.sessionId))return false;
    playback_=true;setup=next;return true;
}

bool NetplayRuntime::FeedAuthoritative(u32 frame,const Netplay::FrameInput* inputs,
                                      std::size_t count){
    if(!CanStart()||!inputs||frame!=NextFrame()||count!=setup_.playerCount)return false;
    for(std::size_t seat=0;seat<count;++seat)if(!Netplay::IsValidFrameInput(inputs[seat]))return false;
    if(!core_.ScheduleLocalInput(frame,inputs[setup_.localPlayer]))return false;
    for(std::uint8_t seat=0;seat<count;++seat){
        if(seat==setup_.localPlayer)continue;
        const auto result=core_.SubmitRemoteInput(seat,frame,inputs[seat]);
        if(result!=Netplay::RemoteInputResult::Accepted&&result!=Netplay::RemoteInputResult::Duplicate)return false;
    }
    return true;
}

bool NetplayRuntime::FeedPlayback(u32 frame,const Netplay::FrameInput* inputs,std::size_t count){
    return playback_&&FeedAuthoritative(frame,inputs,count);
}

bool NetplayRuntime::FeedSpectator(u32 frame){
    if(!spectator_||frame!=NextFrame()||spectator_frames_.empty())return false;
    const auto& packet=spectator_frames_.front();
    if(packet.frame!=frame){spectator_error_="Spectator frame gap";return false;}
    if(!FeedAuthoritative(frame,packet.inputs.data(),packet.playerCount)){
        spectator_error_="Spectator input rejected";return false;
    }
    spectator_frames_.pop_front();return true;
}

bool NetplayRuntime::Configure(const SessionSetup& setup,std::uint64_t sessionId) noexcept {
    if(!setup.configured||!sessionId)return false;
    initial_wait_started_=false;initial_wait_since_=0;initial_wait_error_.clear();
    const u32 words[]{3,setup.playerCount,setup.localPlayer,setup.difficulty,setup.seed,
        u32(sessionId),u32(sessionId>>32),setup.input_delay,setup.loadouts[0].character,setup.loadouts[0].shot,
        setup.loadouts[1].character,setup.loadouts[1].shot,setup.loadouts[2].character,setup.loadouts[2].shot};
    SessionSetup checked{};if(!DecodeSessionSetup(checked,words,14))return false;

    Netplay::SessionConfig session{};
    session.sessionId = sessionId;
    session.seed = setup.seed;
    session.gameplayAbi = live_gameplay_contract(setup);
    session.gameId = 10;
    session.playerCount = static_cast<std::uint8_t>(setup.playerCount);
    session.localPlayer = static_cast<std::uint8_t>(setup.localPlayer);
    if (!gate_.Reset(session)) {
        Clear();
        return false;
    }

    const auto core=core_config(setup,sessionId,static_cast<std::uint8_t>(setup.input_delay));
    if (!core_.Reset(core)) {
        Clear();
        return false;
    }
    configured_ = true;
    return true;
}

void NetplayRuntime::Clear() noexcept {
    transport_.Close();channel_.Clear();network_enabled_=false;network_now_=0;playback_=false;
    initial_wait_started_=false;initial_wait_since_=0;initial_wait_error_.clear();
    spectator_=spectator_retired_=false;spectator_publish_frame_=spectator_receive_frame_=0;
    spectator_frames_.clear();spectator_error_.clear();
    gate_.Clear();
    core_.Clear();
    configured_ = false;
    setup_={};base_session_id_=0;generation_=0;retired_=false;
}

bool NetplayRuntime::RetireRun() noexcept {
    if(!CanRetireRun())return false;
    if(spectator_){
        transport_.Close();network_enabled_=false;channel_.Clear();gate_.Clear();core_.Clear();
        configured_=false;retired_=true;spectator_=false;spectator_retired_=true;
        spectator_frames_.clear();return true;
    }
    if(network_enabled_&&!channel_.Retire(core_,core_.LastSimulatedFrame(),network_now_))return false;
    gate_.Clear();core_.Clear();configured_=false;retired_=true;return true;
}

bool NetplayRuntime::BeginNextRun(SessionSetup& setup,u32 seed) noexcept {
    if(!retired_||spectator_retired_||!base_session_id_||generation_==0xffffffffu||seed>65535)return false;
    const auto generation=generation_+1;
    const auto id=base_session_id_^(std::uint64_t(generation)*0x9e3779b97f4a7c15ull);
    if(!id)return false;
    auto next=setup_;next.sessionId=id;next.seed=seed;next.started=true;
    if(!Configure(next,id))return false;
    if(network_enabled_&&!channel_.BeginSession(gate_.Config(),network_now_))return false;
    setup_=next;setup=next;generation_=generation;retired_=false;
    spectator_publish_frame_=0;return true;
}

NetplayRuntime::WireResult NetplayRuntime::ApplyWire(const u8* bytes,std::size_t size){
    if(playback_||spectator_)return WireResult::IgnoredSession;
    if(!bytes||!size)return WireResult::Malformed;
    Netplay::PacketType type{};
    if(!Netplay::PeekPacketType(bytes,size,&type))return WireResult::Malformed;
    if(type==Netplay::PacketType::Session){
        Netplay::SessionPacket packet{};
        if(!Netplay::DecodeSessionPacket(bytes,size,&packet))return WireResult::Malformed;
        if(!configured_||packet.sessionId!=gate_.Config().sessionId)return WireResult::IgnoredSession;
        const auto result=gate_.Apply(packet);
        if(result==Netplay::SessionPacketResult::ContractMismatch)return WireResult::ContractMismatch;
        if(result==Netplay::SessionPacketResult::InvalidPeer)return WireResult::InvalidPeer;
        if(result==Netplay::SessionPacketResult::ReadyBeforeHello)return WireResult::Malformed;
        return WireResult::Accepted;
    }
    if(type==Netplay::PacketType::Input){
        Netplay::InputPacket packet{};
        if(!Netplay::DecodeInputPacket(bytes,size,&packet))return WireResult::Malformed;
        if(!configured_||packet.sessionId!=gate_.Config().sessionId)return WireResult::IgnoredSession;
        if(packet.senderPlayer==gate_.Config().localPlayer||packet.senderPlayer>=gate_.Config().playerCount)
            return WireResult::InvalidPeer;
        Netplay::RemoteInputResult result{};
        return core_.ApplyInputPacket(packet,&result)?WireResult::Accepted:WireResult::ContractMismatch;
    }
    return WireResult::Malformed;
}

bool NetplayRuntime::BuildInputWire(u8 peer,u32 latest,u32 sequence,u32 ack,std::vector<u8>& out)const{
    if(playback_||spectator_||!CanStart()||peer>=setup_.playerCount||peer==setup_.localPlayer||
       latest==Netplay::INVALID_FRAME||!core_.HasLocalCapture(latest))return false;
    const auto target=core_.LocalFrameForCapture(latest);
    if(target==Netplay::INVALID_FRAME)return false;
    auto packet=core_.BuildInputPacket(peer,target,sequence,ack);
    packet.senderFrame=NextFrame();
    return Netplay::EncodeInputPacket(packet,&out);
}

} // namespace th10::multiplayer
