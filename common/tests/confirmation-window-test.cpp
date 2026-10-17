#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <iostream>

using namespace Netplay;

static void CheckGap(std::uint8_t players,std::uint8_t delayed,std::uint32_t missing){
    RollbackCore core;CoreConfig config{};
    config.sessionId=42;config.playerCount=players;config.localPlayer=0;
    config.maxRollbackFrames=12;assert(core.Reset(config));
    const auto last=missing+config.maxRollbackFrames-1;
    for(std::uint32_t frame=0;frame<=last;++frame){
        assert(core.ScheduleLocalInput(frame,FrameInput(1)));
        for(std::uint8_t seat=1;seat<players;++seat)
            if(seat!=delayed||frame!=missing)
                assert(core.SubmitRemoteInput(seat,frame,FrameInput(0x10))==RemoteInputResult::Accepted);
        const auto decision=core.PrepareFrame(frame);
        assert(decision.canAdvance&&core.MarkSimulated(frame,decision));
    }
    // Newer exact samples must not hide an older still-speculative frame.
    // A title retains the full interval from that gap for world/audio/Replay
    // correction, even though the next decision has no predicted lanes.
    for(std::uint32_t frame=last+1;frame<=last+8;++frame){
        assert(core.ScheduleLocalInput(frame,FrameInput(1)));
        for(std::uint8_t seat=1;seat<players;++seat)
            assert(core.SubmitRemoteInput(seat,frame,FrameInput(0x10))==RemoteInputResult::Accepted);
        for(int attempt=0;attempt<3;++attempt)
            assert(!core.PrepareFrame(frame).canAdvance);
        assert(core.LastSimulatedFrame()==last);
    }
    assert(core.ConfirmedThrough(delayed)==(missing?missing-1:INVALID_FRAME));
    assert(core.SubmitRemoteInput(delayed,missing,FrameInput(0x80))==RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame()==missing&&core.RewindSimulationTo(missing));
    for(std::uint32_t frame=missing;frame<=last+8;++frame){
        const auto decision=core.PrepareFrame(frame);
        assert(decision.canAdvance&&!decision.predictedMask);
        assert(core.MarkSimulated(frame,decision));
    }
    assert(core.ConfirmedThroughAllRemotes()==last+8);
    assert(!core.PrepareFrame(INVALID_FRAME).canAdvance);
}

int main(){
    CheckGap(2,1,0);CheckGap(2,1,19);
    CheckGap(3,1,0);CheckGap(3,2,19);
    CheckGap(3,2,INPUT_HISTORY_SIZE+5);
    std::cout<<"old confirmation gaps bound the entire rollback interval: PASS\n";
}
