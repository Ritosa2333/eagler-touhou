#include "EnemyState.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
void EnemyState::recompose()noexcept{
    motion.velocity={float(float(relative.position.x+absolute.position.x)-motion.position.x),float(float(relative.position.y+absolute.position.y)-motion.position.y),float(float(relative.position.z+absolute.position.z)-motion.position.z)};motion.advance();
    if(flags&0x20000){const float half_width=float(bound_size.x*.5f),half_height=float(bound_size.y*.5f);const float left=float(bound_center.x-half_width),right=float(bound_center.x+half_width),top=float(bound_center.y-half_height),bottom=float(bound_center.y+half_height);if(left>motion.position.x)motion.position.x=left;else if(motion.position.x>right)motion.position.x=right;if(top>motion.position.y)motion.position.y=top;else if(motion.position.y>bottom)motion.position.y=bottom;absolute.position={float(motion.position.x-relative.position.x),float(motion.position.y-relative.position.y),float(motion.position.z-relative.position.z)};}
}
EnemyMovementResult EnemyState::update_movement(float rate,const Vec3& background_delta,EnemyAnimationHost* animations){
    previous=motion;
    if(absolute_angle.duration&&(absolute.flags&15)!=2&&(absolute.flags&15)!=3)absolute.angle=normalize_angle(normalize_angle(absolute_angle.step(rate)[0]));
    if(absolute_speed.duration)absolute.speed=absolute_speed.step(rate)[0];
    if(relative_angle.duration&&(relative.flags&15)!=2&&(relative.flags&15)!=3)relative.angle=normalize_angle(normalize_angle(relative_angle.step(rate)[0]));
    if(relative_speed.duration)relative.speed=relative_speed.step(rate)[0];
    if(absolute_shape.duration){const auto v=absolute_shape.step(rate);absolute.radius=v[0];absolute.angular_velocity=v[1];}
    if(relative_shape.duration){const auto v=relative_shape.step(rate);relative.radius=v[0];relative.angular_velocity=v[1];}
    if(absolute_position.duration){const auto v=absolute_position.step(rate);absolute.velocity={float(v[0]-absolute.position.x),float(v[1]-absolute.position.y),float(v[2]-absolute.position.z)};}else absolute.integrate(rate);
    if(relative_position.duration){const auto v=relative_position.step(rate);relative.velocity={float(v[0]-relative.position.x),float(v[1]-relative.position.y),float(v[2]-relative.position.z)};}else relative.integrate(rate);
    absolute.advance(rate);
    if(flags&0x4000000)relative.position={float(relative.position.x+background_delta.x),float(relative.position.y+background_delta.y),float(relative.position.z+background_delta.z)};
    relative.advance(rate);recompose();
    if(flags&0x100000){
        const i32 direction=motion.velocity.x<-.03f?-1:motion.velocity.x>.03f?1:0;
        if(direction!=movement_direction){
            i32 script=0;
            if(movement_direction==-1)script=direction?2:3;
            else if(movement_direction==0)script=direction==-1?1:2;
            else if(movement_direction==1)script=direction?1:4;
            if(!animations||!animations->change_direction(animation_handles[0],animation_resource,wrapping_add(animation_base,script),wrapping_add(animation_layer,7)))return EnemyMovementResult::animation_unavailable;
            movement_direction=direction;
        }
    }
    if(animations){Vec2 size;if(animations->size(animation_handles[0],size))sprite_size={std::fabs(size.x),std::fabs(size.y)};else animation_handles[0]=0;}else if(animation_handles[0])return EnemyMovementResult::animation_unavailable;
    const float half_width=float(sprite_size.x*.5f),half_height=float(sprite_size.y*.5f);
    if(-192.f>float(motion.position.x+half_width)||float(motion.position.x-half_width)>192.f)return (flags&0x10000)&&!(flags&4)?EnemyMovementResult::outside:EnemyMovementResult::active;
    if(0.f>float(motion.position.y+half_height)||float(motion.position.y-half_height)>448.f)return (flags&0x10000)&&!(flags&8)?EnemyMovementResult::outside:EnemyMovementResult::active;
    flags|=0x10000;return EnemyMovementResult::active;
}
}
