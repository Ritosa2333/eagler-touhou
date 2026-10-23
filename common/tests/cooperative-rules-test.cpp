#include <eagler/gameplay/CooperativeRules.hpp>
#include <cassert>
#include <cstdint>
#include <cstdio>
using namespace eagler::cooperative;
int main(){
    std::uint8_t state=0;
    assert(!AdvanceLifeTransfer(state,true,true)&&TapCount(state)==1);
    for(unsigned i=0;i<24;++i)assert(!AdvanceLifeTransfer(state,false,true));
    assert(!AdvanceLifeTransfer(state,true,true)&&TapCount(state)==1); // expired
    state=0;assert(!AdvanceLifeTransfer(state,true,true));
    for(unsigned i=0;i<23;++i)AdvanceLifeTransfer(state,false,true);
    assert(!AdvanceLifeTransfer(state,true,true)&&TapCount(state)==2); // exactly 24 apart
    AdvanceLifeTransfer(state,false,false);assert(state==0); // shoot/range/life gate
    for(unsigned i=0;i<4;++i)assert(!AdvanceLifeTransfer(state,true,true));
    const auto before=state;assert(AdvanceLifeTransfer(state,true,true)&&state==0);
    state=before;assert(AdvanceLifeTransfer(state,true,true)&&state==0); // rollback
    assert(!AdvanceLifeTransfer(state,true,true));
    for(unsigned i=0;i<100;++i)assert(!AdvanceLifeTransfer(state,false,true));
    assert(state==0); // held Shift never donates
    int power[]={96,30,24},step[]={20,12,12};bool alive[]={true,true,true};
    const auto select=[&]{return LowestPower(3,[&](unsigned i){return alive[i];},[&](unsigned i){return power[i];},[&](unsigned i){return step[i];});};
    assert(select()==2);power[1]=24;assert(select()==1);alive[1]=false;assert(select()==2);
    alive[2]=false;assert(select()==0);alive[0]=false;assert(select()==-1);
    int distance[]={1,9,4};bool needs[]={false,true,true};
    alive[0]=alive[1]=alive[2]=true;
    const auto nearest=[&]{return NearestPowerRecipient(3,[&](unsigned s){return alive[s];},
        [&](unsigned s){return needs[s];},[&](unsigned s){return distance[s];});};
    assert(nearest()==2);needs[1]=needs[2]=false;assert(nearest()==0);
    needs[2]=true;alive[2]=false;assert(nearest()==0); // Unused/ghost seats cannot receive.
    needs[1]=true;assert(nearest()==1);needs[0]=true;distance[0]=9;assert(nearest()==0);
    alive[0]=alive[1]=false;assert(nearest()==-1);
    static_assert(DeathbombFrames(6)==12&&DeathbombFrames(8)==16);
    std::puts("cooperative gestures, timeout boundary, rollback and normalized Power order: PASS");
}
