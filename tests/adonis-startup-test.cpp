#include <eagler/netplay/AdonisStartup.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>
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
    std::vector<std::uint64_t> probeTimes;
    bool dead=false,closed=false,blocked=false,dropAccept=false,dropCommit=false,dropAck=false;
    bool IsOpen()const override{return !dead&&!closed;}
    bool Failed()const override{return dead;}
    std::size_t BufferedAmount()const override{return 0;}
    bool Send(const std::uint8_t* bytes,std::size_t size,bool reliable){
        if(dead||blocked)return false;
        assert(size==64 && AdonisStartup::IsPacket(bytes,size));
        if(bytes[4]==2)probeTimes.push_back(now);
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
                                unsigned loss=0,bool controls=false,bool asymmetric=false,std::uint64_t oneWayUs=35'000){
    Link a,b;a.other=&b;b.other=&a;a.dropEvery=b.dropEvery=loss;
    a.delay=b.delay=oneWayUs;
    if(asymmetric){a.delay=5'000;b.delay=65'000;}
    if(controls){a.dropCommit=true;b.dropAccept=true;b.dropAck=true;}
    AdonisStartup left,right;assert(left.Begin(config(0),0,mode,request,reserve));assert(right.Begin(config(1),0,mode,request,reserve));
    unsigned progress=0;
    for(std::uint64_t t=0;t<10'000'000;t+=1'000){
        a.now=b.now=t;assert(pump(left,a,t)&&pump(right,b,t));
        // Measurement never commits early or invents a value from no samples.
        if(left.Ready()||right.Ready()){
            assert(left.Probes()==129&&right.Probes()==129);
            assert(t>=a.probeTimes.back()+216'000&&t>=b.probeTimes.back()+216'000);
        }
        if(left.Ready()&&right.Ready()){
            if(++progress==800)break; // delayed/duplicate commit and echo packets survive handoff
        }
    }
    assert(left.Ready()&&right.Ready());
    auto l=left.Selected(),r=right.Selected();assert(l.delay==r.delay&&l.fullDelay==r.fullDelay&&l.prediction==r.prediction);
    assert(left.Local().received+left.Local().lost==120&&right.Local().received+right.Local().lost==120);
    assert(a.probeTimes.size()==129&&b.probeTimes.size()==129&&a.control&&b.control);
    assert(a.probeTimes.front()>=AdonisStartup::StabilizeUs+1'000);
    assert(b.probeTimes.front()>=AdonisStartup::StabilizeUs+1'000);
    for(const auto* times:{&a.probeTimes,&b.probeTimes})for(unsigned i=1;i<times->size();++i)
        assert((*times)[i]-(*times)[i-1]==16'000);
    assert(left.Local().minUs<=left.Local().meanUs&&left.Local().meanUs<=left.Local().maxUs);
    assert(l.delay==(request==AdonisStartup::Automatic?l.fullDelay-l.prediction:request));
    const auto available=l.fullDelay-(request==AdonisStartup::Automatic?1u:0u);
    assert(l.prediction==(mode==AdonisMode::Hybrid?std::min(available,reserve):0));
    std::printf("startup mode=%u requested=%u reserve=%u loss=%u asymmetric=%u -> B=%u D=%u P=%u PASS\n",
        unsigned(mode),request,reserve,loss,unsigned(asymmetric),l.fullDelay,l.delay,l.prediction);
    return l;
}
static void stabilization_restarts_after_disconnect(){
    Link a,b;a.other=&b;b.other=&a;
    AdonisStartup left,right;
    assert(left.Begin(config(0),0,AdonisMode::Delay,AdonisStartup::Automatic));
    assert(right.Begin(config(1),0,AdonisMode::Delay,AdonisStartup::Automatic));
    for(std::uint64_t t=0;t<=1'700'000;t+=1'000){
        a.closed=t>=500'000&&t<700'000;a.now=b.now=t;
        assert(pump(left,a,t)&&pump(right,b,t));
        if(t<1'700'000)assert(a.probeTimes.empty());
    }
    assert(a.probeTimes.size()==1&&a.probeTimes.front()==1'700'000);
    assert(!left.Ready());
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
    assert(l.Tick(a,10'000'000));
    assert(!l.Tick(a,AdonisStartup::PeerWaitUs)&&l.Failed());
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
struct MeshLink:PeerTransport{
    struct Pending{std::uint64_t at;std::vector<std::uint8_t> bytes;};
    unsigned seat=0,probes[3]{};
    std::uint64_t now=0,acceptAfter=0;
    MeshLink* peers[3]{};
    std::deque<Pending> incoming;
    bool dropSlowInput=false,dropCommit=false,dropAck=false;
    bool IsOpen()const override{return true;}
    bool Failed()const override{return false;}
    bool Forward(unsigned peer,const std::uint8_t* p,std::size_t n,bool control){
        assert(peer<3&&peer!=seat&&n==64&&p[3]==3&&p[5]==seat&&p[56]==3);
        if(!control){
            assert(p[57]==peer);
            if(p[4]==2)++probes[peer];
            if(dropSlowInput&&seat&&peer)return true;
        }else{
            assert(p[57]==255);
            if(p[4]==6&&peer==0&&now<acceptAfter)return true;
            if(p[4]==7&&peer==2&&dropCommit){dropCommit=false;return true;}
            if(p[4]==8&&peer==0&&dropAck){dropAck=false;return true;}
        }
        const auto delay=seat&&peer?(seat==1?50'000u:70'000u):1'000u;
        peers[peer]->incoming.push_back({now+delay,{p,p+n}});
        if(control)peers[peer]->incoming.push_back({now+delay+1'000,{p,p+n}});
        return true;
    }
    bool SendTo(std::uint8_t peer,const std::uint8_t* p,std::size_t n)override{return Forward(peer,p,n,false);}
    bool SendRepairTo(std::uint8_t,const std::uint8_t*,std::size_t)override{return false;}
    bool SendControl(const std::uint8_t* p,std::size_t n)override{
        for(unsigned peer=0;peer<3;++peer)if(peer!=seat)Forward(peer,p,n,true);
        return true;
    }
    bool Poll(std::vector<std::uint8_t>* out)override{
        for(auto i=incoming.begin();i!=incoming.end();++i)if(i->at<=now){*out=i->bytes;incoming.erase(i);return true;}
        return false;
    }
    std::size_t BufferedAmount()const override{return 0;}
};
static void mesh(AdonisMode mode,std::uint32_t request,bool missingLink=false){
    std::array<MeshLink,3> links{};std::array<AdonisStartup,3> startup{};
    for(unsigned seat=0;seat<3;++seat){
        auto& link=links[seat];link.seat=seat;link.dropSlowInput=missingLink;
        for(unsigned peer=0;peer<3;++peer)link.peers[peer]=&links[peer];
        auto c=config(seat);c.playerCount=3;
        assert(startup[seat].Begin(c,0,mode,request));
    }
    links[0].dropCommit=true;links[2].dropAck=true;
    links[2].acceptAfter=4'000'000; // Host must wait for the third acceptance.
    bool failed=false;
    for(std::uint64_t now=0;now<6'000'000&&!failed;now+=1'000){
        for(auto& link:links)link.now=now;
        for(unsigned seat=0;seat<3;++seat){
            std::vector<std::uint8_t> bytes;
            while(links[seat].Poll(&bytes))if(!startup[seat].Receive(links[seat],bytes.data(),bytes.size(),now)){failed=true;break;}
            if(!failed&&!startup[seat].Tick(links[seat],now))failed=true;
        }
        if(now<links[2].acceptAfter)assert(!startup[0].Ready());
    }
    if(missingLink){assert(failed&&!startup[0].Ready());return;}
    assert(!failed);
    const auto choice=startup[0].Selected();assert(choice.fullDelay==4);
    assert(choice.delay==(request==AdonisStartup::Automatic?(mode==AdonisMode::Hybrid?2u:4u):request));
    for(unsigned seat=0;seat<3;++seat){
        assert(startup[seat].Ready()&&startup[seat].Selected().delay==choice.delay);
        assert(startup[seat].Probes()==129&&startup[seat].Replies()==120);
        assert(startup[seat].ReportFor(1).p95Us>=120'000&&startup[seat].ReportFor(2).p95Us>=120'000);
        for(unsigned peer=0;peer<3;++peer)if(peer!=seat){
            assert(links[seat].probes[peer]==129&&startup[seat].LinkTo(peer).received==120);
            assert(startup[seat].ReportFor(peer)==startup[peer].Local());
        }
    }
}
static void connection_owner(){
    Link a,b;a.other=&b;b.other=&a;a.dropCommit=true;b.dropAck=true;
    AdonisConnection host(a),peer(b);auto c=config(0);c.gameId=8;auto d=c;d.localPlayer=1;
    host.Prepare(c,AdonisMode::Hybrid,true,0,2);peer.Prepare(d,AdonisMode::Hybrid,true,0,2);
    assert(host.Pump(0,false)&&peer.Pump(0,false)&&a.fast==0&&b.fast==0);
    bool hello=false;
    for(std::uint64_t now=1000;now<7'000'000;now+=1000){
        a.now=b.now=now;assert(host.Pump(now,true)&&peer.Pump(now,true));
        if(host.NeedsApply()){host.Applied();SessionPacket packet;packet.sessionId=c.sessionId;
            packet.seed=c.seed;packet.gameplayAbi=c.gameplayAbi;packet.gameId=c.gameId;
            packet.playerCount=2;packet.senderPlayer=0;packet.phase=SessionPhase::Hello;
            std::vector<std::uint8_t> wire;assert(EncodeSessionPacket(packet,&wire));b.incoming.push_back({now+1000,wire});hello=true;}
        if(peer.NeedsApply())peer.Applied();
    }
    assert(hello&&!host.Waiting()&&!peer.Waiting()&&host.Status()[1]==5&&peer.Status()[1]==5);
    std::vector<std::uint8_t> wire;SessionPacket packet;
    assert(peer.Poll(&wire)&&DecodeSessionPacket(wire.data(),wire.size(),&packet)&&packet.senderPlayer==0);
    auto timing=MakeAdonisSpectatorTiming(host.Startup(),c,true);auto bytes=timing.Encode();AdonisSpectatorTiming decoded;
    assert(bytes.size()==40&&AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),decoded));
    assert(decoded.delay==1&&decoded.prediction==2&&decoded.rttP95Us==70'000);
    bytes[6]=9;assert(!AdonisSpectatorTiming::Decode(bytes.data(),bytes.size(),decoded));
    const auto choice=host.Startup().Selected();
    a.closed=true;
    assert(host.Pump(7'001'000,true)&&!host.Failed());
    assert(host.Startup().Ready()&&!host.Waiting());
    a.closed=false;
    assert(host.Pump(7'002'000,true)&&host.Startup().Ready());
    assert(host.Startup().Selected().delay==choice.delay);
    // A terminal wire failure remains fatal to gameplay, not to calibration.
    a.dead=true;
    assert(host.Pump(7'003'000,true)&&host.Failed());
    assert(host.Startup().Ready()&&!host.Startup().Failed());
    assert(host.Status()[1]==5&&host.Startup().Selected().delay==choice.delay);
}
static void connection_waits_for_loading_peer(){
    // RTC can be healthy while a slower endpoint is still creating its world.
    // The local resource-ready time must not consume the measurement deadline.
    for(unsigned slowSeat:{0u,1u,2u}){
        Link a,b;a.other=&b;b.other=&a;
        AdonisConnection host(a),peer(b);
        host.Prepare(config(0),AdonisMode::Delay,true,0,2);
        peer.Prepare(config(1),AdonisMode::Delay,true,0,2);
        for(std::uint64_t now=0;now<18'000'000;now+=1000){
            a.now=b.now=now;
            a.closed=b.closed=slowSeat==2&&now<12'000'000;
            assert(host.Pump(now,slowSeat!=0||now>=12'000'000));
            assert(peer.Pump(now,slowSeat!=1||now>=12'000'000));
            if(now<13'000'000){assert(a.probeTimes.empty()&&b.probeTimes.empty());}
            if(host.NeedsApply())host.Applied();
            if(peer.NeedsApply())peer.Applied();
        }
        assert(!host.Waiting()&&!peer.Waiting());
        assert(host.Startup().Replies()==120&&peer.Startup().Replies()==120);
    }
}
int main(){
    connection_owner();
    connection_waits_for_loading_peer();
    // Original conversion boundaries; in particular the user's 32 ms link
    // must no longer receive an additional mandatory queued frame.
    for(const auto [rtt,frames]:{std::pair{1u,1u},{32'000u,1u},{33'333u,1u},{33'334u,2u},{90'000u,3u},{100'002u,4u},{140'000u,5u}}){
        AdonisStartup::Report report{rtt,120,0};
        const auto pure=AdonisStartup::Choose(report,report,AdonisMode::Delay,AdonisStartup::Automatic,2);
        const auto hybrid=AdonisStartup::Choose(report,report,AdonisMode::Hybrid,AdonisStartup::Automatic,2);
        assert(pure.delay==frames&&pure.fullDelay==frames);
        assert(hybrid.delay==std::max(1u,frames-std::min(frames-1,2u))&&hybrid.prediction==std::min(frames-1,2u));
        const auto manual=AdonisStartup::Choose(report,report,AdonisMode::Hybrid,0,2);
        assert(manual.delay==0&&manual.prediction==std::min(frames,2u));
    }
    const auto low=run(AdonisMode::Hybrid,AdonisStartup::Automatic,2,0,true,false,16'000);
    assert(low.fullDelay==1&&low.delay==1&&low.prediction==0);
    const auto pure=run(AdonisMode::Delay,AdonisStartup::Automatic);
    const auto hybrid=run(AdonisMode::Hybrid,AdonisStartup::Automatic);
    assert(pure.fullDelay==hybrid.fullDelay&&pure.delay==hybrid.delay+2);
    run(AdonisMode::Hybrid,AdonisStartup::Automatic,1,23,true,true);
    run(AdonisMode::Delay,AdonisStartup::Automatic,2,31,true);
    for(unsigned d:{0u,1u,9u})for(auto m:{AdonisMode::Delay,AdonisMode::Hybrid})run(m,d);
    stabilization_restarts_after_disconnect();mismatches();allowance();failure_and_reuse();
    mesh(AdonisMode::Delay,AdonisStartup::Automatic);
    mesh(AdonisMode::Hybrid,AdonisStartup::Automatic);
    mesh(AdonisMode::Hybrid,0);mesh(AdonisMode::Delay,9);
    mesh(AdonisMode::Hybrid,AdonisStartup::Automatic,true);
    Link a,b;a.other=&b;b.other=&a;AdonisStartup s;
    assert(!s.Begin(config(0),0,AdonisMode::Hybrid,10));
    assert(!s.Begin(config(0),0,AdonisMode::Hybrid,0,3));
    assert(s.Begin(config(0),1,AdonisMode::Delay,0));assert(!s.Tick(a,0));
    assert(s.Begin(config(0),0,AdonisMode::Delay,0));assert(!s.Tick(a,AdonisStartup::PeerWaitUs));
    std::puts("Actual input lane, two-sided immutable choice, lost control repair, manual D, reserve and failure gates PASS");
}
