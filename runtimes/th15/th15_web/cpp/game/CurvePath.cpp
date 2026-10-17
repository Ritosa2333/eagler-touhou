#include "CurvePath.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec3 polar(float angle,float speed)noexcept{return {float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};}
void sum(Vec3& p,const Vec3& d,float scale)noexcept{p={float(p.x+float(d.x*scale)),float(p.y+float(d.y*scale)),float(p.z+float(d.z*scale))};}
void subtract(Vec3& p,const Vec3& d,float scale)noexcept{p={float(p.x-float(d.x*scale)),float(p.y-float(d.y*scale)),float(p.z-float(d.z*scale))};}
void vector_state(const Vec3& v,CurvePathSample& out)noexcept{out.speed=float(std::sqrt(double(float(float(v.x*v.x)+float(v.y*v.y)))));out.angle=float(std::atan2(double(v.y),double(v.x)));}
}
bool CurvePath::evaluate(const CurvePathSegment& s,float time,CurvePathSample& out)noexcept{
    const float t=float(time-s.begin);
    switch(s.mode){
    case CurvePathMode::linear:{const auto axis=[&](float d,float p){return float(float(float(d*t)*s.speed)+p);};out.position={axis(s.direction.x,s.origin.x),axis(s.direction.y,s.origin.y),axis(s.direction.z,s.origin.z)};out.speed=s.speed;out.angle=s.angle;return true;}
    case CurvePathMode::acceleration:
        if(s.angular_acceleration<-990.f){const float a=float(float(s.acceleration*t)+float(s.speed*2.f)),b=float(t+1.f);const auto axis=[&](float d,float p){return float(p+float(float(float(d*a)*b)*.5f));};out.position={axis(s.direction.x,s.origin.x),axis(s.direction.y,s.origin.y),axis(s.direction.z,s.origin.z)};out.speed=float(float(s.acceleration*t)+s.speed);out.angle=s.angle;}
        else{const auto velocity=polar(s.angle,s.speed),acceleration=polar(s.angular_acceleration,s.acceleration);const Vec3 v{float(acceleration.x+velocity.x),float(acceleration.y+velocity.y),float(acceleration.z+velocity.z)};out.position=s.origin;sum(out.position,v,t);vector_state(v,out);}return true;
    case CurvePathMode::angular:{out.position=s.origin;out.angle=s.angle;out.speed=s.speed;const i32 whole=i32(t);for(i32 i=0;i<whole;i++){const auto v=polar(out.angle,out.speed);out.angle=normalize_angle(float(out.angle+s.angular_acceleration));out.speed=float(out.speed+s.acceleration);sum(out.position,v,1);}const auto v=polar(out.angle,out.speed);sum(out.position,v,float(t-float(std::floor(double(t)))));return true;}
    }return false;
}
bool CurvePath::preceding(const CurvePathSegment& s,float time,const CurvePathSample& newer,CurvePathSample& out)noexcept{
    switch(s.mode){
    case CurvePathMode::linear:out=newer;subtract(out.position,s.direction,s.speed);out.speed=s.speed;out.angle=s.angle;return true;
    case CurvePathMode::acceleration:
        if(s.angular_acceleration<-990.f){out.position=newer.position;subtract(out.position,s.direction,float(newer.speed-s.acceleration));out.speed=float(s.speed-s.acceleration);out.angle=newer.angle;}
        else{const auto velocity=polar(newer.angle,-newer.speed),acceleration=polar(s.angular_acceleration,-s.acceleration);const Vec3 v{float(acceleration.x+velocity.x),float(acceleration.y+velocity.y),float(acceleration.z+velocity.z)};out.position=newer.position;sum(out.position,v,1);vector_state(v,out);}return true;
    case CurvePathMode::angular:{const auto v=polar(newer.angle,newer.speed);const float whole=float(std::floor(double(time))),part=float(time-whole);out.position=newer.position;subtract(out.position,v,part);out.speed=float(newer.speed-s.acceleration);out.angle=normalize_angle(float(newer.angle-s.angular_acceleration));const auto prior=polar(out.angle,out.speed);subtract(out.position,prior,float(float(1.f-time)+whole));return true;}
    }return false;
}
bool CurvePath::sample(float time,const CurvePathSample* newer,CurvePathSample& out)const noexcept{for(const auto& s:segments)if(time>=s.begin&&time<s.end)return newer?preceding(s,time,*newer,out):evaluate(s,time,out);return true;}
}
