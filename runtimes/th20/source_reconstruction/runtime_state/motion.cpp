#include "motion.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
namespace th20::source::state {
namespace m=ecl::math;namespace n=recovered;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
sprite::Vec3 add(sprite::Vec3 a,sprite::Vec3 b){return {n::add32(a.x,b.x),n::add32(a.y,b.y),n::add32(a.z,b.z)};}
sprite::Vec3 subtract(sprite::Vec3 a,sprite::Vec3 b){return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z)};}
float damped(float value,float slowdown){return sub(value,n::mul32(value,slowdown));}
}
void update_motion_velocity(Motion& motion,float clock_scale){
    if(motion.field_44&32u)return;
    switch(motion.field_44&15u){
    case 0:
        m::polar(motion.vector_38.x,motion.vector_38.y,motion.angle_1c,n::mul32(clock_scale,motion.field_18));
        if(motion.field_34>0){const auto scaled=sprite::Vec3{n::mul32(motion.vector_38.x,motion.field_34),n::mul32(motion.vector_38.y,motion.field_34),n::mul32(motion.vector_38.z,motion.field_34)};motion.vector_38=subtract(motion.vector_38,scaled);}
        if(motion.field_44&16u)motion.angle_1c=m::wrap_angle(n::add32(damped(n::mul32(clock_scale,motion.field_24),motion.field_34),motion.angle_1c));
        break;
    case 2:case 3:
        motion.field_20=n::add32(damped(n::mul32(clock_scale,motion.field_24),motion.field_34),motion.field_20);
        motion.angle_1c=m::wrap_angle(n::add32(damped(n::mul32(clock_scale,motion.field_18),motion.field_34),motion.angle_1c));
        break;
    case 4:
        motion.angle_30=m::wrap_angle(n::add32(motion.angle_30,damped(n::mul32(clock_scale,motion.field_24),motion.field_34)));
        m::polar(motion.vector_38.x,motion.vector_38.y,motion.angle_28,n::mul32(clock_scale,motion.field_18));motion.vector_38.z=0;
        break;
    default:break;
    }
}
void snap_motion_position(Motion& motion){
    motion.position.x=div(static_cast<float>(std::floor(static_cast<double>(n::mul32(motion.position.x,100)))),100);
    motion.position.y=div(static_cast<float>(std::floor(static_cast<double>(n::mul32(motion.position.y,100)))),100);
}
void update_motion_position(Motion& motion,float clock_scale){
    if(motion.field_44&32u)return;
    sprite::Vec3 offset{};
    switch(motion.field_44&15u){
    case 0:motion.position=add(motion.position,motion.vector_38);break;
    case 2:
        m::polar(offset.x,offset.y,motion.angle_1c,motion.field_20);offset.z=0;motion.position=add(motion.vector_38,offset);break;
    case 3:
        // 4294e0 constructs an angle (429210 wraps once), then 453ac0
        // explicitly wraps again. Its bounded loops make both calls
        // observable when the first result still exceeds one revolution.
        m::polar(offset.x,offset.y,m::wrap_angle(m::wrap_angle(m::angle_difference(motion.angle_1c,motion.angle_28))),motion.field_20);
        offset.x=n::mul32(offset.x,motion.field_2c);m::rotate(offset.x,offset.y,motion.angle_28);offset.z=0;motion.position=add(motion.vector_38,offset);break;
    case 4:{
        const auto previous=motion.position;motion.velocity=add(motion.velocity,motion.vector_38);
        const auto angle=m::wrap_angle(m::wrap_angle(n::add32(motion.angle_28,1.5707963705062866f)));
        const auto amplitude=n::mul32(n::mul32(m::sine(motion.angle_30),motion.field_20),clock_scale);
        m::polar(offset.x,offset.y,angle,amplitude);offset.z=0;motion.position=add(offset,motion.velocity);
        const auto delta=subtract(motion.position,previous);motion.angle_1c=m::wrap_angle(m::arctangent(delta.y,delta.x));break;
    }
    default:break;
    }
    snap_motion_position(motion);
}
void update_motion(Motion& motion,float clock_scale){update_motion_velocity(motion,clock_scale);update_motion_position(motion,clock_scale);}
bool outside_motion_bounds(const Motion& motion,float x,float y,float width,float height){
    return sub(x,div(width,2))>motion.position.x||motion.position.x>n::add32(div(width,2),x)||sub(y,div(height,2))>motion.position.y||motion.position.y>n::add32(div(height,2),y);
}
}
