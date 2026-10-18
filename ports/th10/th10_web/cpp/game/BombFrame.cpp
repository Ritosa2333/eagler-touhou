#include "BombEnvironment.hpp"
namespace th10 {
namespace {
bool special_card(const BombEnvironment& env){return ((*env.shared->spell_flags&1)&&*env.spell_number>=93&&*env.spell_number<=96)||*env.spell_number==109;}
}
// 0x405860. Certain spell cards select a separate bomb animation and cancellation.
i32 Bomb::start(BombEnvironment& env){
    if(active)return -1;active=1;auto& shared=*env.shared;
    if(!(timer_flags&1)){timer_flags|=1;timer.rate=shared.default_rate;}timer.initialize(-1);
    radius=32.f;radial_speed=4.f;position=*env.player_position;
    const i32 script=special_card(env)?0x1b9:shared.economy->character?0x197:0x190;
    animation=shared.manager->create_at(*shared.effect_file,script,position,true,AnimationPlacement::WorldBack,*shared.animations,*shared.allocation);
    env.play_sound(38,position.x);const i32 power=shared.economy->power;env.update_power(power/20,(power%20)*100/20);
    shared.cancel_spell_capture();shared.economy->add_rank(-128);mode=(*shared.spell_flags&1)&&*shared.spell_elapsed>=60;return 0;
}
// 0x405ac0. Laser conversion reads spell flags again after bullet cancellation.
i32 Bomb::cancel_projectiles(BombEnvironment& env){
    if(active){env.cancel_bullets(position,radius,!(*env.shared->spell_flags&1),!special_card(env));env.cancel_lasers(position,radius,!(*env.shared->spell_flags&1));}return 0;
}
// 0x405750: the effect animation determines both lifetime and damage radius.
i32 Bomb::update(BombEnvironment& env){
    if(!active)return 1;auto& shared=*env.shared;
    if(active==1){
        auto* sprite=shared.manager->registry.find_and_clear(animation);if(!sprite){active=0;return 1;}
        const bool slow=(*shared.spell_flags&1)&&((*env.spell_number>=93&&*env.spell_number<=96)||*env.spell_number==109);
        position.y=Scalar::sub(position.y,slow?.5f:shared.economy->character?1.3f:1.f);
        shared.manager->registry.set_position(animation,position,true);radius=sprite->scale.y;cancel_projectiles(env);
    }
    timer.tick();return 1;
}
}
