#include "EnemyState.hpp"
namespace th15 {
const std::string* EnemyState::check_interrupt(EnemyWorldState& world)noexcept{
    phase_life=life;life_threshold=0;
    for(auto& interrupt:interrupts)if(interrupt.life>=0){
        phase_life=wrapping_sub(life,interrupt.life);life_threshold=interrupt.life;
        if(life<=interrupt.life){
            if(chapter_contribution&&chapter==world.current_chapter){world.chapter_defeated=wrapping_add(world.chapter_defeated,chapter_contribution);chapter_contribution=0;}
            life=interrupt.life;interrupt.life=-1;age_timer.set(0);flags&=~0x1000000u;return &interrupt.script;
        }break;
    }
    for(auto& interrupt:interrupts)if(interrupt.life>=0&&interrupt.time>0){
        if(flags&0x800000){const i32 remaining=wrapping_sub(interrupt.time,age_timer.current),seconds=remaining/60,hundredths=wrapping_mul(remaining%60,100)/60;world.boss_seconds=seconds>99?99:seconds;world.boss_hundredths=seconds>99?99:hundredths;
            if(world.practice&&world.practice->enabled)world.practice->lock_timer.observe();
        }
        if(age_timer.current<interrupt.time)return nullptr;
        life=interrupt.life;interrupt.life=-1;age_timer.set(0);flags|=0x1000000;
        if(!(world.spell_flags()&8)){
            flags&=~0x1000000u;world.spell_flags()|=128;
            if((world.spell_flags()&1)&&world.spell_elapsed()>=60){world.spell_flags()&=~34u;world.spell_bonus()=0;world.boss_damage_gate=0;world.counters[2]=0;}
            else{if((world.spell_flags()&1)&&world.game_state==1)world.spell_flags()|=32;world.boss_damage_gate=0;world.counters[2]=0;}
        }else if(world.spell_flags()&1)world.chapter_defeated=wrapping_add(world.chapter_defeated,chapter_contribution);
        chapter_contribution=0;return &interrupt.timeout_script;
    }
    return nullptr;
}
}
