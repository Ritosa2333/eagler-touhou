#include "EnemyState.hpp"
namespace th17 {
const std::string* EnemyState::check_timeout(EnemyWorldState& world)noexcept{
    for(auto& entry:interrupts)if(entry.life>=0&&entry.time>0){
        if(flags&0x800000){const i32 remaining=wrapping_sub(entry.time,age_timer.current),seconds=remaining/60,hundredths=wrapping_mul(remaining%60,100)/60;world.boss_seconds=seconds>99?99:seconds;world.boss_hundredths=seconds>99?99:hundredths;}
        if(age_timer.current<entry.time)return nullptr;
        life=entry.life;entry.life=-1;age_timer.set(0);flags|=0x1000000;
        if(!(world.spell_flags()&8)){world.spell_flags()|=128;if(world.spell_flags()&1){if(world.spell_elapsed()>=60){world.spell_flags()&=~34u;world.spell_bonus()=0;}else if(world.game_state==1)world.spell_flags()|=32;}world.boss_damage_gate=0;world.counter(2)=0;}
        else if((world.spell_flags()&9)==9){flags&=~0x1000000u;world.chapter_defeated=wrapping_add(world.chapter_defeated,chapter_contribution);}
        chapter_contribution=0;return &entry.timeout_script;
    }return nullptr;
}
const std::string* EnemyState::check_life_interrupt(EnemyWorldState& world)noexcept{
    phase_life=life;life_threshold=0;
    for(auto& entry:interrupts)if(entry.life>=0){phase_life=wrapping_sub(life,entry.life);life_threshold=entry.life;if(life<=entry.life){if(chapter_contribution&&chapter==world.current_chapter){world.chapter_defeated=wrapping_add(world.chapter_defeated,chapter_contribution);chapter_contribution=0;}life=entry.life;entry.life=-1;age_timer.set(0);flags&=~0x1000000u;return &entry.script;}break;}return nullptr;
}
}
