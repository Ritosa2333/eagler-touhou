#include "../../cpp/multiplayer/ChargeHistory.hpp"
#include "../../cpp/game/ChargeReplayTrack.hpp"
#include "../../cpp/game/TouchMotionTrack.hpp"
#include "../../cpp/game/Lzss.hpp"
#include <memory>
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
    ChargeReplayTrack longRun;
    for(u32 frame=0;frame<72000;++frame){
        std::array<ChargeSample,2> values{};
        for(u32 side=0;side<2;++side){auto& v=values[side];const u32 tick=frame%(side?237:181);v.active=1;
            v.charge=float(tick)*1.125f;v.charging_time={i32(tick)-1,float(tick)+.25f,i32(tick)};
            v.full_charge={i32(frame)-1,float(frame),i32(frame)};
            if(tick==0){v.events=ChargeSample::End;v.released=200;}
        }
        CHECK(longRun.record(frame/24000,frame%24000,false,values));
    }
    std::vector<u8> rows(40+72000*88);u32 offset=40;
    for(u32 stage=0;stage<10;++stage){charge_record::word(rows.data()+stage*4,u32(longRun.stages[stage].size()));
        for(const auto& e:longRun.stages[stage]){auto* at=rows.data()+offset;charge_record::word(at,e.frame);charge_record::word(at+4,e.hold);
            charge_record::encode(e.samples[0],at+8);charge_record::encode(e.samples[1],at+48);offset+=88;
        }
    }
    auto codec=std::make_unique<Lzss>();auto packed=codec->encode(rows.data(),u32(rows.size()));
    std::vector<u8> legacy(4+packed.size()+24);charge_record::word(legacy.data(),u32(rows.size()));std::copy(packed.begin(),packed.end(),legacy.begin()+4);
    auto* footer=legacy.data()+legacy.size()-24;std::memcpy(footer,"T9CHARGE",8);charge_record::word(footer+8,9);charge_record::word(footer+12,1);
    const u32 length=u32(legacy.size())-24;charge_record::word(footer+16,length);charge_record::word(footer+20,touch::MotionTrack::hash(legacy.data(),length));
    auto compact=longRun.trailer();CHECK(compact.size()<legacy.size()/10);
    for(const auto* bytes:{&legacy,&compact}){ChargeReplayTrack loaded;CHECK(loaded.load(bytes->data(),u32(bytes->size())));
        CHECK(loaded.trailer()==compact);
        for(u32 stage=0;stage<10;++stage){CHECK(loaded.stages[stage].size()==longRun.stages[stage].size());
            for(u32 i=0;i<loaded.stages[stage].size();++i){const auto& a=loaded.stages[stage][i];const auto& b=longRun.stages[stage][i];CHECK(a.frame==b.frame&&a.hold==b.hold&&a.samples==b.samples);}
        }
    }
    for(const std::vector<u8> invalidRuns:{std::vector<u8>{0,41},std::vector<u8>{0,0},std::vector<u8>{0,128},std::vector<u8>{0,39}}){
        auto badPacked=codec->encode(invalidRuns.data(),u32(invalidRuns.size()));std::vector<u8> bad(8+badPacked.size()+24);
        charge_record::word(bad.data(),40);charge_record::word(bad.data()+4,u32(invalidRuns.size()));std::copy(badPacked.begin(),badPacked.end(),bad.begin()+8);
        auto* end=bad.data()+bad.size()-24;std::memcpy(end,"T9CHARGE",8);charge_record::word(end+8,9);charge_record::word(end+12,2);
        const u32 n=u32(bad.size())-24;charge_record::word(end+16,n);charge_record::word(end+20,touch::MotionTrack::hash(bad.data(),n));
        ChargeReplayTrack invalid;CHECK(!invalid.load(bad.data(),u32(bad.size())));
    }
    std::printf("20-minute charge track | old %zu bytes | new %zu bytes\n",legacy.size(),compact.size());
    std::puts("Charge history | immutable samples | correction | ordered confirmation | ACK | bounded ring | mod Replay | touch coexistence: PASS");
}
