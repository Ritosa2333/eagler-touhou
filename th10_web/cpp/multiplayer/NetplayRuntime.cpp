#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"
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
}

bool NetplayRuntime::Connect(const char* relayUrl){
    if(!configured_||playback_||network_enabled_||core_.LastSimulatedFrame()!=Netplay::INVALID_FRAME||
       !relayUrl||!relayUrl[0])return false;
    if(!transport_.Connect(relayUrl,gate_.Config().localPlayer,gate_.Config().playerCount))return false;
    network_now_=network_clock();
    if(!channel_.BeginSession(gate_.Config(),network_now_)){transport_.Close();return false;}
    network_enabled_=true;return true;
}

bool NetplayRuntime::PumpNetwork(bool expectsInput){
    if(!network_enabled_)return true;
    network_now_=network_clock();
    return channel_.Pump(gate_,core_,network_now_,expectsInput);
}

const char* NetplayRuntime::NetworkError()const{
    if(channel_.Error()==Netplay::SessionChannel::Failure::Transport)return transport_.LastError().c_str();
    return channel_.ErrorText();
}

bool NetplayRuntime::CanRetireRun()const{
    const auto last=core_.LastSimulatedFrame(),confirmed=core_.ConfirmedThroughAllRemotes();
    if(!CanStart()||core_.HasRollbackRequest()||last==Netplay::INVALID_FRAME||
       confirmed==Netplay::INVALID_FRAME||confirmed<last)return false;
    return !network_enabled_||channel_.CanRetire(core_,last);
}

bool NetplayRuntime::Reset(const SessionSetup& setup, std::uint64_t sessionId) noexcept {
    Clear();
    if(!Configure(setup,sessionId)){Clear();return false;}
    setup_=setup;base_session_id_=sessionId;return true;
}

bool NetplayRuntime::BeginPlayback(SessionSetup& setup) noexcept {
    // An offline input source owns every lane. It does not fabricate peers or
    // send HELLO/READY messages, and can never attach a live network transport.
    auto next=setup;next.sessionId=0x5250591000000001ull;next.started=false;
    if(!Reset(next,next.sessionId))return false;
    playback_=true;setup=next;return true;
}

bool NetplayRuntime::FeedPlayback(u32 frame,const Netplay::FrameInput* inputs,std::size_t count){
    if(!playback_||!CanStart()||!inputs||frame!=NextFrame()||count!=setup_.playerCount)return false;
    for(std::size_t seat=0;seat<count;++seat)if(!Netplay::IsValidFrameInput(inputs[seat]))return false;
    if(!core_.ScheduleLocalInput(frame,inputs[setup_.localPlayer]))return false;
    for(std::uint8_t seat=0;seat<count;++seat){
        if(seat==setup_.localPlayer)continue;
        const auto result=core_.SubmitRemoteInput(seat,frame,inputs[seat]);
        if(result!=Netplay::RemoteInputResult::Accepted&&result!=Netplay::RemoteInputResult::Duplicate)return false;
    }
    return true;
}

bool NetplayRuntime::Configure(const SessionSetup& setup,std::uint64_t sessionId) noexcept {
    if(!setup.configured||!sessionId)return false;
    const u32 words[]{2,setup.playerCount,setup.localPlayer,setup.difficulty,setup.seed,
        u32(sessionId),u32(sessionId>>32),setup.loadouts[0].character,setup.loadouts[0].shot,
        setup.loadouts[1].character,setup.loadouts[1].shot,setup.loadouts[2].character,setup.loadouts[2].shot};
    SessionSetup checked{};if(!DecodeSessionSetup(checked,words,13))return false;

    Netplay::SessionConfig session{};
    session.sessionId = sessionId;
    session.seed = setup.seed;
    session.gameplayAbi = GameplayContract(setup);
    session.gameId = 10;
    session.playerCount = static_cast<std::uint8_t>(setup.playerCount);
    session.localPlayer = static_cast<std::uint8_t>(setup.localPlayer);
    if (!gate_.Reset(session)) {
        Clear();
        return false;
    }

    Netplay::CoreConfig core{};
    core.sessionId = sessionId;
    core.playerCount = session.playerCount;
    core.localPlayer = session.localPlayer;
    core.inputDelay = 0;
    core.maxRollbackFrames = 12;
    // Match the proven TH07 production policy: movement/focus/shoot may be
    // predicted, while Bomb and Pause remain exact edge actions.
    core.predictableButtons = InputLanes::kShoot | InputLanes::kFocus |
                              InputLanes::kDirection;
    core.directionButtons = InputLanes::kDirection;
    core.maxDirectionPredictionFrames = 3;
    core.maxDirectTouchDeltaPredictionFrames = 0;
    if (!core_.Reset(core)) {
        Clear();
        return false;
    }
    configured_ = true;
    return true;
}

void NetplayRuntime::Clear() noexcept {
    transport_.Close();channel_.Clear();network_enabled_=false;network_now_=0;playback_=false;
    gate_.Clear();
    core_.Clear();
    configured_ = false;
    setup_={};base_session_id_=0;generation_=0;retired_=false;
}

bool NetplayRuntime::RetireRun() noexcept {
    if(!CanRetireRun())return false;
    if(network_enabled_&&!channel_.Retire(core_,core_.LastSimulatedFrame(),network_now_))return false;
    gate_.Clear();core_.Clear();configured_=false;retired_=true;return true;
}

bool NetplayRuntime::BeginNextRun(SessionSetup& setup,u32 seed) noexcept {
    if(!retired_||!base_session_id_||generation_==0xffffffffu||seed>65535)return false;
    const auto generation=generation_+1;
    const auto id=base_session_id_^(std::uint64_t(generation)*0x9e3779b97f4a7c15ull);
    if(!id)return false;
    auto next=setup_;next.sessionId=id;next.seed=seed;next.started=true;
    if(!Configure(next,id))return false;
    if(network_enabled_&&!channel_.BeginSession(gate_.Config(),network_now_))return false;
    setup_=next;setup=next;generation_=generation;retired_=false;return true;
}

NetplayRuntime::WireResult NetplayRuntime::ApplyWire(const u8* bytes,std::size_t size){
    if(playback_)return WireResult::IgnoredSession;
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
    if(playback_||!CanStart()||peer>=setup_.playerCount||peer==setup_.localPlayer||
       latest==Netplay::INVALID_FRAME||!core_.HasLocalCapture(latest))return false;
    auto packet=core_.BuildInputPacket(peer,latest,sequence,ack);
    packet.senderFrame=NextFrame();
    return Netplay::EncodeInputPacket(packet,&out);
}

} // namespace th10::multiplayer
