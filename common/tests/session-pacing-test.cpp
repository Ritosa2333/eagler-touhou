#include <eagler/netplay/SessionPacing.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace Netplay;
int main()
{
    SessionPacing pacing;
    InputPacket packet{}; packet.senderPlayer=1;
    // A symmetric three-frame delivery delay is latency, not a clock lead.
    for(unsigned f=0;f<100;++f){packet.senderFrame=f;packet.frameAdvantage=3;pacing.Observe(f+3,packet);}
    assert(pacing.IntervalScale()==1 && pacing.Lead()==0);
    // Eight frames of real local lead, plus three in transit.
    for(unsigned f=100;f<250;++f){packet.senderFrame=f;packet.frameAdvantage=-5;pacing.Observe(f+11,packet);}
    assert(pacing.Lead()==8 && pacing.IntervalScale()>1.019 && pacing.IntervalScale()<=1.02);
    const auto scale=pacing.IntervalScale();
    // Retransmission and reordering cannot skew the estimator.
    for(unsigned f=0;f<100;++f){packet.senderFrame=f;packet.frameAdvantage=30;pacing.Observe(f,packet);}
    assert(pacing.IntervalScale()==scale);
    // A second, faster peer must not cancel the wait needed by the slower one.
    packet.senderPlayer=2;
    for(unsigned f=100;f<250;++f){packet.senderFrame=f;packet.frameAdvantage=11;pacing.Observe(f-5,packet);}
    assert(pacing.Lead()==8);
    pacing.Reset();assert(pacing.IntervalScale()==1 && pacing.Lead()==0);
    // An implausible sample doesn't affect the clock.
    packet.senderFrame=0;packet.frameAdvantage=-100;pacing.Observe(0,packet);
    assert(pacing.IntervalScale()==1);
    // Equal nominal clocks starting eight frames apart converge with bounded
    // wall-clock pacing; no logical frames are inserted or discarded.
    SessionPacing ahead,behind;double x=108,y=100;
    for(unsigned step=0;step<1800;++step){
        InputPacket p{};p.senderPlayer=1;p.senderFrame=unsigned(y)-3;
        p.frameAdvantage=short(int(y)-int(x)+3);ahead.Observe(unsigned(x),p);
        p.senderFrame=unsigned(x)-3;p.frameAdvantage=short(int(x)-int(y)+3);behind.Observe(unsigned(y),p);
        x+=1/ahead.IntervalScale();y+=1/behind.IntervalScale();
    }
    assert(std::abs(x-y)<2);
    std::puts("session pacing: PASS");
}
