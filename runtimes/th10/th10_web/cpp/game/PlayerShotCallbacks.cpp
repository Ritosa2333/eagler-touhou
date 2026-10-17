#include "Player.hpp"
#include "Enemy.hpp"
#include <cmath>
namespace th10 {
// 0x428ad0. A target outside the horizontal playfield margin is ignored.
void Player::initialize_homing_shot(PlayerShot& shot){
    shot.homing_target=target;if(target&&number(224.f)<number(std::fabs(target->state.current.position.x)))shot.homing_target=nullptr;
}
// 0x428b10. Large turns slow the homing shot; sufficiently aligned shots speed up.
void Player::update_homing_shot(PlayerShot& shot){
    if(shot.state==2)return;
    if(shot.homing_target&&(shot.homing_target->state.flags&0xc0011))shot.homing_target=nullptr;
    if(!shot.homing_target){auto speed=number(shot.motion.speed)+number(.1f);if(number(16.f)<speed)speed=number(16.f);shot.motion.speed=speed.to_float();return;}
    const auto& target_position=shot.homing_target->state.current.position;
    const float desired=angle_to(number(target_position.y)-number(shot.motion.position.y),number(target_position.x)-number(shot.motion.position.x)).to_float();
    const float turn=angle_difference(desired,shot.motion.angle).to_float();auto speed=number(shot.motion.speed);
    if(shot.timer.current>=120)speed=speed+number(.2f);
    else{
        const auto magnitude=number(std::fabs(turn));
        if(number(.7853981852531433f)<magnitude||number(.7853981852531433f)==magnitude){speed=speed-number(.3f);if(speed<number(4.f))speed=number(4.f);}
        else if(magnitude<number(.2617993950843811f)){speed=speed+number(.1f);if(number(16.f)<speed)speed=number(16.f);}
        shot.motion.angle=normalize_angle((number(turn)*number(.2f)+number(shot.motion.angle)).to_float()).to_float();
    }
    shot.motion.speed=speed.to_float();
}
// 0x428c20. Lasers track their option before this frame's movement is applied.
void Player::update_option_laser(PlayerShot& shot){
    const auto point=options[shot.definition->option-1].position;auto& motion=shot.motion;
    motion.position={(Extended::from_int(point.x)*number(.01f)).to_float(),(Extended::from_int(point.y)*number(.01f)).to_float(),0};
    motion.position.x=(number(shot.definition->offset.x)-number(motion.velocity.x)+number(motion.position.x)).to_float();
    motion.position.y=(number(shot.definition->offset.y)-number(motion.velocity.y)+number(motion.position.y)).to_float();
}
// 0x428ca0, called before the target's storage is reclaimed.
void Player::forget_enemy(Enemy* enemy) noexcept {
    if(target==enemy){target=nullptr;target_seen=0;}for(auto& shot:shots)if(shot.homing_target==enemy)shot.homing_target=nullptr;
}
// Resource fixups in 0x426520. Callback tables are supplied by the platform
// integration; this method does not execute an address or use CPU state.
void PlayerProfile::resolve(const PlayerCallbackTables& callbacks){
    const auto diagonal=sine(Extended::from_double(.7853981852531433));
    fast_diagonal=(diagonal*number(fast_speed)).to_float();slow_diagonal=(diagonal*number(slow_speed)).to_float();
    for(u32 i=0;i<pattern_count;++i){
        auto& definitions=patterns[i].definitions;
        definitions=reinterpret_cast<PlayerShotDefinition*>(reinterpret_cast<uintptr_t>(this)+reinterpret_cast<uintptr_t>(definitions));
        for(auto* definition=definitions;definition->fire_interval>=0;++definition){
            definition->on_initialize=callbacks.initialize[definition->on_initialize];definition->on_update=callbacks.update[definition->on_update];
            definition->on_draw=callbacks.draw[definition->on_draw];definition->on_hit=callbacks.hit[definition->on_hit];
        }
    }
}
i32 Player::load_profile(const char* name,PlayerProfileEnvironment& env){profile=env.load(name);if(!profile)return -1;profile->resolve(env.callbacks);return 0;}
}
