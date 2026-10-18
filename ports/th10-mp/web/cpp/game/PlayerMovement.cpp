#include "PlayerMovement.hpp"
namespace th10 {
namespace {
float pixels(i32 hundredths){return Scalar::mul_int(hundredths,.01f);}
i32 negate(i32 value){return static_cast<i32>(0u-static_cast<u32>(value));}
i32 clamp(i32 value,i32 minimum,i32 maximum){return value<minimum?minimum:value>maximum?maximum:value;}
}
// 0x428e10.
void Player::set_position(PlayerOption::FixedPoint point) noexcept {
    fixed_position=point;position.x=pixels(point.x);position.y=pixels(point.y);for(auto& option:options)option.snap_next=1;
}
void Player::update_position_history() noexcept {
    const i32 limit=static_cast<i32>(static_cast<u32>(option_count)*8u);i32 tail=32;
    while(tail>limit){position_history[tail]=position_history[limit];--tail;}
    while(tail>0){position_history[tail]=position_history[tail-1];--tail;}
}
// 0x4250b0: movement is stored as integer hundredths before the sprite positions.
i32 Player::move(PlayerMovementEnvironment& env){
    const u32 keys=*env.input_keys;
    direction=(keys&0x50)==0x50?5:(keys&0x60)==0x60?7:(keys&0x90)==0x90?6:(keys&0xa0)==0xa0?8:
              keys&0x20?2:keys&0x10?1:keys&0x40?3:keys&0x80?4:0;
    if(!env.enemy_count||!*env.enemy_count||focus_timer.current<4){focused=0;option_follow_speed=30;}
    else{
        focused=(keys>>2)&1;
        if(wrapping_add(env.economy->shot_type,static_cast<i32>(static_cast<u32>(env.economy->character)*3u))==5){
            if(keys&4)option_follow_speed=0;else if(option_follow_speed<30)option_follow_speed=wrapping_add(option_follow_speed,1);
        }
    }
    const bool show_hitbox=focused||(env.always_hitbox&&*env.always_hitbox);
    if(!show_hitbox){if(env.manager->registry.find(focus_animation))env.manager->registry.interrupt(focus_animation,1);focus_animation=0;}
    else if(!focus_animation)focus_animation=env.manager->create(*env.effect_file,0x160,9,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
    const i32 straight=focused?slow_speed:fast_speed,diagonal=focused?slow_diagonal:fast_diagonal;
    i32 x=0,y=0;
    switch(direction){
    case 1:y=negate(straight);break;case 2:y=straight;break;case 3:x=negate(straight);break;case 4:x=straight;break;
    case 5:x=y=negate(diagonal);break;case 6:x=diagonal;y=negate(diagonal);break;case 7:x=negate(diagonal);y=diagonal;break;case 8:x=y=diagonal;break;
    }
    if(env.movement(*this,straight,x,y))direction=x<0?(y<0?5:y>0?7:3):x>0?(y<0?6:y>0?8:4):y<0?1:y>0?2:0;
    i32 animation_index=-1;
    if(x<0&&input_velocity.x>=0)animation_index=1;
    else if(x>0&&input_velocity.x<=0)animation_index=3;
    else if(x==0&&input_velocity.x<0)animation_index=2;
    else if(x==0&&input_velocity.x>0)animation_index=4;
    if(animation_index>=0)animation_file->initialize_script(animation,animation_index,*env.animations,env.manager->started_scripts);
    input_velocity={x,y};velocity={Scalar::mul_int_truncate(x,*env.default_rate),Scalar::mul_int_truncate(y,*env.default_rate)};
    fixed_position={clamp(wrapping_add(fixed_position.x,velocity.x),-18400,18400),clamp(wrapping_add(fixed_position.y,velocity.y),3200,43200)};
    position.x=pixels(fixed_position.x);position.y=pixels(fixed_position.y);
    if(env.manager->registry.find_and_clear(focus_animation))env.manager->registry.set_position(focus_animation,position,true);
    if(!focused&&(x||y))update_position_history();position_history[0]=fixed_position;
    for(auto& option:options)if(option.active){
        const auto& offset=focused?option.focused_offset:option.offset;option.target={wrapping_add(offset.x,fixed_position.x),wrapping_add(offset.y,fixed_position.y)};
        if(option.on_update)env.update_option(option);
        if(option.snap_next){option.snap_next=0;option.position=option.target;}
        else if(option_follow_speed>=30){
            const i32 dx=static_cast<i32>(static_cast<u32>(wrapping_add(option.target.x,negate(option.position.x)))*static_cast<u32>(option_follow_speed))/100;
            const i32 dy=static_cast<i32>(static_cast<u32>(wrapping_add(option.target.y,negate(option.position.y)))*static_cast<u32>(option_follow_speed))/100;
            if(dx||dy){option.position.x=wrapping_add(option.position.x,dx);option.position.y=wrapping_add(option.position.y,dy);}else option.position=option.target;
        }
        const Vec3 point{pixels(option.position.x),pixels(option.position.y),0};
        env.manager->registry.set_position(option.animations[0],point,true);env.manager->registry.set_position(option.animations[1],point,true);
    }
    return 0;
}
}
