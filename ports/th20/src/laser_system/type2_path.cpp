#include "type2.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
namespace th20::source::laser {
namespace {
namespace n=recovered;namespace m=ecl::math;
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
sprite::Vec3 scale(sprite::Vec3 p,float v){return {n::mul32(p.x,v),n::mul32(p.y,v),n::mul32(p.z,v)};}
sprite::Vec3 plus(sprite::Vec3 a,const sprite::Vec3& b){return {n::add32(a.x,b.x),n::add32(a.y,b.y),n::add32(a.z,b.z)};}
sprite::Vec3 minus(sprite::Vec3 a,const sprite::Vec3& b){return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z)};}
sprite::Vec3 polar(float a,float s){sprite::Vec3 p{};m::polar(p.x,p.y,a,s);return p;}
float floor_value(float x){return static_cast<float>(std::floor(static_cast<double>(x)));} //4592b0
void magnitude_angle(const sprite::Vec3& v,float& s,float& a){s=m::square_root(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)));a=m::arctangent(v.y,v.x);}
}
void sample_curve_absolute(const CurveNode& c,sprite::Vec3& p,float& speed,float& angle,float time){
    const float t=sub(time,c.begin);
    switch(c.kind){
    case 0:p=plus(c.position,scale(scale(c.direction,t),c.speed));speed=c.speed;angle=c.angle;break;
    case 1:
        // COMISS threshold, angular followed by JBE includes unordered inputs.
        if(!(c.angular_acceleration < -990.0f)){
            auto v=plus(polar(c.angle,c.speed),polar(c.angular_acceleration,c.acceleration));p=plus(c.position,scale(v,t));magnitude_angle(v,speed,angle);
        }else{
            auto v=scale(scale(c.direction,n::add32(n::mul32(t,c.acceleration),n::mul32(c.speed,2))),n::add32(t,1));
            v={div(v.x,2),div(v.y,2),div(v.z,2)};p=plus(c.position,v);speed=n::add32(n::mul32(c.acceleration,t),c.speed);angle=c.angle;
        }break;
    case 2:{
        auto pos=c.position;float a=c.angle,s=c.speed;
        const auto steps=_mm_cvtt_ss2si(_mm_set_ss(t));
        for(std::int32_t i=0;i<steps;++i){auto v=polar(a,s);a=m::wrap_angle(n::add32(a,c.angular_acceleration));s=n::add32(s,c.acceleration);pos=plus(pos,v);}
        p=plus(pos,scale(polar(a,s),sub(t,floor_value(t))));speed=s;angle=a;break;
    }
    }
}
void sample_curve_backwards(const CurveNode& c,sprite::Vec3& p,float& speed,float& angle,const sprite::Vec3& previous,float previous_speed,float previous_angle,float time){
    switch(c.kind){
    case 0:p=minus(previous,scale(c.direction,c.speed));speed=c.speed;angle=c.angle;break;
    case 1:
        if(!(c.angular_acceleration < -990.0f)){
            auto v=plus(polar(previous_angle,-previous_speed),polar(c.angular_acceleration,-c.acceleration));p=plus(previous,v);magnitude_angle(v,speed,angle);
        }else{p=minus(previous,scale(c.direction,sub(previous_speed,c.acceleration)));speed=sub(c.speed,c.acceleration);angle=previous_angle;}break;
    case 2:
        p=minus(previous,scale(polar(previous_angle,previous_speed),sub(time,floor_value(time))));speed=sub(previous_speed,c.acceleration);angle=m::wrap_angle(sub(previous_angle,c.angular_acceleration));
        p=minus(p,scale(polar(angle,speed),n::add32(sub(1,time),floor_value(time))));break;
    }
}
void sample_curve_path(const CurveNode* c,sprite::Vec3& p,float& speed,float& angle,const sprite::Vec3& previous,float previous_speed,float previous_angle,float time,bool backwards){
    while(c){if(time>=c->begin && time<c->end){if(backwards)sample_curve_backwards(*c,p,speed,angle,previous,previous_speed,previous_angle,time);else sample_curve_absolute(*c,p,speed,angle,time);return;}c=c->next;}
}
}
