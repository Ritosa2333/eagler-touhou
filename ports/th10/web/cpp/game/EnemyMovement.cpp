#include "EnemyVariables.hpp"
namespace th10 {
namespace {
Vec3 difference(const Vec3& a,const Vec3& b){
    return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};
}
void advance_velocity(Movement& movement,Vec3Interpolator& position,const float* rate){
    if(position.duration){movement.velocity=difference(sample(position,rate),movement.position);return;}
    if(movement.flags&1){
        movement.radius=Scalar::add(movement.radial_velocity,movement.radius);
        movement.set_angle(Scalar::add(movement.speed,movement.angle));
    }else{
        const auto velocity=polar(movement.angle,movement.speed);
        movement.velocity={velocity.x,velocity.y,0.0f};
    }
}
void clamp_coordinate(float& position,float center,float size){
    const auto half=number(Scalar::mul(size,0.5f));
    const auto low=number(center)-half;
    if(number(position)<low)position=low.to_float();
    else{const auto high=half+number(center);if(high<number(position))position=high.to_float();}
}
}
// Movement and visibility portion of 0x40dc80. Called once, before enemy ECL.
EnemyMotionResult EnemyState::advance_movement(EnemyEnvironment& env) noexcept {
    if(flags&0x400)return EnemyMotionResult::AlreadyUpdated;
    flags|=0x400;previous=current;
    if(absolute_angle.duration){const auto value=sample(absolute_angle,env.default_rate);absolute.set_angle(value.x);absolute.speed=value.y;}
    if(absolute_radius.duration){const auto value=sample(absolute_radius,env.default_rate);absolute.radius=value.x;absolute.radial_velocity=value.y;}
    if(relative_angle.duration){const auto value=sample(relative_angle,env.default_rate);relative.set_angle(value.x);relative.speed=value.y;}
    if(relative_radius.duration){const auto value=sample(relative_radius,env.default_rate);relative.radius=value.x;relative.radial_velocity=value.y;}
    advance_velocity(absolute,absolute_position,env.default_rate);
    advance_velocity(relative,relative_position,env.default_rate);
    absolute.update();
    if(flags&0x40000){
        relative.position.x=Scalar::add(env.camera_delta->x,relative.position.x);
        relative.position.y=Scalar::add(env.camera_delta->y,relative.position.y);
        relative.position.z=Scalar::add(env.camera_delta->z,relative.position.z);
    }
    relative.update();
    current.velocity.x=(number(relative.position.x)+number(absolute.position.x)-number(current.position.x)).to_float();
    current.velocity.y=(number(relative.position.y)+number(absolute.position.y)-number(current.position.y)).to_float();
    const auto z=number(Scalar::add(relative.position.z,absolute.position.z));
    current.velocity.z=(z-number(current.position.z)).to_float();
    current.update();
    if(flags&0x200){
        clamp_coordinate(current.position.x,clamp_center.x,clamp_size.x);
        clamp_coordinate(current.position.y,clamp_center.y,clamp_size.y);
        absolute.position=difference(current.position,relative.position);
    }
    const auto half_width=number(visual_size.x)*number(0.5f),half_height=number(visual_size.y)*number(0.5f);
    const auto x=number(current.position.x),y=number(current.position.y);
    const bool outside=half_width+x<number(-192.0f)||number(192.0f)<x-half_width||
        half_height+y<number(0.0f)||number(448.0f)<y-half_height;
    if(outside){if((flags&0x100)&&!(flags&4))return EnemyMotionResult::Despawn;}
    else flags|=0x100;
    return EnemyMotionResult::Continue;
}
}
