#include "../../cpp/multiplayer/SpectatorSchedule.hpp"
#include <cassert>
#include <cstdio>

using th09::multiplayer::SpectatorSchedule;
int main() {
    assert(SpectatorSchedule::Plan(0,4)==0);
    assert(SpectatorSchedule::Plan(3,1)==1);
    assert(SpectatorSchedule::Plan(6,1)==2);
    assert(SpectatorSchedule::Plan(100,1)==4);
    assert(SpectatorSchedule::Plan(8192,4)==6);
    for(double cost : {0.1,2.0,3.0,8.0,20.0}) {
        unsigned queued=100,played=0,rendered=0;double now=0;
        while(queued) {
            const auto plan=SpectatorSchedule::Plan(queued,4);
            unsigned done=0,drawn=0;double start=now;
            for(unsigned n=0;n<plan;++n) {
                if(!SpectatorSchedule::CanStart(done,now-start))break;
                assert(n==0||now-start<8.0);
                if(SpectatorSchedule::Render(n,plan))++drawn;
                --queued;++played;++done;now+=cost;
            }
            assert(done>=1&&done<=6&&drawn>=1);
            if(cost>=8)assert(done==1);
            assert(played+queued==100);rendered+=drawn;
        }
        assert(played==100&&rendered>0);
    }
    puts("Spectator callback: six-step/8ms start budget, visible fallback, exact queued input retention PASS");
}
