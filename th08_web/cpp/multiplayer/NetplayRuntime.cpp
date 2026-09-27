#include "NetplayRuntime.hpp"
#include "InputSample.hpp"
#include <algorithm>
#include <cmath>

namespace th08::multiplayer {
bool NetplayRuntime::ValidInput(const Netplay::FrameInput& input) noexcept {
    return ValidInputSample(input);
}
bool NetplayRuntime::configure(const SessionSetup& setup) noexcept {
    const std::uint32_t words[]{3,setup.player_count,setup.local_player,setup.difficulty,setup.seed,
        std::uint32_t(setup.session_id),std::uint32_t(setup.session_id>>32),
        setup.build[0],setup.build[1],setup.build[2],setup.build[3],
        setup.characters[0],0,setup.characters[1],0,setup.characters[2],0};
    SessionSetup validated;
    if(!setup.configured||!decode_session_setup(validated,words,17))return false;
    Netplay::SessionConfig session;
    session.sessionId=setup.session_id;session.seed=setup.seed;
    session.gameplayAbi=gameplay_contract(setup);session.gameId=8;
    session.playerCount=std::uint8_t(setup.player_count);session.localPlayer=std::uint8_t(setup.local_player);
    Netplay::CoreConfig core;
    core.sessionId=session.sessionId;core.playerCount=session.playerCount;core.localPlayer=session.localPlayer;
    core.inputDelay=0;core.maxRollbackFrames=MaxRollbackFrames;
    core.predictableButtons=1|4|16|32|64|128;
    core.directionButtons=16|32|64|128;core.maxDirectionPredictionFrames=2;
    if(!gate_.Reset(session)||!core_.Reset(core))return false;
    setup_=validated;next_=0;correction_end_=Netplay::INVALID_FRAME;
    configured_=true;retired_=world_ready_=false;return true;
}
bool NetplayRuntime::Reset(const SessionSetup& setup) noexcept {
    // Validation precedes any mutation of an existing live session.
    if(ReadOnly()||setup.started||!setup.session_id||(configured_&&LastFrame()!=Netplay::INVALID_FRAME))return false;
    if(!configure(setup))return false;
    base_session_id_=setup.session_id;generation_=0;return true;
}
void NetplayRuntime::Clear() noexcept {
    gate_.Clear();core_.Clear();setup_={};base_session_id_=0;
    next_=generation_=0;correction_end_=Netplay::INVALID_FRAME;
    configured_=retired_=world_ready_=spectator_=playback_=false;
}
bool NetplayRuntime::MarkReady(){
    if(!configured_||retired_||ReadOnly()||!gate_.CanSendReady())return false;
    gate_.MarkLocalReady();return gate_.LocalReady();
}
bool NetplayRuntime::CaptureLocal(std::uint32_t frame,const Netplay::FrameInput& input){
    return !ReadOnly()&&CanStart()&&!Correcting()&&frame==next_&&ValidInput(input)&&core_.ScheduleLocalInput(frame,input);
}
bool NetplayRuntime::BeginSpectator(){
    if(!configured_||retired_||ReadOnly()||next_||generation_||setup_.local_player!=0||
       LastFrame()!=Netplay::INVALID_FRAME||core_.HasLocalCapture(0))return false;
    spectator_=true;world_ready_=false;return true;
}
bool NetplayRuntime::FeedSpectator(const Netplay::SpectatorFramePacket& packet){
    if(!spectator_||!CanStart()||packet.sessionId!=Config().sessionId||
       packet.gameplayAbi!=Config().gameplayAbi||packet.playerCount!=setup_.player_count||
       packet.frame!=next_||next_==Netplay::INVALID_FRAME||core_.HasLocalCapture(next_))return false;
    return FeedAuthoritative(packet.frame,packet.inputs);
}
bool NetplayRuntime::BeginPlayback(){
    if(!configured_||retired_||ReadOnly()||next_||generation_||
       LastFrame()!=Netplay::INVALID_FRAME||core_.HasLocalCapture(0))return false;
    playback_=true;world_ready_=false;return true;
}
bool NetplayRuntime::FeedPlayback(std::uint32_t frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){
    return playback_&&CanStart()&&frame==next_&&!core_.HasLocalCapture(frame)&&FeedAuthoritative(frame,inputs);
}
bool NetplayRuntime::FeedAuthoritative(std::uint32_t frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){
    if(frame==Netplay::INVALID_FRAME)return false;
    for(std::uint8_t seat=0;seat<setup_.player_count;++seat)if(!ValidInput(inputs[seat]))return false;
    auto candidate=core_;
    if(!candidate.ScheduleLocalInput(frame,inputs[setup_.local_player]))return false;
    for(std::uint8_t seat=0;seat<setup_.player_count;++seat){
        if(seat==setup_.local_player)continue;
        const auto result=candidate.SubmitRemoteInput(seat,frame,inputs[seat]);
        if(result!=Netplay::RemoteInputResult::Accepted&&result!=Netplay::RemoteInputResult::Duplicate)return false;
    }
    core_=candidate;return true;
}
bool NetplayRuntime::receive_frame(std::uint32_t frame)const {
    if(frame==Netplay::INVALID_FRAME)return false;
    // Bound the ring at its oldest unconfirmed input, not at the packet's
    // newest frame. A future packet must never overwrite a live missing slot.
    const auto confirmed=ConfirmedThrough();
    const auto first=confirmed==Netplay::INVALID_FRAME?0:confirmed+1;
    return frame<first||frame-first<Netplay::INPUT_HISTORY_SIZE-Netplay::MAX_REDUNDANT_INPUTS;
}
Netplay::RemoteInputResult NetplayRuntime::SubmitRemote(std::uint8_t seat,std::uint32_t frame,
                                                       const Netplay::FrameInput& input){
    if(ReadOnly()||!CanStart()||!receive_frame(frame)||!ValidInput(input))return Netplay::RemoteInputResult::InvalidPlayer;
    return core_.SubmitRemoteInput(seat,frame,input);
}
Netplay::FrameDecision NetplayRuntime::Prepare(std::uint32_t frame)const {
    if(!CanStart()||frame!=next_||frame==Netplay::INVALID_FRAME||
       (core_.HasRollbackRequest()&&!Correcting()))return {};
    const auto confirmed=ConfirmedThrough();
    const auto first=confirmed==Netplay::INVALID_FRAME?0:confirmed+1;
    // A later exact packet does not make an earlier gap recoverable. Bound the
    // entire pending interval even if the current frame itself is available.
    if(frame>=first&&(!world_ready_||frame-first>=MaxRollbackFrames))return {};
    return core_.PrepareFrame(frame);
}
bool NetplayRuntime::MarkSimulated(std::uint32_t frame,const Netplay::FrameDecision& decision){
    if(frame!=next_)return false;
    const auto expected=Prepare(frame);
    if(!expected.canAdvance||!decision.canAdvance||expected.predictedMask!=decision.predictedMask||
       expected.inputs!=decision.inputs)return false;
    if(!core_.MarkSimulated(frame,decision))return false;
    ++next_;return true;
}
bool NetplayRuntime::ConfirmedInputs(std::uint32_t frame,
    std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& out)const {
    if(!CanStart()||Correcting()||core_.HasRollbackRequest()||LastFrame()==Netplay::INVALID_FRAME||
       frame>LastFrame()||ConfirmedThrough()==Netplay::INVALID_FRAME||frame>ConfirmedThrough())return false;
    std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> result{};
    const auto exact=core_.PrepareFrame(frame);
    if(!exact.canAdvance||exact.predictedMask)return false;
    for(std::uint8_t seat=0;seat<setup_.player_count;++seat){
        if(!core_.UsedInput(seat,frame,&result[seat])||result[seat]!=exact.inputs[seat])return false;
    }
    out=result;return true;
}
bool NetplayRuntime::SetWorldReady(bool ready){
    if(ReadOnly()&&ready)return false;
    if(!configured_||Correcting()||core_.HasRollbackRequest())return false;
    if(!ready&&LastFrame()!=Netplay::INVALID_FRAME&&
       (ConfirmedThrough()==Netplay::INVALID_FRAME||ConfirmedThrough()<LastFrame()))return false;
    world_ready_=ready;return true;
}
bool NetplayRuntime::BeginCorrection(std::uint32_t first,std::uint32_t checkpointSpan){
    if(ReadOnly())return false;
    if(!CanStart()||!world_ready_||Correcting()||!core_.HasRollbackRequest()||
       checkpointSpan<1||checkpointSpan>3||first>core_.RollbackFrame()||
       core_.RollbackFrame()-first>=checkpointSpan||first>=next_||
       next_-core_.RollbackFrame()>MaxRollbackFrames||
       next_-first>MaxRollbackFrames+checkpointSpan-1)return false;
    const auto end=next_;
    if(!core_.RewindSimulationTo(first))return false;
    correction_end_=end;next_=first;return true;
}
bool NetplayRuntime::EndCorrection(bool lifecycle_boundary){
    if(!Correcting()||(!lifecycle_boundary&&next_!=correction_end_))return false;
    core_.ClearRollbackRequest();correction_end_=Netplay::INVALID_FRAME;return true;
}
bool NetplayRuntime::CanRetire()const {
    return CanStart()&&!Correcting()&&!core_.HasRollbackRequest()&&
        LastFrame()!=Netplay::INVALID_FRAME&&ConfirmedThrough()!=Netplay::INVALID_FRAME&&
        ConfirmedThrough()>=LastFrame();
}
bool NetplayRuntime::Retire(){
    if(!CanRetire())return false;
    retired_=true;world_ready_=false;return true;
}
bool NetplayRuntime::BeginNextRun(SessionSetup& out,std::uint32_t seed){
    if(spectator_||!retired_||!base_session_id_||seed>65535||generation_==0xffffffffu)return false;
    const auto generation=generation_+1;
    auto next=setup_;next.started=false;next.seed=seed;
    next.session_id=base_session_id_^(std::uint64_t(generation)*0x9e3779b97f4a7c15ull);
    if(!next.session_id||!configure(next))return false;
    generation_=generation;out=setup_;return true;
}
NetplayRuntime::WireResult NetplayRuntime::ApplyWire(const std::uint8_t* bytes,std::size_t size){
    if(!configured_||retired_||ReadOnly())return WireResult::IgnoredSession;
    Netplay::PacketType type;
    if(!Netplay::PeekPacketType(bytes,size,&type))return WireResult::Malformed;
    if(type==Netplay::PacketType::Session){
        Netplay::SessionPacket packet;
        if(!Netplay::DecodeSessionPacket(bytes,size,&packet))return WireResult::Malformed;
        if(packet.sessionId!=setup_.session_id)return WireResult::IgnoredSession;
        const auto result=gate_.Apply(packet);
        return result==Netplay::SessionPacketResult::Accepted||result==Netplay::SessionPacketResult::Duplicate?
            WireResult::Accepted:WireResult::Rejected;
    }
    if(type!=Netplay::PacketType::Input)return WireResult::Rejected;
    Netplay::InputPacket packet;
    if(!Netplay::DecodeInputPacket(bytes,size,&packet))return WireResult::Malformed;
    if(packet.sessionId!=setup_.session_id)return WireResult::IgnoredSession;
    if(!CanStart()||(packet.inputCount&&
       (!receive_frame(packet.latestFrame)||!receive_frame(packet.firstInputFrame))))return WireResult::Rejected;
    for(std::uint8_t i=0;i<packet.inputCount;++i)if(!ValidInput(packet.inputs[i]))return WireResult::Rejected;
    return core_.ApplyInputPacket(packet)?WireResult::Accepted:WireResult::Rejected;
}
bool NetplayRuntime::BuildInputWire(std::uint8_t peer,std::uint32_t frame,std::uint32_t sequence,
                                    std::uint32_t ack,std::vector<std::uint8_t>& out)const {
    if(ReadOnly()||!CanStart()||peer>=setup_.player_count||peer==setup_.local_player||!core_.HasLocalCapture(frame))return false;
    return Netplay::EncodeInputPacket(core_.BuildInputPacket(peer,frame,sequence,ack),&out);
}
}
