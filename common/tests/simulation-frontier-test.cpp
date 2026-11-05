#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace Netplay;

namespace {
std::vector<std::uint8_t> packet(const RollbackCore& core,std::uint32_t last){
    std::vector<std::uint8_t> bytes;
    assert(EncodeInputPacket(core.BuildInputPacket(1,last,19,7),&bytes));
    return bytes;
}

void CheckPreservedInputHistory(std::uint8_t players){
    RollbackCore core;CoreConfig config{};
    config.sessionId=42;config.playerCount=players;config.maxRollbackFrames=12;
    assert(core.Reset(config));
    for(std::uint32_t frame=0;frame<9;++frame){
        FrameInput local{std::uint16_t(frame)};
        local.analogMode=AnalogMode::DirectTouchDelta;
        local.x=float(frame)*0.25f;local.touchUsed=true;
        assert(core.ScheduleLocalInput(frame,local));
        for(std::uint8_t peer=1;peer<players;++peer)
            if(frame<4)assert(core.SubmitRemoteInput(peer,frame,FrameInput(peer))==RemoteInputResult::Accepted);
        const auto decision=core.PrepareFrame(frame);
        assert(decision.canAdvance&&core.MarkSimulated(frame,decision));
    }
    // Peer has acknowledged a prefix of our already captured input stream.
    InputPacket ack{};ack.sessionId=42;ack.senderPlayer=1;ack.playerCount=players;
    ack.ackFrame=2;assert(core.ApplyInputPacket(ack));
    assert(core.AcknowledgedLocalThrough(1)==2);
    assert(core.AcknowledgedLocalThrough(0)==INVALID_FRAME);
    assert(core.AcknowledgedLocalThrough(3)==INVALID_FRAME);
    assert(core.AcknowledgedLocalThroughAllRemotes()==(players==2?2:INVALID_FRAME));
    if(players==3){
        ack.senderPlayer=2;ack.ackFrame=1;assert(core.ApplyInputPacket(ack));
        assert(core.AcknowledgedLocalThroughAllRemotes()==1);
    }
    assert(core.SubmitRemoteInput(1,4,FrameInput(8))==RemoteInputResult::RollbackRequired);
    const auto wire=packet(core,8);
    assert(!core.RewindSimulationTo(5)); // cannot skip first divergent frame
    assert(core.LastSimulatedFrame()==8&&core.RollbackFrame()==4);
    assert(core.RewindSimulationTo(4));
    assert(core.LastSimulatedFrame()==3&&!core.HasRollbackRequest());
    FrameInput used{};assert(core.UsedInput(1,3,&used));
    for(std::uint32_t frame=4;frame<9;++frame){
        assert(!core.UsedInput(1,frame,&used));
        assert(core.HasLocalCapture(frame));
        bool present=false;const auto saved=core.LocalInput(frame,&present);
        assert(present&&saved.x==float(frame)*0.25f&&saved.touchUsed);
        assert(!core.ScheduleLocalInput(frame,FrameInput(65535)));
    }
    assert(core.ConfirmedThrough(1)==4);
    assert(packet(core,8)==wire); // ACK/capture history unchanged by rewind
    assert(core.AcknowledgedLocalThroughAllRemotes()==(players==2?2:1));
    // Samples for the abandoned future are now confirmed input, not a false
    // rollback request for frames which no longer exist in the simulation.
    assert(core.SubmitRemoteInput(1,5,FrameInput(9))==RemoteInputResult::Accepted);
    assert(!core.HasRollbackRequest());
    for(std::uint32_t frame=4;frame<7;++frame){
        const auto decision=core.PrepareFrame(frame);
        assert(decision.canAdvance&&core.MarkSimulated(frame,decision));
    }
    assert(core.LastSimulatedFrame()==6); // old frame 8 is not phantom progress
    assert(!core.RewindSimulationTo(7));
    assert(core.RewindSimulationTo(0));
    assert(core.LastSimulatedFrame()==INVALID_FRAME);
    assert(core.HasLocalCapture(0)&&core.HasLocalCapture(8));
    assert(core.ConfirmedThrough(1)==5);
}

void CheckFailureAndRingBoundaries(){
    RollbackCore core;assert(!core.RewindSimulationTo(0));
    CoreConfig config{};config.sessionId=10;assert(core.Reset(config));
    assert(!core.RewindSimulationTo(0));
    for(std::uint32_t frame=0;frame<INPUT_HISTORY_SIZE+20;++frame){
        assert(core.ScheduleLocalInput(frame,FrameInput(1)));
        assert(core.SubmitRemoteInput(1,frame,FrameInput(2))==RemoteInputResult::Accepted);
        assert(core.MarkSimulated(frame,core.PrepareFrame(frame)));
    }
    const auto last=core.LastSimulatedFrame();
    const auto wire=packet(core,last);
    assert(!core.RewindSimulationTo(INVALID_FRAME));
    assert(!core.RewindSimulationTo(last+1));
    assert(!core.RewindSimulationTo(last-INPUT_HISTORY_SIZE));
    assert(core.LastSimulatedFrame()==last&&packet(core,last)==wire);
    assert(core.RewindSimulationTo(last-INPUT_HISTORY_SIZE+1));
    assert(core.LastSimulatedFrame()==last-INPUT_HISTORY_SIZE);

    assert(core.Reset(config));
    for(std::uint32_t frame=0;frame<3;++frame){
        assert(core.ScheduleLocalInput(frame,FrameInput(1)));
        assert(core.SubmitRemoteInput(1,frame,FrameInput(2))==RemoteInputResult::Accepted);
        if(frame!=1)assert(core.MarkSimulated(frame,core.PrepareFrame(frame)));
    }
    // Hole inside the requested simulated interval: reject transactionally.
    assert(!core.RewindSimulationTo(0));
    FrameInput used{};assert(core.LastSimulatedFrame()==2&&core.UsedInput(1,2,&used));
}
}

int main(){
    CheckPreservedInputHistory(2);CheckPreservedInputHistory(3);
    CheckFailureAndRingBoundaries();
    std::cout<<"eagler-common simulation frontier: PASS\n";
}
