#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <iostream>
using namespace Netplay;

void directionHorizon(unsigned players, unsigned remote) {
    CoreConfig config; config.playerCount=players; config.localPlayer=0; config.maxRollbackFrames=8;
    config.predictableButtons=15; config.directionButtons=15; config.maxDirectionPredictionFrames=2;
    RollbackCore core; assert(core.Reset(config));
    for(unsigned slot=1;slot<players;slot++)
        assert(core.SubmitRemoteInput(slot,0,FrameInput(slot==remote?8:0))==RemoteInputResult::Accepted);
    for(unsigned frame=0;frame<4;frame++) {
        assert(core.ScheduleLocalInput(frame,FrameInput(0)));
        auto decision=core.PrepareFrame(frame); assert(decision.canAdvance);
        if(frame==3) assert(decision.inputs[remote].buttons==0);
        assert(core.MarkSimulated(frame,decision));
    }
    assert(core.SubmitRemoteInput(remote,1,FrameInput(8))==RemoteInputResult::RollbackRequired);
    assert(core.HasRollbackRequest() && core.RollbackFrame()==3);
    assert(core.RewindSimulationTo(3));
    auto replay=core.PrepareFrame(3); assert(replay.canAdvance && replay.inputs[remote].buttons==8);
}

void matchingTouchDelta() {
    CoreConfig config; config.playerCount=2; config.localPlayer=0; config.maxRollbackFrames=8;
    RollbackCore core; assert(core.Reset(config));
    FrameInput swipe; swipe.analogMode=AnalogMode::DirectTouchDelta; swipe.x=20;
    assert(core.SubmitRemoteInput(1,0,swipe)==RemoteInputResult::Accepted);
    for(unsigned frame=0;frame<3;frame++) {
        assert(core.ScheduleLocalInput(frame,FrameInput(0)));
        auto decision=core.PrepareFrame(frame); assert(decision.canAdvance);
        if(frame>0) assert(decision.inputs[1].x==0);
        assert(core.MarkSimulated(frame,decision));
    }
    FrameInput noMovement; noMovement.analogMode=AnalogMode::DirectTouchDelta;
    assert(core.SubmitRemoteInput(1,1,noMovement)==RemoteInputResult::PredictionCorrect);
    assert(!core.HasRollbackRequest());
}
void departedLane() {
    CoreConfig config; config.playerCount=3; config.localPlayer=0; config.maxRollbackFrames=8;
    RollbackCore core; assert(core.Reset(config));
    for(unsigned slot=1;slot<3;slot++) for(unsigned frame=0;frame<5;frame++)
        assert(core.SubmitRemoteInput(slot,frame,FrameInput(slot==2?8:0))==RemoteInputResult::Accepted);
    for(unsigned frame=0;frame<3;frame++) { assert(core.ScheduleLocalInput(frame,FrameInput(0)));auto row=core.PrepareFrame(frame);assert(row.canAdvance);assert(core.MarkSimulated(frame,row)); }
    assert(core.RetireRemoteFrom(2,3));
    for(unsigned frame=3;frame<6;frame++) {
        assert(core.ScheduleLocalInput(frame,FrameInput(0)));
        assert(core.SubmitRemoteInput(2,frame,FrameInput(0))==RemoteInputResult::Accepted);
        auto row=core.PrepareFrame(frame);assert(row.canAdvance && row.inputs[2].buttons==0);assert(core.MarkSimulated(frame,row));
    }
}
int main() {
    departedLane();
    directionHorizon(2,1); directionHorizon(3,1); directionHorizon(3,2); matchingTouchDelta();
    std::cout << "PASS PC-98 prediction frontier and touch delta\n";
}
