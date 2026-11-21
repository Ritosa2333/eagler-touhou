#include "game/EnemyPhaseRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>

std::string target(std::uint32_t value){return value?std::to_string(value):std::string{};}
std::uint32_t identity(const std::string& value){return value.empty()?0:std::uint32_t(std::stoul(value));}
int main(){
    char command=0;
    while(std::cin>>command){
        th128::enemy::PhaseContext context;
        if(command=='Q'){
            std::uint32_t fraction=0;std::cin>>context.life>>context.remaining>>context.threshold
                >>context.timer.previous>>context.timer.current>>fraction>>context.timer.clockSource>>context.timer.flags>>context.flags;
            context.timer.fraction=std::bit_cast<float>(fraction);
            for(auto& record:context.records){std::uint32_t health=0,time=0;std::cin>>record.threshold>>record.limit>>health>>time;
                record.healthTarget=target(health);record.timeTarget=target(time);}
            for(auto& value:context.countdown)std::cin>>value;
            th128::enemy::PhaseSpell spell;std::cin>>spell.flags>>spell.age>>spell.value80>>spell.playerMode>>context.managerWord18;
            context.spell=spell;
            const auto result=th128::enemy::queryPhase(context);assert(result);
            const auto& after=result->next;
            std::cout<<identity(result->target)<<' '<<after.life<<' '<<after.remaining<<' '<<after.threshold
                <<' '<<after.timer.previous<<' '<<after.timer.current<<' '<<std::bit_cast<std::uint32_t>(after.timer.fraction)
                <<' '<<after.timer.clockSource<<' '<<after.timer.flags<<' '<<after.flags;
            for(const auto& record:after.records)std::cout<<' '<<std::uint32_t(record.threshold)<<' '<<std::uint32_t(record.limit)
                <<' '<<identity(record.healthTarget)<<' '<<identity(record.timeTarget);
            for(const auto value:after.countdown)std::cout<<' '<<value;
            std::cout<<' '<<after.spell->flags<<' '<<after.spell->value80<<' '<<after.managerWord18<<'\n';
        }else if(command=='R'){
            int slot=0;std::int32_t threshold=0,limit=0;std::cin>>slot>>threshold>>limit;
            for(auto& record:context.records){std::uint32_t health=0,time=0;std::cin>>record.threshold>>record.limit>>health>>time;
                record.healthTarget=target(health);record.timeTarget=target(time);}
            assert(th128::enemy::registerPhase(context.records,slot,threshold,limit,"3538976"));
            for(std::size_t index=0;index<context.records.size();++index){const auto& record=context.records[index];if(index)std::cout<<' ';
                std::cout<<std::uint32_t(record.threshold)<<' '<<std::uint32_t(record.limit)<<' '<<identity(record.healthTarget)<<' '<<identity(record.timeTarget);}
            std::cout<<'\n';
        }else return 2;
    }
    th128::enemy::PhaseContext invalid;invalid.life=100;invalid.records[0]={50,1,"Health","Timeout"};invalid.timer.current=1;
    assert(!th128::enemy::queryPhase(invalid));
    invalid.timer.current=0;assert(th128::enemy::queryPhase(invalid));
    invalid.life=50;assert(th128::enemy::queryPhase(invalid)->target=="Health");
    assert(!th128::enemy::registerPhase(invalid.records,8,1,1,"bad"));assert(!th128::enemy::registerPhase(invalid.records,-1,1,1,"bad"));
    const auto retained=invalid.records[0];assert(!th128::enemy::registerPhase(invalid.records,0,1,1,std::string(201,'a')));
    assert(invalid.records[0].healthTarget==retained.healthTarget);
}
