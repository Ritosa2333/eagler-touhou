#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <cstring>
#include <limits>
#include <cstdio>
#include <type_traits>

using namespace Netplay;
static_assert(std::is_trivially_copyable_v<RollbackCore>);

static void unchangedAfterRejection(RollbackCore& core,const InputPacket& packet)
{
    unsigned char before[sizeof(core)];std::memcpy(before,&core,sizeof(core));
    assert(!core.ApplyInputPacket(packet));
    assert(std::memcmp(before,&core,sizeof(core))==0);
}

int main()
{
    RollbackCore core;CoreConfig config{};config.sessionId=75;config.maxRollbackFrames=12;
    assert(core.Reset(config));
    for(std::uint32_t f=0;f<3;++f)assert(core.ScheduleLocalInput(f,FrameInput(1)));
    assert(core.SubmitRemoteInput(1,1,FrameInput(2))==RemoteInputResult::Accepted);
    InputPacket packet{};packet.sessionId=75;packet.senderPlayer=1;packet.playerCount=2;
    packet.firstInputFrame=0;packet.latestFrame=1;packet.inputCount=2;packet.ackFrame=2;
    packet.inputs[0]=FrameInput(4);packet.inputs[1]=FrameInput(99);
    unchangedAfterRejection(core,packet);
    assert(!core.InputPresent(1,0));assert(core.AcknowledgedLocalThrough(1)==INVALID_FRAME);
    packet.inputs[1]=FrameInput(2);assert(core.ApplyInputPacket(packet));
    assert(core.ConfirmedThrough(1)==1&&core.AcknowledgedLocalThrough(1)==2);
    assert(core.MarkSimulated(0,core.PrepareFrame(0)));
    packet.ackFrame=3;unchangedAfterRejection(core,packet);packet.ackFrame=2;
    packet.inputs[1].x=std::numeric_limits<float>::infinity();unchangedAfterRejection(core,packet);
    packet.inputs[1].x=0;packet.inputs[1].analogMode=static_cast<AnalogMode>(255);
    unchangedAfterRejection(core,packet);packet.inputs[1]=FrameInput(2);
    packet.inputCount=1;
    packet.firstInputFrame=packet.latestFrame=257;unchangedAfterRejection(core,packet);
    packet.firstInputFrame=packet.latestFrame=256;unchangedAfterRejection(core,packet);
    packet.firstInputFrame=packet.latestFrame=INVALID_FRAME;unchangedAfterRejection(core,packet);
    packet.firstInputFrame=packet.latestFrame=2;packet.inputs[0]=FrameInput(3);
    assert(core.ApplyInputPacket(packet));assert(core.ConfirmedThrough(1)==2);

    assert(core.Reset(config));
    for(std::uint32_t f=0;f<INPUT_HISTORY_SIZE+4;++f){
        assert(core.ScheduleLocalInput(f,FrameInput(1)));
        assert(core.SubmitRemoteInput(1,f,FrameInput(2))==RemoteInputResult::Accepted);
        assert(core.MarkSimulated(f,core.PrepareFrame(f)));
    }
    packet.firstInputFrame=packet.latestFrame=0;packet.ackFrame=100;packet.inputs[0]=FrameInput(2);
    RemoteInputResult result{};assert(core.ApplyInputPacket(packet,&result));
    assert(result==RemoteInputResult::TooOld);
    assert(core.InputPresent(1,INPUT_HISTORY_SIZE));
    assert(core.ConfirmedThrough(1)==INPUT_HISTORY_SIZE+3);
    std::puts("eagler-common packet transaction: PASS");
}
