#include "PlayerShooting.hpp"
namespace th10 {
namespace {
void seek_timer(Timer& timer,u32& flags,i32 frame,const float* rate){
    if(!(flags&1)){flags|=1;timer.rate=rate;}
    timer.previous=wrapping_add(frame,-1);timer.current=frame;timer.fractional=Extended::from_int(frame).to_float();
}
void set_sprite_position(AnmVm& vm,const Vec3& point){
    vm.position={Scalar::add(point.x,224.f),Scalar::add(point.y,16.f),point.z};
}
}
// 0x428d70. An unordered comparison follows the original x87 inside branch.
bool outside_playfield(const Vec3& position,float half_width,float half_height) noexcept {
    const auto left=number(half_width)+number(position.x),right=number(position.x)-number(half_width);
    const auto top=number(half_height)+number(position.y),bottom=number(position.y)-number(half_height);
    return left<number(-192.f)||left==number(-192.f)||number(192.f)<right||right==number(192.f)||
           top<number(0.f)||top==number(0.f)||number(448.f)<bottom||bottom==number(448.f);
}
// 0x427e90. Reusing a slot retains fields that the original does not initialize.
i32 Player::spawn_shot(PlayerShotDefinition& definition,i32 frame,PlayerShootingEnvironment& env){
    if(definition.type==3&&active_lasers[definition.option])return 0;
    PlayerShot* shot=nullptr;for(auto& candidate:shots)if(!candidate.state){shot=&candidate;break;}if(!shot)return 0;
    shot->state=1;shot->definition=&definition;seek_timer(shot->timer,shot->timer_flags,0,env.default_rate);
    auto& motion=shot->motion;
    if(!definition.option)motion.position=position;
    else{const auto point=options[definition.option-1].position;motion.position={(Extended::from_int(point.x)*number(.01f)).to_float(),(Extended::from_int(point.y)*number(.01f)).to_float(),0};}
    if(definition.type==3)active_lasers[definition.option]=1;
    motion.speed=definition.speed;motion.angle=normalize_angle(definition.angle).to_float();motion.update_velocity();
    motion.position.x=(number(definition.offset.x)-number(motion.velocity.x)+number(motion.position.x)).to_float();
    motion.position.y=(number(definition.offset.y)-number(motion.velocity.y)+number(motion.position.y)).to_float();
    shot->animation=env.manager->create(*animation_file,definition.animation+5,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
    auto* sprite=env.manager->registry.find_and_clear(shot->animation);
    if(sprite->flags&0x8000000){sprite->rotation.z=definition.angle;sprite->flags|=4;}
    shot->secondary_animation=definition.type==3?env.manager->create(*animation_file,0x10,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation):0;
    if(definition.on_initialize)env.initialize_shot(*this,*shot,frame);
    if(definition.sound>=0)env.play_shot_sound(definition.sound,motion.position.x);
    return 0;
}
// 0x428160: each power/focus band is a sentinel-terminated shot definition list.
i32 Player::fire_pattern(i32 frame,PlayerShootingEnvironment& env){
    i32 band=static_cast<u16>(env.economy->power)/20;if(band>=5)band=4;if(focused)band+=5;
    for(auto* definition=profile->patterns[band].definitions;definition->fire_interval>=0;++definition)
        if(frame%definition->fire_interval==definition->fire_offset)spawn_shot(*definition,frame,env);
    return 0;
}
// 0x4281d0. The fifteen-frame firing cycle continues to finish after key release.
i32 Player::update_firing(PlayerShootingEnvironment& env){
    if(state!=1){target=nullptr;target_seen=0;return 0;}
    if(fire_timer.current<0){if(!(*env.input_keys&1))return 0;seek_timer(fire_timer,fire_timer_flags,0,env.default_rate);}
    if(fire_timer.current!=fire_timer.previous)fire_pattern(fire_timer.current,env);
    if(fire_timer.current>=15){
        if(*env.input_keys&1)fire_timer.advance(-15.f);else seek_timer(fire_timer,fire_timer_flags,-1,env.default_rate);
    }else fire_timer.tick();
    return 0;
}
// 0x428280: lasers, bullets and their animation lifetime share this ordered pool.
i32 Player::update_shots(PlayerShootingEnvironment& env){
    auto& registry=env.manager->registry;
    for(auto& shot:shots)if(shot.state){
        if(shot.definition->type==3&&shot.state==1&&(fire_timer.current<0||shot.definition->option-1>=option_count)){
            registry.interrupt(shot.animation,1);registry.interrupt(shot.secondary_animation,1);shot.state=2;active_lasers[shot.definition->option]=0;
        }
        if(shot.definition->type==3&&shot.state==1&&(env.dialogue_active||!env.enemy_manager_present)){
            shot.state=2;registry.interrupt(shot.animation,1);registry.interrupt(shot.secondary_animation,1);active_lasers[shot.definition->option]=0;
        }
        if(shot.definition->type==3&&!shot.collided&&shot.state==1&&shot.first_collision==1){registry.interrupt(shot.animation,3);shot.first_collision=0;}
        shot.collided=0;if(shot.definition->on_update)env.update_shot(*this,shot);
        auto& motion=shot.motion;motion.update_velocity();motion.update();
        auto* sprite=registry.find(shot.animation);
        if(!sprite){shot.state=0;registry.request_delete(shot.secondary_animation);shot.animation=shot.secondary_animation=0;continue;}
        if(shot.definition->type!=3&&shot.timer.current>=10&&outside_playfield(motion.position,
                Scalar::mul(sprite->sprite->width,sprite->scale.x),Scalar::mul(sprite->sprite->height,sprite->scale.y))){
            registry.request_delete(shot.animation);shot.animation=0;shot.state=0;continue;
        }
        set_sprite_position(*sprite,motion.position);
        if(auto* secondary=registry.find_and_clear(shot.secondary_animation))set_sprite_position(*secondary,motion.position);
        if(sprite->flags&0x8000000){sprite->rotation.z=motion.angle;sprite->flags|=4;}
        shot.timer.tick();
    }
    return 0;
}
}
