#include "game/EnemyRules.hpp"
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
int main(){
    int maximum=0,boss=0,slot=0;float life=0;std::uint32_t color=0;
    while(std::cin>>maximum>>boss>>slot>>life>>color){
        th128::enemy::BossGaugeState state;
        assert(th128::enemy::setBossGauge(state,boss,slot,life,maximum,color));
        const auto& result=state.values[boss][slot];assert(result.configured);
        std::cout<<std::bit_cast<std::uint32_t>(result.ratio)<<' '<<result.color<<'\n';
        const auto retained=state;
        assert(!th128::enemy::setBossGauge(state,-1,slot,life,maximum,color));
        assert(!th128::enemy::setBossGauge(state,8,slot,life,maximum,color));
        assert(!th128::enemy::setBossGauge(state,boss,4,life,maximum,color));
        assert(!th128::enemy::setBossGauge(state,boss,slot,life,0,color));
        assert(state.values[boss][slot].ratio==retained.values[boss][slot].ratio&&state.values[boss][slot].color==retained.values[boss][slot].color);
    }
    th128::enemy::BossGaugeState gauges;th128::enemy::BossSlots owners;
    gauges.values[0][0]={.5f,0xffff8080u,true};gauges.values[1][0]={.25f,1,true};gauges.values[3][0]={.75f,2,true};owners.owners[1]=42;
    th128::enemy::clearUnownedBossGauges(gauges,owners);
    assert(gauges.values[0][0].ratio==0&&gauges.values[0][0].color==0xffff8080u&&gauges.values[0][0].configured);
    assert(gauges.values[1][0].ratio==.25f&&gauges.values[3][0].ratio==.75f);
}
