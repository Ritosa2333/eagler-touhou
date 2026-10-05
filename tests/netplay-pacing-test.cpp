#include "../portable/sdl/FrameCadence.hpp"
#include <eagler/netplay/SessionPacing.hpp>
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <cassert>
#include <cstdio>
int main(){
    using Clock=Netplay::PendingFrameSchedule<touhou::sdl::FrameCadence>;
    constexpr auto step=touhou::sdl::FrameCadence::interval;
    // Healthy SP/live cadence remains the title's original cadence.
    for(bool live:{false,true})for(double hz:{30.,59.94,60.,90.,120.,144.,165.,240.}){
        Clock clock;touhou::sdl::FrameCadence original;
        for(unsigned i=0;i<unsigned(hz*10);++i){
            assert(clock.advance(1./hz,live)==original.advance(1./hz));
            clock.complete();
        }
    }
    Clock stalled;
    assert(stalled.advance(step*1.75,true)==1);stalled.blocked(true);
    const auto phase=stalled.debt;
    for(double wait:{0.,.001,step*.5,step*2,10.,100.}){
        assert(stalled.advance(wait,true)==1);
        stalled.blocked(true);
        assert(stalled.retry_pending()&&stalled.debt==phase);
    }
    // Recovery services that pending frame once; it cannot repay the stall.
    stalled.complete();assert(!stalled.retry_pending());
    assert(stalled.advance(0,true)==0);
    assert(stalled.advance(step*.3,true)==1);stalled.complete();
    assert(stalled.advance(0,true)==0);
    // CPU stalls also discard expired wall-time slots, preserving logical order.
    stalled.reset();assert(stalled.advance(1.,true)==1);stalled.complete();
    for(unsigned i=0;i<60;++i){assert(stalled.advance(step,true)==1);stalled.complete();}
    assert(stalled.advance(0,true)==0);
    stalled.blocked(true);stalled.reset();assert(stalled.advance(0,true)==0);
    assert(stalled.advance(step,true)==1);stalled.blocked(true);
    assert(stalled.advance(0,false)==0&&!stalled.retry_pending());
    for(unsigned hz:{60u,120u,144u,165u}){
        Clock clock[2];Netplay::SessionPacing pacing[2];
        unsigned frame[2]{108,100};
        for(unsigned callback=0;callback<hz*60;++callback){
            for(unsigned seat=0;seat<2;++seat){
                const auto peer=1-seat;Netplay::InputPacket packet{};
                packet.senderPlayer=peer;packet.senderFrame=frame[peer]-3;
                packet.frameAdvantage=short(int(frame[peer])-int(frame[seat])+3);
                pacing[seat].Observe(frame[seat],packet);
            }
            for(unsigned seat=0;seat<2;++seat){
                const auto ticks=clock[seat].advance((1./hz)/pacing[seat].IntervalScale(),true);
                assert(ticks<=1);frame[seat]+=ticks;
                clock[seat].complete();
                assert(clock[seat].interpolation_alpha()>=0&&clock[seat].interpolation_alpha()<=1);
            }
        }
        const auto difference=int(frame[0])-int(frame[1]);
        assert(difference>=-2&&difference<=2);
        assert(frame[0]>3600&&frame[1]>3600);
    }
    std::puts("TH10 netplay display cadence: PASS");
}
