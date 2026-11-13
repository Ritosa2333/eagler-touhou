#include "EnemyState.hpp"
#include "HudScalarTh14.hpp"
namespace th11 {
#if PORT_TITLE == 16
// Retail424f00 and425010 are separate consumers. The frame checks time before
// collecting shot damage, and checks health only after receiving damage.
const char* EnemyState::check_health_interrupt16(EnemyPhaseState& world)noexcept{
    interrupt_health=health;health_baseline=0;
    for(auto& interrupt:interrupts){
        if(interrupt.health<0)continue;
        interrupt_health=wrapping_sub(health,interrupt.health);health_baseline=interrupt.health;
        if(health>interrupt.health)return nullptr;
        if(th16_chapter_credit&&world.chapter&&world.chapter_removed&&th16_chapter_tag==*world.chapter){
            *world.chapter_removed=wrapping_add(*world.chapter_removed,th16_chapter_credit);th16_chapter_credit=0;
        }
        health=interrupt.health;interrupt.health=-1;th16_phase_timer.set(0,world.rate);flags&=~0x1000000u;
        return interrupt.health_script;
    }
    return nullptr;
}
const char* EnemyState::check_timed_interrupt16(EnemyPhaseState& world)noexcept{
    for(auto& interrupt:interrupts){
        if(interrupt.health<0||interrupt.time<=0)continue;
        if(flags&0x800000){
            const i32 remaining=wrapping_sub(interrupt.time,th16_phase_timer.current),seconds=remaining/60,hundredths=(remaining%60)*100/60;
            th14_hud_store(*world.countdown_seconds,seconds>99?99:seconds);*world.countdown_hundredths=seconds>99?99:hundredths;
        }
        if(th16_phase_timer.current<interrupt.time)return nullptr;
        health=interrupt.health;interrupt.health=-1;th16_phase_timer.set(0,world.rate);flags|=0x1000000;
        if(!(*world.spell_flags&8)){
            flags&=~0x1000000u;*world.spell_flags|=0x80;
            if(*world.spell_flags&1){
                if(*world.spell_elapsed>=60){*world.spell_bonus=0;*world.spell_flags&=~0x22u;}
                else if(world.special_active==1)*world.spell_flags|=0x20;
            }
            *world.shared_spell_state=0;
        }else if((*world.spell_flags&9)==9&&world.chapter_removed){
            *world.chapter_removed=wrapping_add(*world.chapter_removed,th16_chapter_credit);
        }
        th16_chapter_credit=0;return interrupt.timeout_script;
    }
    return nullptr;
}
#endif
// 0x4100e0: spell damage is accumulated in sevenths with signed wrapping.
i32 EnemyState::take_damage(i32 amount)noexcept{
#if PORT_TITLE == 16
    th16_damage_total=wrapping_add(th16_damage_total,amount); // Retail41a6d9.
#endif
    if(health_flags&1){scaled_health=wrapping_sub(scaled_health,amount);const i32 difference=wrapping_sub(scaled_health,signed_bits(u32(health_baseline)*7));health=wrapping_add(difference/7,health_baseline);}
    else health=wrapping_sub(health,amount);
    return health;
}
// 0x417b40. The first active health interrupt takes precedence; the first
// active timed interrupt is then considered independently.
const char* EnemyState::check_interrupts(EnemyPhaseState& world)noexcept{
#if PORT_TITLE == 16
    if(const char* name=check_health_interrupt16(world))return name;
    return check_timed_interrupt16(world);
#endif
#if PORT_TITLE == 14
    auto& clock=timer_2bc;constexpr u32 timeout_flag=0x1000000;
#else
    auto& clock=lifetime;constexpr u32 timeout_flag=0x100000;
#endif
    interrupt_health=health;health_baseline=0;
    for(auto& interrupt:interrupts){
        if(interrupt.health<0)continue;
        interrupt_health=wrapping_sub(health,interrupt.health);health_baseline=interrupt.health;
        if(health<=interrupt.health){health=interrupt.health;interrupt.health=-1;clock.set(0,world.rate);flags&=~timeout_flag;return interrupt.health_script;}
        break;
    }
    for(auto& interrupt:interrupts){
        if(interrupt.health<0||interrupt.time<1)continue;
        if(kPortTitle!=14||(flags&0x800000)){
            const i32 remaining=wrapping_sub(interrupt.time,clock.current),seconds=remaining/60,hundredths=(remaining%60)*100/60;
            th14_hud_store(*world.countdown_seconds,seconds>99?99:seconds);*world.countdown_hundredths=seconds>99?99:hundredths;
        }
        if(clock.current<interrupt.time)return nullptr;
        health=interrupt.health;interrupt.health=-1;clock.set(0,world.rate);flags|=timeout_flag;
        if(!(*world.spell_flags&8)){
            if constexpr(kPortTitle==14)*world.spell_flags|=0x80;
            if(*world.spell_flags&1){
                if(*world.spell_elapsed>=60){*world.spell_bonus=0;*world.spell_flags&=~0x22u;}
                else if(kPortTitle==14?world.special_active==1:(world.special_active&&signed_bits(u32(world.character)*2+u32(world.subtype))!=5))*world.spell_flags|=0x20;
            }
            *world.shared_spell_state=0;
        }
        return interrupt.timeout_script;
    }
    return nullptr;
}
}
