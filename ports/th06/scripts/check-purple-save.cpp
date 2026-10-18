#include "PurpleSave.hpp"
#include <cassert>
#include <cstdio>
int main() {
    using Save=PracticeRuntime::Purple::PurpleSave;
    Save stats;
    for(int spell=0;spell<65;++spell)for(int diff=0;diff<5;++diff)
        for(int player=0;player<4;++player)for(int state=0;state<3;++state) {
            assert(stats.Add(spell,diff,player,state));
            assert(stats.current.SC_History[spell][diff][player][state]==1);
            assert(stats.total.SC_History[spell][diff][player][state]==1);
        }
    assert(!stats.Add(65,0,0,0)&&!stats.Add(-1,0,0,0));
    assert(!stats.Add(0,5,0,0)&&!stats.Add(0,0,4,0)&&!stats.Add(0,0,0,3));
    stats.total.SC_History[64][4][3][0]=INT32_MAX;
    assert(stats.Add(64,4,3,0));
    assert(stats.total.SC_History[64][4][3][0]==INT32_MAX);
    assert(stats.GetCurrentSpellCardCount(64,4,3,Save::Capture)==2);
    stats.total.timePlayer[4][3]=1234567890123LL;
    assert(stats.GetTotalGameTime(4,3)==1234567890123LL);
    assert(stats.GetTotalGameTime(5,3)==0);
    std::puts("Purple TH06 native save layout, 3900 counters, bounds and overflow passed");
}
