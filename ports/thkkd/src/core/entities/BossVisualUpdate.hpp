// 恢复0x60ea60：符卡圆环跟随Boss，再更新入场演出；逐条保留x87写回顺序。
#pragma once
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/player/OptionsMotion.hpp"
namespace kaikeidou {
inline void update_boss_visuals(const EnemyPool& enemies,BossPhaseContext& phase,BossPresentationContext& presentation,unsigned precision){
    using namespace binary_float;
    auto& ring=presentation.spell_ring_reals;auto& entry=presentation.entry_reals;
    const auto add_to=[&](std::uint64_t& field,std::uint64_t value){field=to_binary64(add(binary64(field),binary64(value),precision));};
    if(phase.spell_overlay==2||phase.spell_overlay==1){
        ring[0]=ease_option_value(ring[0],enemies.enemies.at(phase.boss_index).quad(0x38),0x4034000000000000ull,precision);
        ring[1]=ease_option_value(ring[1],enemies.enemies.at(phase.boss_index).quad(0x40),0x4034000000000000ull,precision);
        if(phase.spell_overlay==2){auto speed=binary64(ring[3]);speed.negative=!speed.negative;ring[2]=to_binary64(add(binary64(ring[2]),speed,precision));}
        else{
            ring[2]=ease_option_value(ring[2],0x3ff028f5c28f5c29ull,0x4038000000000000ull,precision);
            if(crt_sse2::real(ring[2])>=1.){phase.spell_overlay=2;ring[2]=0x3ff0000000000000ull;}
        }
    }
    if(presentation.entry_state==2||presentation.entry_state==4){add_to(entry[1],0xc018000000000000ull);add_to(entry[2],0x3faa9fbe76c8b439ull);}
    else if(presentation.entry_state==1){
        add_to(entry[0],0x4000000000000000ull);auto speed=binary64(entry[3]);speed.negative=!speed.negative;entry[1]=to_binary64(add(binary64(entry[1]),speed,precision));
        entry[3]=to_binary64(multiply(binary64(entry[3]),binary64(entry[4]),precision));add_to(entry[4],0xbf60624dd2f1a9fcull);
        if(crt_sse2::real(entry[0])>=100.){presentation.entry_state=2;entry[0]=0x4059000000000000ull;entry[2]=0x3ff921fb54442d28ull;}
    }else if(presentation.entry_state==3){
        entry[0]=ease_option_value(entry[0],0,0x4044000000000000ull,precision);add_to(entry[1],0xc018000000000000ull);add_to(entry[2],0x3faa9fbe76c8b439ull);
    }
}
}
