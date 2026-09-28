#include "../../cpp/multiplayer/FrameSchedule.hpp"
#include <cstdio>
#include <cstdlib>

#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();} } while(0)
using th09::multiplayer::FrameSchedule;
using touhou::sdl::FrameCadence;
int main(){
    constexpr double step=FrameCadence::interval;
    for(bool rollback:{false,true})for(double hz:{30.,59.94,60.,90.,120.,144.,165.,240.}){
        FrameSchedule tested;FrameCadence original;
        for(unsigned i=0;i<unsigned(hz*10);++i){
            CHECK(tested.advance(1./hz,rollback)==original.advance(1./hz));
            tested.complete();
        }
    }
    FrameSchedule c;
    CHECK(c.advance(step*1.75,true)==1);c.blocked(true);
    // No time credit is needed: this is still the already due, once-captured
    // frame, not permission to simulate an extra fresh frame each callback.
    for(double delay:{0.,.001,step*.5,step*2,10.}){
        CHECK(c.advance(delay,true)==1);c.blocked(true);
    }
    c.complete();CHECK(c.advance(step*.30,true)==1);c.complete();
    CHECK(c.advance(0,true)==0);

    // A completed expensive callback keeps its fractional phase. An exhausted
    // multi-tick batch still discards unexecuted work instead of a catch-up loop.
    c.reset();CHECK(c.advance(step*1.75,true)==1);c.complete();c.exhausted(1,1);
    CHECK(c.advance(step*.30,true)==1);
    c.reset();CHECK(c.advance(step*3.75,true)==3);c.complete();c.exhausted(1,3);
    CHECK(c.advance(step*.30,true)==0);

    // A very long stall cannot turn one retry into multiple ticks or leave
    // whole-tick debt behind. Fractional phase alone is preserved.
    c.reset();FrameCadence capped;CHECK(c.advance(1.,true)==capped.advance(1.));c.blocked(true);
    CHECK(c.advance(100.,true)==1);c.complete();CHECK(c.advance(step*.2,true)==0);

    // Pause/restart and leaving rollback erase the pending retry. The ordinary
    // lockstep/non-rollback blocked behavior remains a hard clock reset.
    c.reset();CHECK(c.advance(step*1.75,true)==1);c.blocked(true);c.reset();CHECK(c.advance(0,true)==0);
    CHECK(c.advance(step*1.75,true)==1);c.blocked(true);CHECK(c.advance(0,false)==0);
    c.reset();CHECK(c.advance(step*1.75,false)==1);c.blocked(false);CHECK(c.advance(step*.3,false)==0);
    std::puts("Frame schedule: unchanged ordinary cadence; one pending retry; no stall debt; budget and pause/restart PASS");
}
