#include "EnemyState.hpp"
namespace th11 {
// 0x4100e0: spell damage is accumulated in sevenths with signed wrapping.
i32 EnemyState::take_damage(i32 amount)noexcept{
    if(health_flags&1){scaled_health=wrapping_sub(scaled_health,amount);const i32 difference=wrapping_sub(scaled_health,signed_bits(u32(health_baseline)*7));health=wrapping_add(difference/7,health_baseline);}
    else health=wrapping_sub(health,amount);
    return health;
}
// 0x417b40. The first active health interrupt takes precedence; the first
// active timed interrupt is then considered independently.
const char* EnemyState::check_interrupts(EnemyPhaseState& world)noexcept{
    interrupt_health=health;health_baseline=0;
    for(auto& interrupt:interrupts){
        if(interrupt.health<0)continue;
        interrupt_health=wrapping_sub(health,interrupt.health);health_baseline=interrupt.health;
        if(health<=interrupt.health){health=interrupt.health;interrupt.health=-1;lifetime.set(0,world.rate);flags&=~0x100000u;return interrupt.health_script;}
        break;
    }
    for(auto& interrupt:interrupts){
        if(interrupt.health<0||interrupt.time<1)continue;
        const i32 remaining=wrapping_sub(interrupt.time,lifetime.current),seconds=remaining/60,hundredths=(remaining%60)*100/60;
        *world.countdown_seconds=seconds>99?99:seconds;*world.countdown_hundredths=seconds>99?99:hundredths;
        if(lifetime.current<interrupt.time)return nullptr;
        health=interrupt.health;interrupt.health=-1;lifetime.set(0,world.rate);flags|=0x100000;
        if(!(*world.spell_flags&8)){
            if(*world.spell_flags&1){
                if(*world.spell_elapsed>=60){*world.spell_bonus=0;*world.spell_flags&=~0x22u;}
                else if(world.special_active&&signed_bits(u32(world.character)*3+u32(world.subtype))!=5)*world.spell_flags|=0x20;
            }
            *world.shared_spell_state=0;
        }
        return interrupt.timeout_script;
    }
    return nullptr;
}
}
