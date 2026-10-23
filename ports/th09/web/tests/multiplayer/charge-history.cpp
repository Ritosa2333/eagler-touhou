#include "../../cpp/multiplayer/ChargeHistory.hpp"
#include "../../cpp/game/ChargeReplayTrack.hpp"
#include "../../cpp/game/TouchMotionTrack.hpp"
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();}}while(0)
using namespace th09;
int main(){
    using multiplayer::ChargeHistory;
    ChargeHistory left,right;left.begin(0x123456789ULL,0,true);right.begin(0x123456789ULL,1,true);
    ChargeSample a,b;a.active=b.active=1;a.charge=123.5f;b.charge=210;
    a.charging_time={99,100,100};a.full_charge={0,0,0};
    std::array<u8,40> encoded{};ChargeHistory::EncodeSample(a,encoded.data());ChargeSample decoded;
    CHECK(ChargeHistory::DecodeSample(encoded.data(),decoded)&&decoded==a);
    std::array<u8,ChargeHistory::PacketBytes> p0{},p1{};
    CHECK(left.Publish(0,a,p0));CHECK(right.Publish(0,b,p1));CHECK(!left.Publish(0,a,p0));
    CHECK(left.Applied(0,{a,a}));CHECK(right.Applied(0,{b,b}));
    std::array<u8,ChargeHistory::AckBytes> ack{};bool accepted=false;
    CHECK(left.Receive(p1.data(),p1.size(),1,ack,accepted)&&accepted);CHECK(right.ReceiveAck(ack.data(),ack.size()));
    CHECK(left.RollbackFrame()==0);left.Restored(0);CHECK(left.Applied(0,{a,b}));
    CHECK(right.Receive(p0.data(),p0.size(),1,ack,accepted)&&accepted);CHECK(left.ReceiveAck(ack.data(),ack.size()));
    CHECK(left.ConfirmedCount()==1&&right.ConfirmedCount()==1&&left.CanRetire(1));
    CHECK(left.CanAdvance(8,8)&&!left.CanAdvance(9,8));
    CHECK(left.Receive(p1.data(),p1.size(),1,ack,accepted));
    auto forged=p1;charge_record::real(forged.data()+20,300);CHECK(!left.Receive(forged.data(),forged.size(),1,ack,accepted));
    forged=p1;charge_record::word(forged.data()+12,600);CHECK(!left.Receive(forged.data(),forged.size(),1,ack,accepted));
    auto stale=p1;stale[4]^=1;CHECK(left.Receive(stale.data(),stale.size(),1,ack,accepted)&&!accepted);
    ChargeHistory disabled;disabled.begin(1,0,false);CHECK(!disabled.Publish(0,a,p0)&&disabled.CanAdvance(5000,8));
    {
        ChargeHistory recovering,sender;recovering.begin(77,0,true);sender.begin(77,1,true);
        CHECK(recovering.Applied(1,{a,a}));recovering.Restored(0);
        CHECK(sender.Publish(1,b,p1));CHECK(recovering.Receive(p1.data(),p1.size(),0,ack,accepted));
        CHECK(recovering.RollbackFrame()==Netplay::INVALID_FRAME);
        std::array<u8,ChargeHistory::PacketBytes> repeated{};CHECK(sender.Repeat(1,repeated)&&repeated==p1);
    }
    // Input and metadata acknowledgements have independent ownership. Reusing
    // the bounded ring cannot turn old results into a new accepted frame.
    for(u32 frame=1;frame<1200;++frame){
        a.charge=float(frame%399);b.charge=float((frame*3)%399);
        CHECK(left.Publish(frame,a,p0)&&right.Publish(frame,b,p1));
        CHECK(left.Receive(p1.data(),p1.size(),frame,ack,accepted)&&right.ReceiveAck(ack.data(),ack.size()));
        CHECK(right.Receive(p0.data(),p0.size(),frame,ack,accepted)&&left.ReceiveAck(ack.data(),ack.size()));
        CHECK(left.Applied(frame,{a,b})&&right.Applied(frame,{a,b}));
        CHECK(left.CanRetire(frame+1)&&right.CanRetire(frame+1));
    }
    ChargeReplayTrack recording;std::array<ChargeSample,2> samples{a,b};
    CHECK(recording.record(9,0,true,samples));CHECK(recording.record(9,0,false,samples));
    samples[0].charge=0;samples[0].released=200;samples[0].events=ChargeSample::End;
    CHECK(recording.record(9,1,false,samples));
    const auto trailer=recording.trailer();CHECK(trailer.size()>24);
    std::vector<u8> file(512);file.insert(file.end(),trailer.begin(),trailer.end());
    ChargeReplayTrack playback;CHECK(playback.load(file.data(),u32(file.size()))&&playback.enabled&&playback.playing);
    CHECK(playback.stages[9].size()==3&&playback.stages[9][2].samples==samples);
    CHECK(playback.peek(9)->hold==1);playback.pop(9);CHECK(playback.peek(9)->hold==0);
    CHECK(playback.trailer()==trailer);
    touch::MotionTrack movement;movement.begin(18,true,false,true);movement.record(18,true,2,3);
    const auto motion=movement.trailer(9);file.insert(file.end(),motion.begin(),motion.end());
    CHECK(playback.load(file.data(),u32(file.size()))&&playback.enabled&&playback.stages[9].size()==3);
    auto broken=trailer;broken[5]^=1;CHECK(!playback.load(broken.data(),u32(broken.size())));
    CHECK(!recording.record(9,1,false,samples));
    ChargeReplayTrack ordinary;std::array<u8,50> native{};CHECK(ordinary.load(native.data(),native.size())&&!ordinary.enabled&&ordinary.trailer().empty());
    std::puts("Charge history | immutable samples | correction | ordered confirmation | ACK | bounded ring | mod Replay | touch coexistence: PASS");
}
