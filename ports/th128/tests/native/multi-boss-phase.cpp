#include "game/EnemyPhaseRules.hpp"
#include "game/EnemyRules.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

namespace {
struct Entity {
    std::array<th128::enemy::PhaseRecord,8> records{};
    th128::enemy::PhaseTimer timer{};
    std::int32_t life=0,remaining=0,threshold=0;
    std::uint32_t flags=0x81000000u;
};

struct BossOwner {
    std::int32_t retained=0;
    std::uint32_t flags=0;
};

std::array<Entity,3> entities{};
std::array<BossOwner,3> owners{};
th128::enemy::BossSlots slots{};
th128::enemy::BossGaugeState gauges{};
std::array<std::uint32_t,2> countdown{0xffffffffu,0};
std::int32_t managerWord=0;
th128::enemy::PhaseSpell spell{};

std::string text(const std::string& value){return value.empty()?std::string("-"):value;}
std::string bits(std::uint32_t value){char buffer[16];std::snprintf(buffer,sizeof buffer,"%08x",value);return buffer;}
}

int main(){
    char command=0;
    while(std::cin>>command){
        if(command=='B'){
            int index=0,slot=0;std::int32_t threshold=0,limit=0;std::string target;
            if(!(std::cin>>index>>slot>>threshold>>limit>>target)||index<0||index>2)return 2;
            const bool ok=th128::enemy::registerPhase(entities[std::size_t(index)].records,slot,threshold,limit,target);
            std::cout<<"B "<<index<<' '<<(ok?1:0)<<'\n';
        }else if(command=='C'){
            int index=0,slot=0;if(!(std::cin>>index>>slot)||index<0||index>2||slot<0||slot>7)return 2;
            const auto& record=entities[std::size_t(index)].records[std::size_t(slot)];
            std::cout<<"C "<<index<<' '<<slot<<' '<<record.threshold<<' '<<record.limit<<' '
                <<text(record.healthTarget)<<' '<<text(record.timeTarget)<<'\n';
        }else if(command=='Q'){
            int index=0;std::uint32_t spellFlags=0;std::int32_t current=0,life=0;
            if(!(std::cin>>index>>life>>current>>spellFlags)||index<0||index>2)return 2;
            auto& entity=entities[std::size_t(index)];
            entity.life=life;entity.timer.current=current;entity.timer.previous=current-1;
            spell.flags=spellFlags;
            th128::enemy::PhaseContext context;
            context.records=entity.records;context.timer=entity.timer;context.life=entity.life;
            context.remaining=entity.remaining;context.threshold=entity.threshold;context.flags=entity.flags;
            context.countdown=countdown;context.managerWord18=managerWord;context.spell=spell;
            const auto decision=th128::enemy::queryPhase(context);
            if(!decision){std::cout<<"Q "<<index<<" rejected\n";continue;}
            entity.records=decision->next.records;entity.timer=decision->next.timer;
            entity.life=decision->next.life;entity.remaining=decision->next.remaining;
            entity.threshold=decision->next.threshold;entity.flags=decision->next.flags;
            countdown=decision->next.countdown;managerWord=decision->next.managerWord18;
            if(!decision->next.spell)return 2;spell=*decision->next.spell;
            std::cout<<"Q "<<index<<' '<<text(decision->target)<<' '<<entity.life<<' '<<entity.remaining<<' '
                <<entity.threshold<<' '<<entity.timer.previous<<' '<<entity.timer.current<<' '
                <<countdown[0]<<' '<<countdown[1]<<' '<<bits(entity.flags)<<' '<<bits(spell.flags)<<' '
                <<managerWord<<' '<<(decision->timedOut?1:0)<<'\n';
        }else if(command=='T'){
            int index=0;std::int32_t slot=0;std::uint32_t id=0;
            if(!(std::cin>>index>>slot>>id)||index<0||index>2)return 2;
            auto& owner=owners[std::size_t(index)];
            const bool ok=th128::enemy::setBossSlot(slots,owner.flags,owner.retained,id,slot);
            std::cout<<"T "<<index<<' '<<(ok?1:0)<<' '<<slots.owners[0]<<' '<<slots.owners[1]<<' '
                <<slots.owners[2]<<' '<<bits(slots.flags)<<'\n';
        }else if(command=='U'){
            int index=0;if(!(std::cin>>index)||index<0||index>2)return 2;
            auto& owner=owners[std::size_t(index)];
            const bool ok=th128::enemy::setBossSlot(slots,owner.flags,owner.retained,0,-1);
            th128::enemy::clearUnownedBossGauges(gauges,slots);
            std::cout<<"U "<<index<<' '<<(ok?1:0)<<' '<<slots.owners[0]<<' '<<slots.owners[1]<<' '
                <<slots.owners[2]<<' '<<bits(slots.flags)<<' '<<bits(std::bit_cast<std::uint32_t>(gauges.values[0][0].ratio))
                <<' '<<(gauges.values[0][0].configured?1:0)<<' '
                <<bits(std::bit_cast<std::uint32_t>(gauges.values[1][0].ratio))<<' '
                <<(gauges.values[1][0].configured?1:0)<<'\n';
        }else if(command=='X'){
            int boss=0,slot=0;float life=0;std::int32_t maximum=0;std::uint32_t color=0;
            if(!(std::cin>>boss>>slot>>life>>maximum>>color)||boss<0||boss>7||slot<0||slot>3)return 2;
            const bool ok=th128::enemy::setBossGauge(gauges,boss,slot,life,maximum,color);
            const auto& segment=gauges.values[std::size_t(boss)][std::size_t(slot)];
            std::cout<<"X "<<boss<<' '<<slot<<' '<<(ok?1:0)<<' '
                <<bits(std::bit_cast<std::uint32_t>(segment.ratio))<<' '<<(segment.configured?1:0)<<' '
                <<segment.color<<'\n';
        }else if(command=='Z'){
            entities={};owners={};slots={};gauges={};countdown={0xffffffffu,0};managerWord=0;spell={};
            std::cout<<"Z\n";
        }else if(command=='P'){
            std::cout<<"P "<<slots.owners[0]<<' '<<slots.owners[1]<<' '<<slots.owners[2]<<' '<<bits(slots.flags);
            for(std::size_t boss=0;boss<3;++boss)std::cout<<' '<<(gauges.values[boss][0].configured?1:0)<<' '
                <<bits(std::bit_cast<std::uint32_t>(gauges.values[boss][0].ratio));
            std::cout<<'\n';
        }else return 2;
    }
    return 0;
}
