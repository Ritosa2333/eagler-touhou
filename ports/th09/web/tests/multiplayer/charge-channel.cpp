#include "ChargeChannel.hpp"
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <vector>
using namespace th09;
using namespace th09::multiplayer;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();}}while(0)
using Batch=std::array<u8,ChargeHistory::MaxBatchBytes>;
using SamplePacket=std::array<u8,ChargeHistory::PacketBytes>;
static u32 word(const u8* p){return charge_record::word(p);}
static ChargeSample value(u32 side,u32 frame){ChargeSample s;s.active=1;s.charge=float((frame*3+side)%400);return s;}
struct Link:Netplay::PeerTransport {
    struct Packet{std::uint64_t due;std::vector<u8> bytes;};
    Link* peer=nullptr;std::deque<Packet> incoming;
    std::uint64_t now=0,controlUntil=0;
    unsigned mode=0,fast=0,repairs=0,rejected=0,dropped=0,bytes=0;
    bool IsOpen()const override{return true;}
    bool Failed()const override{return false;}
    std::size_t BufferedAmount()const override{return mode==5&&now<400?256*1024+1:0;}
    bool SendTo(u8,const u8* p,std::size_t n)override{
        CHECK(ChargeHistory::IsBatch(p,n));CHECK(n<=ChargeHistory::MaxBatchBytes);++fast;bytes+=unsigned(n);
        if(mode==1||(mode==2&&fast%5==0)||(mode==3&&word(p+20)==0)||
           (mode==4&&now>=400&&now<650)){++dropped;return true;}
        const auto delay=mode==2?3+(fast*17)%35:3;
        peer->incoming.push_back({now+delay,{p,p+n}});
        if(mode==2&&fast%7==0)peer->incoming.push_back({now+delay+30,{p,p+n}});
        return true;
    }
    bool SendRepairTo(u8,const u8* p,std::size_t n)override{
        CHECK(n<=ChargeHistory::MaxBatchBytes);++repairs;
        if(mode==5&&now<400){++rejected;return false;}
        peer->incoming.push_back({std::max(now+3,controlUntil),{p,p+n}});return true;
    }
    bool SendControl(const u8*,std::size_t)override{CHECK(false);return false;}
    bool Poll(std::vector<u8>* out)override{
        // New packets can overtake delayed/duplicated batches.
        for(auto it=incoming.begin();it!=incoming.end();++it)if(it->due<=now){
            *out=std::move(it->bytes);incoming.erase(it);return true;
        }return false;
    }
};
static void codec(){
    ChargeHistory a,b;a.begin(7,0,true);b.begin(7,1,true);
    SamplePacket sample{};Batch packet{};std::size_t size=0;
    for(u32 f=0;f<9;++f)CHECK(a.Publish(f,value(0,f),sample));
    CHECK(a.Batch(packet,size)&&size==376&&word(packet.data()+20)==8);
    CHECK(word(packet.data()+24)==0&&word(packet.data()+24+7*44)==8);
    // Missing frame 7 is covered as soon as the first seven are acknowledged.
    bool accepted=false,data=false;
    CHECK(b.ReceiveBatch(packet.data(),size,8,accepted,data)&&accepted&&data&&b.RemoteCount()==7);
    Batch ack{};std::size_t ackSize=0;CHECK(b.Batch(ack,ackSize)&&ackSize==24);
    CHECK(a.ReceiveBatch(ack.data(),ackSize,9,accepted,data)&&accepted&&!data);
    CHECK(a.Batch(packet,size)&&word(packet.data()+24)==7);
    CHECK(b.ReceiveBatch(packet.data(),size,8,accepted,data)&&b.RemoteCount()==9);
    auto malformed=packet;charge_record::word(malformed.data()+20,9);
    CHECK(!b.ReceiveBatch(malformed.data(),size,8,accepted,data));
    CHECK(!b.ReceiveBatch(packet.data(),size-1,8,accepted,data));
    malformed=ack;charge_record::word(malformed.data()+16,10);
    CHECK(!a.ReceiveBatch(malformed.data(),ackSize,9,accepted,data));
    malformed=packet;malformed[4]^=1;
    CHECK(b.ReceiveBatch(malformed.data(),size,8,accepted,data)&&!accepted);
    malformed=packet;charge_record::real(malformed.data()+28,399);
    CHECK(!b.ReceiveBatch(malformed.data(),size,8,accepted,data));
    ChargeHistory retained;retained.begin(8,0,true);
    for(u32 f=0;f<512;++f)CHECK(retained.Publish(f,value(0,f),sample));
    CHECK(!retained.Publish(512,value(0,512),sample)&&retained.Get(0,0));
}
static void run(unsigned mode,u32 limit,bool oneWay=false){
    Link links[2];links[0].peer=&links[1];links[1].peer=&links[0];
    for(auto& link:links){link.mode=mode;link.controlUntil=mode==6?2000:0;}
    ChargeHistory histories[2];ChargeChannel channels[2];
    for(u32 side=0;side<2;++side){histories[side].begin(91,side,true);channels[side].Begin(side,0);}
    bool completed=false;unsigned waits=0;
    for(u32 tick=0;tick<limit*30+1000;++tick){
        for(auto& link:links)link.now=std::uint64_t(tick)*16;
        for(u32 side=0;side<2;++side){
            std::vector<u8> bytes;
            while(links[side].Poll(&bytes)){
                bool handled=false;CHECK(channels[side].Receive(histories[side],bytes.data(),bytes.size(),histories[side].LocalCount(),handled)&&handled);
            }
            CHECK(channels[side].Pump(links[side],histories[side],links[side].now));
            const auto frame=histories[side].LocalCount();
            if(frame<limit&&(!oneWay||side==0)){
                if(oneWay||histories[side].CanAdvance(frame,8)){
                    SamplePacket packet{};CHECK(histories[side].Publish(frame,value(side,frame),packet));
                    CHECK(channels[side].Published(links[side],histories[side],links[side].now));
                }else ++waits;
            }
        }
        completed=histories[0].AcknowledgedCount()==limit&&histories[1].RemoteCount()==limit&&
            (oneWay||(histories[1].AcknowledgedCount()==limit&&histories[0].RemoteCount()==limit));
        if(completed)break;
    }
    CHECK(completed);
    for(u32 side=0;side<2;++side)if(!oneWay||side==1){
        CHECK(histories[side].RemoteCount()==limit);
        for(u32 f=limit>400?limit-400:0;f<limit;++f)CHECK(*histories[side].Get(1-side,f)==value(1-side,f));
    }
    if(mode==0||mode==6)CHECK(waits==0);
    if(mode==1)CHECK(links[0].repairs&&links[1].repairs);
    if(mode==3){CHECK(links[1].dropped&&links[1].repairs);}
    if(mode==4)CHECK(waits&&links[0].dropped&&links[1].dropped);
    if(mode==5)CHECK(waits&&links[0].rejected&&links[1].rejected);
    std::printf("charge lane mode=%u frames=%u oneWay=%u waits=%u fast=%u/%u repair=%u/%u PASS\n",
        mode,limit,unsigned(oneWay),waits,links[0].fast,links[1].fast,links[0].repairs,links[1].repairs);
}
int main(){
    codec();
    run(0,72000); // Twenty minutes, beyond record-ring reuse.
    run(1,300); // Entire fast lane lost, including ACK-only tails.
    run(2,1200); // Loss, duplicates and reordering on both endpoints.
    run(3,300,true); // Input arrived; every fast ACK is lost.
    run(4,1200); // A burst exceeds the world's prediction horizon.
    run(5,300); // Bounded repair refusal and ordinary backpressure recover.
    run(6,1200); // Reliable stream held; fast records continue normally.
    std::puts("charge batch codec | immutable samples | oldest-hole recovery | ACK retirement | bounded retries PASS");
}
