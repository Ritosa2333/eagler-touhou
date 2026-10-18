#include "LaserBehavior.hpp"
#include "GameMath.hpp"
#include "HighRefresh.hpp"
namespace th10 {
namespace {
void seek(Timer& timer,u32& flags,i32 value,const float* rate){
    if(!(flags&1)){flags|=1;timer.rate=rate;}
    timer.previous=wrapping_add(value,-1);timer.current=value;timer.fractional=Extended::from_int(value).to_float();
}
void move(Vec3& point,const Vec3& velocity,float rate){
    const auto x=number(rate)*number(velocity.x),y=number(rate)*number(velocity.y);
    const float z=Scalar::mul(rate,velocity.z);
    point.x=(x+number(point.x)).to_float();point.y=(y+number(point.y)).to_float();point.z=Scalar::add(z,point.z);
}
Vec3 point_along(const EnemyLaser& laser,float length){
    const auto offset=polar(laser.angle,length);
    return {Scalar::add(offset.x,laser.position.x),Scalar::add(offset.y,laser.position.y),laser.position.z};
}
void start_animations(AnmVm& beam,AnmVm& tip,i32 type,i32 color,bool additive,LaserBehaviorEnvironment& env){
    initialize_embedded_animation(*env.animation_file,beam,wrapping_add(env.sprite_scripts[type],color),*env.animations,*env.started_animations);
    beam.pending_interrupt=2;beam.update(*env.animations);
    if(additive)beam.flags=(beam.flags&~0x20u)|0x10;
    beam.flags=(beam.flags&0xfc63ffffu)|0x600000;
    initialize_embedded_animation(*env.animation_file,tip,wrapping_add(color,0x103),*env.animations,*env.started_animations);
    tip.pending_interrupt=2;tip.update(*env.animations);tip.flags=(tip.flags&~0x20u)|0x10;
    tip.flags=(tip.flags&0xfc7fffffu)|0x400000;
}
void collide(EnemyLaser& laser,float wide_scale,LaserBehaviorEnvironment& env){
    const auto origin=point_along(laser,Scalar::mul(laser.length,.1f));
    const auto width=number(laser.width);
    const float hit_width=(laser.width<32.f?width*number(.5f):width-(width+number(16.f))*number(wide_scale)).to_float();
    const i32 result=env.collide_player(origin,laser.angle,hit_width,Scalar::mul(laser.length,.8f));
    if(result==1)env.cancel_rectangle(laser,*env.player_position,{32,32,0},0);
    else if(result==2&&laser.timer.current%5==0){env.graze_effect(*env.player_position);env.play_sound(28,laser.position.x);}
}
void animate(EnemyLaser& laser,AnmVm& beam,AnmVm& tip,LaserBehaviorEnvironment& env){
    const auto scale_x=number(laser.width)/number(beam.sprite->width);beam.flags|=8;beam.scale.x=scale_x.to_float();
    const auto scale_y=number(laser.length)/number(beam.sprite->height);beam.flags|=8;beam.scale.y=scale_y.to_float();
    beam.update(*env.animations);if(laser.distance_travelled==0.f)tip.update(*env.animations);
}
i32 draw_laser(EnemyLaser& laser,AnmVm& beam,AnmVm& tip,LaserBehaviorEnvironment& env){
    Vec3 position=laser.position;float angle=laser.angle,length=laser.length,width=laser.width;AnmVm beam_copy,tip_copy;AnmVm* draw_beam=&beam,*draw_tip=&tip;
    if(high_refresh::render_only){beam_copy=beam;tip_copy=tip;draw_beam=&beam_copy;draw_tip=&tip_copy;}const bool interpolated=env.presentation(laser,position,angle,length,width);if(interpolated&&draw_beam->sprite){draw_beam->scale.x=Scalar::div(width,draw_beam->sprite->width);draw_beam->scale.y=Scalar::div(length,draw_beam->sprite->height);draw_beam->flags|=8;}
    draw_beam->script_position={Scalar::add(position.x,224.f),Scalar::add(position.y,16.f),position.z};
    draw_beam->rotation.z=add_angle(angle,1.57079637050628662109375f).to_float();draw_beam->flags|=4;env.submit(*draw_beam);
    if(laser.distance_travelled==0.f){draw_tip->script_position={Scalar::add(position.x,224.f),Scalar::add(position.y,16.f),position.z};env.submit(*draw_tip);}
    return 0;
}
}
bool laser_outside_playfield(const Vec3& point,float x,float y) noexcept {
    const auto left=number(x)+number(point.x),right=number(point.x)-number(x),top=number(y)+number(point.y),bottom=number(point.y)-number(y);
    return left<number(-192.f)||left==number(-192.f)||number(192.f)<right||right==number(192.f)||
           top<number(0.f)||top==number(0.f)||number(448.f)<bottom||bottom==number(448.f);
}
// 0x41c8c0 / 0x41e5c0. Parameters are copied before initializing animations.
i32 StraightLaser::start(const StraightLaserParameters& values,LaserBehaviorEnvironment& env){
    parameters=values;base.state=2;
    start_animations(beam,tip,parameters.sprite_type,parameters.color,parameters.flags&1,env);
    seek(base.outside_delay,base.outside_delay_flags,30,env.rate);
    base.length=parameters.initial_length;base.position=parameters.position;base.angle=parameters.angle;
    base.speed=parameters.speed;base.width=parameters.width;base.turn_sound=24;
    base.distance_travelled=parameters.initial_length>0.f?.01f:0.f;
    const auto velocity=polar(base.angle,base.speed);base.velocity.x=velocity.x;base.velocity.y=velocity.y;
    return 0;
}
i32 TimedLaser::start(const TimedLaserParameters& values,LaserBehaviorEnvironment& env){
    parameters=values;base.state=3;
    start_animations(beam,tip,parameters.sprite_type,parameters.color,parameters.flags&2,env);
    base.position=parameters.position;base.angle=parameters.angle;base.turn_sound=24;
    base.length=parameters.initial_length;base.width=2.f;base.speed=parameters.growth_speed;
    return 0;
}
// 0x41d3d0: straight lasers extend before their origin starts moving.
i32 StraightLaser::update(LaserBehaviorEnvironment& env){
    env.run_commands(*this);
    if(base.active_features){
        constexpr u32 masks[]={1,0x10,0x20,0x40,0x100,0x80,0x8000c00,0x100000,0x200000,0x4000000};
        for(u32 i=0;i<10;i++)if(base.active_features&masks[i])env.update_feature(*this,static_cast<LaserFeature>(i));
        if(base.active_features&0x8000){if(base.feature_delay.current<=0)base.active_features^=0x8000;else base.feature_delay.advance(-1.f);}
    }
    const auto distance=number(*env.rate)*number(base.speed);
    if(base.length<parameters.target_length){
        const auto length=distance+number(base.length);base.length=length.to_float();if(number(parameters.target_length)<length)base.length=parameters.target_length;
    }else{
        base.distance_travelled=(distance+number(base.distance_travelled)).to_float();move(base.position,base.velocity,*env.rate);
        if(parameters.maximum_distance>0.f&&number(parameters.maximum_distance)<number(base.distance_travelled)+number(base.length)){
            const auto length=number(parameters.maximum_distance)-number(base.distance_travelled);base.length=length.to_float();parameters.target_length=length.to_float();if(base.length<=0.f)return 1;
        }
    }
    if(base.outside_delay.current<=0){
        const auto end=point_along(base,base.length);
        if(laser_outside_playfield(base.position,base.width,base.width)&&laser_outside_playfield(end,base.width,base.width))return 1;
    }else base.outside_delay.advance(-1.f);
    if(base.length>16.f&&base.width>3.f)collide(base,.5f,env);
    animate(base,beam,tip,env);return 0;
}
// 0x41e700: warning -> expansion -> active -> contraction. Some transitions
// deliberately fall through so zero-duration phases finish in the same frame.
i32 TimedLaser::update(LaserBehaviorEnvironment& env){
    if(base.length<parameters.target_length){
        const auto length=number(*env.rate)*number(base.speed)+number(base.length);base.length=length.to_float();if(number(parameters.target_length)<length)base.length=parameters.target_length;
    }
    base.angle=add_angle(base.angle,Scalar::mul(*env.rate,parameters.angular_velocity)).to_float();
    if(parameters.flags&1&&env.boss_position)base.position=*env.boss_position;
    move(base.position,parameters.velocity,*env.rate);
    switch(base.state){
    case 3:if(base.timer.current>=parameters.warning_frames){seek(base.timer,base.timer_flags,0,env.rate);base.state=4;}break;
    case 4:
        if(base.timer.current<parameters.grow_frames){base.width=(number(parameters.width)*number(base.timer.fractional)/Extended::from_int(parameters.grow_frames)).to_float();break;}
        seek(base.timer,base.timer_flags,0,env.rate);base.state=2;base.width=parameters.width;
        [[fallthrough]];
    case 2:
        if(base.timer.current<parameters.active_frames)break;
        seek(base.timer,base.timer_flags,0,env.rate);base.state=5;
        [[fallthrough]];
    case 5:
        if(base.timer.current>=parameters.shrink_frames)return 1;
        base.width=(number(parameters.width)-number(parameters.width)*number(base.timer.fractional)/Extended::from_int(parameters.shrink_frames)).to_float();break;
    }
    if((base.state==4||base.state==2)&&base.length>16.f)collide(base,.33333334f,env);
    animate(base,beam,tip,env);return 0;
}
i32 StraightLaser::draw(LaserBehaviorEnvironment& env){return draw_laser(base,beam,tip,env);}
i32 TimedLaser::draw(LaserBehaviorEnvironment& env){return draw_laser(base,beam,tip,env);}
}
