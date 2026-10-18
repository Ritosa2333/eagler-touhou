#include "EnemyPhase.hpp"
namespace th10 {
// 0x4127a0. Health interrupts take priority. Timeout scanning independently
// selects the first still-active interrupt with a positive time limit.
const char* EnemyState::check_interrupts(EnemyPhaseState& world) noexcept {
    const auto reset_timer=[&]{
        if(!(lifetime_flags&1)){lifetime.rate=world.default_rate;lifetime_flags|=1;}
        lifetime.initialize(-1);
    };
    for(auto& interrupt:interrupts){
        if(interrupt.health<0)continue;
        health_to_interrupt=wrapping_add(health,static_cast<i32>(0u-static_cast<u32>(interrupt.health)));
        if(health<=interrupt.health){
            health=interrupt.health;interrupt.health=-1;reset_timer();flags&=~0x10000u;
            return interrupt.health_subroutine;
        }
        break;
    }
    for(auto& interrupt:interrupts){
        if(interrupt.health<0||interrupt.time<1)continue;
        const auto remaining=wrapping_add(wrapping_add(interrupt.time,static_cast<i32>(0u-static_cast<u32>(lifetime.current))),59)/60;
        *world.countdown=remaining>99?99:remaining;
        if(lifetime.current<interrupt.time)return nullptr;
        health=interrupt.health;interrupt.health=-1;reset_timer();flags|=0x10000;
        *world.item_value=wrapping_add(*world.item_value,-3000);
        if(*world.item_value<5000)*world.item_value=5000;
        if(!(*world.spell_flags&8)&&*world.spell_elapsed>59){
            *world.spell_bonus=0;*world.spell_flags&=~2u;
            for(auto* animation:world.spell_animation_flags)*animation&=~2u;
        }
        return interrupt.time_subroutine;
    }
    return nullptr;
}
}
