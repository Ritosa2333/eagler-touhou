#include <eagler/netplay/ConfirmedInputClock.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <array>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <deque>

using Netplay::ConfirmedInputClock;
static void ownership() {
    ConfirmedInputClock clock;
    assert(clock.CanCapture(0,0,false,0));
    clock.Captured(0);
    assert(!clock.CanCapture(1,0,false,20'000));
    assert(!clock.CanCapture(1,0,true,1'000));
    assert(clock.CanCapture(1,0,true,16'666));
    clock.Captured(16'666);
    assert(!clock.CanCapture(2,0,true,16'666));
    assert(!clock.CanCapture(ConfirmedInputClock::MaxLead,0,true,1'000'000));
    assert(!clock.CanCapture(1,2,true,1'000'000));
    assert(!clock.CanCapture(2,0,false,1'000'000)); // queued tail drains on resume
    assert(clock.CanCapture(2,2,false,1'000'000));
    clock.Captured(1'000'000);
    assert(!clock.CanCapture(3,2,true,1'000'000)); // no manufactured missed slots
    clock.Reset();
    assert(!clock.CanCapture(1,0,true,2'000'000));
    assert(clock.CanCapture(0,0,true,2'000'000));
    clock.BeginPhase(2'000'000);
    assert(!clock.CanCapture(1,0,true,2'000'000));
    assert(clock.CanCapture(1,0,true,2'016'666));
    clock.Captured(2'000'000); // owed current slot can precede the phase deadline
    assert(clock.CanCapture(1,0,true,2'016'666));
}
struct Packet { unsigned from,to,frame; std::uint64_t due; Netplay::FrameInput input; };
static unsigned run(unsigned seats,bool independent,unsigned variation) {
    struct Peer { Netplay::RollbackCore core; ConfirmedInputClock clock; unsigned capture=0,next=0; };
    std::array<Peer,3> peers;
    for(unsigned s=0;s<seats;++s){Netplay::CoreConfig c;c.sessionId=1;c.playerCount=seats;c.localPlayer=s;
        c.allowPrediction=false;c.maxRollbackFrames=12;assert(peers[s].core.Reset(c));}
    std::deque<Packet> packets;
    unsigned peak=0;
    for(std::uint64_t now=0;now<=2'000'000;now+=1'000){
        for(auto it=packets.begin();it!=packets.end();){
            if(it->due>now){++it;continue;}
            const auto r=peers[it->to].core.SubmitRemoteInput(it->from,it->frame,it->input);
            assert(r==Netplay::RemoteInputResult::Accepted||r==Netplay::RemoteInputResult::Duplicate);
            it=packets.erase(it);
        }
        if(now%17'000)continue; // explicitly modeled 58.8 Hz callbacks, not browser evidence
        for(unsigned s=0;s<seats;++s){auto& p=peers[s];
            if(p.clock.CanCapture(p.capture,p.next,independent,now)){
                Netplay::FrameInput input;input.buttons=std::uint16_t((p.capture+s)%3);
                const unsigned f=p.capture++;assert(p.core.ScheduleLocalInput(f,input));p.clock.Captured(now);
                for(unsigned to=0;to<seats;++to)if(to!=s){
                    // Mean one-way 25 ms. Alternate half of the declared RTT variation.
                    const int jitter=variation?((f+s)%2?int(variation/2):-int(variation/2)):0;
                    packets.push_back({s,to,f,now+std::uint64_t(25'000+jitter*1000),input});
                }
            }
            peak=std::max(peak,p.capture-p.next);
            const auto d=p.core.PrepareFrame(p.next);
            if(d.canAdvance){assert(!d.predictedMask);
                for(unsigned seat=0;seat<seats;++seat)assert(d.inputs[seat].buttons==(p.next+seat)%3);
                assert(p.core.MarkSimulated(p.next,d));++p.next;}
        }
    }
    assert(peak<=ConfirmedInputClock::MaxLead);
    unsigned low=peers[0].next;for(unsigned s=1;s<seats;++s)low=std::min(low,peers[s].next);
    return low;
}
int main(){
    ownership();
    for(unsigned seats:{2u,3u})for(unsigned variation:{5u,20u}){
        const auto baseline=run(seats,false,variation),pipelined=run(seats,true,variation);
        assert(pipelined>100&&pipelined>baseline+25);
        std::printf("confirmed-input clock model: seats=%u RTT=50 variation=%u coupled=%u pipelined=%u ticks/2s\n",seats,variation,baseline,pipelined);
    }
    std::puts("PASS confirmed UI input clock; deterministic core model, no WAN/display claim");
}
