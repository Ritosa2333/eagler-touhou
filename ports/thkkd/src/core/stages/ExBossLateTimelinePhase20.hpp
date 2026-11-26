#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase20(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(t.frame==100){
        const auto initialize=[&](std::uint32_t slot,std::uint32_t kind){auto& item=items.items[slot];item.word(0,1);item.word(0x10,0);item.word(8,kind);item.word(4,0);item.word(0xc,0);item.quad(0x18,enemy().quad(0x38));if(compare(m.i(0),m.q(item.quad(0x18)))>0)item.quad(0x18,0);else if(compare(m.c(0x6d1cb0),m.q(item.quad(0x18)))<0)item.quad(0x18,constant(0x6d1cb0));item.quad(0x20,enemy().quad(0x40));};
        for(unsigned i=0;i<3;++i){const auto base=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            for(unsigned j=0;j<20;++j){const auto slot=find_free_item(items);if(slot==-1)continue;initialize(static_cast<std::uint32_t>(slot),0);auto& item=items.items[static_cast<std::uint32_t>(slot)];const auto half=m.mul(m.i(j),m.c(0x6d1860));const auto angle=m.store(m.add(m.div(m.add(half,half),m.c(0x6d1a40)),m.q(base))),speed=m.store(m.div(m.i(i*12u+48u),m.c(0x6d1a40)));
                auto value=math.cosine(angle);item.quad(0x28,m.store(m.mul(value,m.q(speed))));value=math.sine(angle);item.quad(0x30,m.store(m.mul(value,m.q(speed))));item.quad(0x38,constant(0x6d23b0));
            }
        }
        const auto slot=find_free_item(items);if(slot!=-1){initialize(static_cast<std::uint32_t>(slot),1);auto& item=items.items[static_cast<std::uint32_t>(slot)];auto value=math.cosine(0);item.quad(0x28,m.store(m.mul(value,m.i(0))));value=math.sine(0);item.quad(0x30,m.store(m.mul(value,m.i(0))));item.quad(0x38,constant(0x6d23b0));}
    }else if(t.frame==130){enemy().word(8,17);const auto side=random.integer_bits(1,precision,true)?20u:364u;const auto x=stage6_boss_timeline_pick_character(character,{{192,side,192}});enemy().quad(0xa8,m.store(m.i(x)));const auto y=stage6_boss_timeline_pick_character(character,{{90,120,140}});enemy().quad(0xb0,m.store(m.i(y)));enemy().quad(0xb8,constant(0x6d1e20));enemy().quad(0xc0,constant(0x6d1e20));}
    if(character==0)phase20_reimu();else if(character==1)phase20_marisa();else phase20_sanae();
}
}}
