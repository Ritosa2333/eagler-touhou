#include "SpellCard.hpp"
namespace th10 {
// 0x408d60. Bonus decay precedes animation updates; UI thresholds use the
// advanced timer. Different spill points in the three smoothing axes matter.
i32 SpellCard::update(SpellEnvironment& env){
    if(!(spell_flags&1))return 1;
    if(elapsed.current>=60)(*env.stage)->draw_flags&=~1u;
#ifdef TH_ENABLE_THPRAC
    // 0x408d93: F4 patches the elapsed<300 branch to an unconditional jump,
    // so the bonus decay block is always skipped while the lock holds.
    if(elapsed.current>=300&&!(spell_flags&8)&&!env.time_locked()){
#else
    if(elapsed.current>=300&&!(spell_flags&8)){
#endif
        const auto decay=wrapping_add(initial_bonus,-initial_bonus/10)/wrapping_add(duration,-300);
        const auto remaining=wrapping_add(bonus,static_cast<i32>(0u-static_cast<u32>(decay)));bonus=wrapping_add(remaining,-remaining%10);
    }
    env.update_animation(backgrounds[0]);env.update_animation(backgrounds[1]);elapsed.tick();
    if(elapsed.current>=120){
        const bool hidden=(spell_flags&4)!=0;
        if((!hidden&&th10::number(env.player_position->y)<th10::number(96.0f))||(hidden&&th10::number(128.0f)<th10::number(env.player_position->y))){
            const std::int16_t interrupt=hidden?2:3;
            for(u32 id:title_animations)env.registry->interrupt(id,interrupt);
            for(auto& vm:bonus_digits)vm.pending_interrupt=interrupt;
            for(auto& vm:record_digits)vm.pending_interrupt=interrupt;
            if(hidden)spell_flags&=~4u;else spell_flags|=4;
        }
    }
    if(spell_flags&2){i32 digits=bonus,divisor=10000000;for(auto& vm:bonus_digits){const auto digit=digits/divisor;digits%=divisor;env.bind_digit(vm,wrapping_add(digit,30));env.update_animation(vm);divisor/=10;}}
    else{env.bind_digit(bonus_digits[2],41);env.update_animation(bonus_digits[2]);}
    auto captures=env.record(number,false).captures;if(captures>=100)captures=99;
    env.bind_digit(record_digits[0],captures/10+30);env.bind_digit(record_digits[1],captures%10+30);
    auto attempts=env.record(number,false).attempts;if(attempts>=100)attempts=99;
    env.bind_digit(record_digits[3],attempts/10+30);env.bind_digit(record_digits[4],attempts%10+30);
    for(auto& vm:record_digits)env.update_animation(vm);
    const auto& boss=env.boss_position();const auto dx=th10::number(boss.x)-th10::number(circle_position.x),dy=th10::number(boss.y)-th10::number(circle_position.y),dz=th10::number((th10::number(boss.z)-th10::number(circle_position.z)).to_float());const auto weight=th10::number(0.05f);
    const auto step_x=th10::number((dx*weight).to_float()),step_y=dy*weight,step_z=dz*weight;
    circle_position.x=(step_x+th10::number(circle_position.x)).to_float();circle_position.y=(step_y+th10::number(circle_position.y)).to_float();circle_position.z=(step_z+th10::number(circle_position.z)).to_float();
    env.registry->set_position(circle_animation,circle_position,true);return 1;
}
i32 SpellCard::draw_backgrounds(SpellEnvironment& env){if(spell_flags&1){env.draw_animation(backgrounds[0]);env.draw_animation(backgrounds[1]);}return 1;}
i32 SpellCard::draw_digits(SpellEnvironment& env){if(spell_flags&1){for(auto& vm:bonus_digits)env.draw_animation(vm);for(auto& vm:record_digits)env.draw_animation(vm);}return 1;}
}
