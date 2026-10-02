#include <eagler/netplay/AdonisStartup.hpp>
#include <cassert>
#include <cstdio>
#include <deque>

using namespace Netplay;
struct Link : PeerTransport {
    struct Packet {std::uint64_t due;std::vector<std::uint8_t> bytes;};
    Link* other=nullptr;
    std::deque<Packet> incoming;
    std::uint64_t now=0,delay=35'000;
    unsigned fast=0,control=0,dropEvery=0;
    bool dead=false,blocked=false,dropAccept=false,dropCommit=false,dropAck=false;
    bool IsOpen()const override{return !dead;}
    bool Failed()const override{return dead;}
    std::size_t BufferedAmount()const override{return 0;}
    bool Send(const std::uint8_t* bytes,std::size_t size,bool reliable){
        if(dead||blocked)return false;
        assert(size==64 && AdonisStartup::IsPacket(bytes,size));
        if(reliable){
            ++control;
            bool* once=bytes[4]==6?&dropAccept:bytes[4]==7?&dropCommit:bytes[4]==8?&dropAck:nullptr;
            if(once&&*once){*once=false;return true;}
        }else if(++fast && dropEvery && fast%dropEvery==0)return true;
        other->incoming.push_back({now+(reliable?1'000:delay),{bytes,bytes+size}});return true;
    }
    bool SendTo(std::uint8_t,const std::uint8_t* p,std::size_t n)override{return Send(p,n,false);}
    bool SendRepairTo(std::uint8_t,const std::uint8_t*,std::size_t)override{assert(false);return false;}
    bool SendControl(const std::uint8_t* p,std::size_t n)override{return Send(p,n,true);}
    bool Poll(std::vector<std::uint8_t>* out)override{
        for(auto i=incoming.begin();i!=incoming.end();++i)if(i->due<=now){*out=std::move(i->bytes);incoming.erase(i);return true;}
        return false;
    }
};
SessionConfig config(unsigned side){SessionConfig c;c.sessionId=0x901;c.gameplayAbi=42;c.gameId=9;c.seed=314;c.playerCount=2;c.localPlayer=side;return c;}
static bool pump(AdonisStartup& s,Link& l,std::uint64_t now){
    l.now=now;std::vector<std::uint8_t> wire;
    for(unsigned n=0;n<256&&l.Poll(&wire);++n)if(!s.Receive(l,wire.data(),wire.size(),now))return false;
    return s.Tick(l,now);
}
static AdonisStartup::Choice run(AdonisMode mode,std::uint32_t request,unsigned reserve=2,
                                unsigned loss=0,bool controls=false,bool asymmetric=false){
    Link a,b;a.other=&b;b.other=&a;a.dropEvery=b.dropEvery=loss;
    if(asymmetric){a.delay=5'000;b.delay=65'000;}
    if(controls){a.dropCommit=true;b.dropAccept=true;b.dropAck=true;}
    AdonisStartup left,right;assert(left.Begin(config(0),0,mode,request,reserve));assert(right.Begin(config(1),0,mode,request,reserve));
    unsigned progress=0;
    for(std::uint64_t t=0;t<10'000'000;t+=1'000){
        a.now=b.now=t;assert(pump(left,a,t)&&pump(right,b,t));
        // Measurement never commits early or invents a value from no samples.
        if(left.Ready()||right.Ready())assert(left.Probes()==130&&right.Probes()==130);
        if(left.Ready()&&right.Ready()){
            if(++progress==800)break; // delayed/duplicate commit and echo packets survive handoff
        }
    }
    assert(left.Ready()&&right.Ready());
    auto l=left.Selected(),r=right.Selected();assert(l.delay==r.delay&&l.fullDelay==r.fullDelay&&l.prediction==r.prediction);
    assert(left.Local().received+left.Local().lost==120&&right.Local().received+right.Local().lost==120);
    assert(a.fast>=130&&b.fast>=130&&a.control&&b.control);
    assert(l.delay==(request==AdonisStartup::Automatic?l.fullDelay-l.prediction:request));
    assert(l.prediction==(mode==AdonisMode::Hybrid?reserve:0));
    std::printf("startup mode=%u requested=%u reserve=%u loss=%u asymmetric=%u -> B=%u D=%u P=%u PASS\n",
        unsigned(mode),request,reserve,loss,unsigned(asymmetric),l.fullDelay,l.delay,l.prediction);
    return l;
}
static void mismatches(){
    for(unsigned kind=0;kind<5;++kind){
        Link a,b;a.other=&b;b.other=&a;AdonisStartup l,r;auto c=config(1);
        if(kind==3)c.gameplayAbi++;if(kind==4)c.seed++;
        assert(l.Begin(config(0),0,AdonisMode::Hybrid,AdonisStartup::Automatic));
        assert(r.Begin(c,0,kind==0?AdonisMode::Delay:AdonisMode::Hybrid,kind==1?3:AdonisStartup::Automatic,kind==2?1:2));
        bool failed=false;
        for(unsigned t=0;t<1'000'000&&!failed;t+=1000){a.now=b.now=t;failed=!pump(l,a,t)||!pump(r,b,t);}
        assert(failed&&!l.Ready()&&!r.Ready());
    }
}
static void failure_and_reuse(){
    Link a,b;a.other=&b;b.other=&a;AdonisStartup l;assert(l.Begin(config(0),0,AdonisMode::Delay,AdonisStartup::Automatic));
    assert(l.Tick(a,0));const auto stale=b.incoming.front().bytes;
    assert(!l.Tick(a,10'000'000)&&l.Failed());
    auto c=config(1);c.sessionId++;
    assert(l.Begin(c,0,AdonisMode::Hybrid,AdonisStartup::Automatic));
    assert(l.Receive(b,stale.data(),stale.size(),1'000)&&!l.Ready());
    auto bad=stale;bad[8]=std::uint8_t(c.sessionId);bad[5]=0;bad[6]=2;bad[7]=2;
    assert(!l.Receive(b,bad.data(),bad.size()-1,2'000));
    assert(l.Begin(c,0,AdonisMode::Hybrid,AdonisStartup::Automatic));
    assert(l.Receive(b,bad.data(),bad.size(),1'000));assert(l.Tick(b,1'000));
    assert(!l.Tick(b,601'000)&&l.Failed());
    assert(l.Begin(c,0,AdonisMode::Hybrid,AdonisStartup::Automatic));
    assert(!l.Ready()&&l.Probes()==0&&l.Local().received==0);
}
static void allowance(){
    for(unsigned p:{0u,1u,2u}){
        AdonisPhase phase;assert(phase.Reset(90,0,2,p));
        for(unsigned f=0;f<80;++f){const auto due=1'000'000+f*16667ull;phase.ObserveDue(f,due);phase.ObserveArrival(1,f,due+20'000);AdonisPhaseSample sample;phase.PollLocalSample(&sample);}
        const auto expected=p==2?0.0:p==1?3.333:8.0;
        assert(std::abs(phase.TakeDelayMs()-expected)<.001);
    }
}
int main(){
    const auto pure=run(AdonisMode::Delay,AdonisStartup::Automatic);
    const auto hybrid=run(AdonisMode::Hybrid,AdonisStartup::Automatic);
    assert(pure.fullDelay==hybrid.fullDelay&&pure.delay==hybrid.delay+2);
    run(AdonisMode::Hybrid,AdonisStartup::Automatic,1,23,true,true);
    run(AdonisMode::Delay,AdonisStartup::Automatic,2,31,true);
    for(unsigned d:{0u,1u,9u})for(auto m:{AdonisMode::Delay,AdonisMode::Hybrid})run(m,d);
    mismatches();allowance();failure_and_reuse();
    Link a,b;a.other=&b;b.other=&a;AdonisStartup s;
    assert(!s.Begin(config(0),0,AdonisMode::Hybrid,10));
    assert(!s.Begin(config(0),0,AdonisMode::Hybrid,0,3));
    assert(s.Begin(config(0),1,AdonisMode::Delay,0));assert(!s.Tick(a,0));
    assert(s.Begin(config(0),0,AdonisMode::Delay,0));assert(!s.Tick(a,10'000'000));
    std::puts("Actual input lane, two-sided immutable choice, lost control repair, manual D, reserve and failure gates PASS");
}
