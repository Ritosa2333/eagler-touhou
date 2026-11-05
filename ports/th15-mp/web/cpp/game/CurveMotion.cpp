#include "CurveMotion.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
bool CurveLaserMotion::initialize(const Vec3& origin,float bearing,float thickness,float travel_speed,u32 count,float initial_time,const CurvePath* inherited){
    if(count>0x100000)return false;position=source=origin;angle=source_angle=bearing;width=render_width=thickness;speed=source_speed=travel_speed;velocity={float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};
    points.assign(count,{origin,bearing,travel_speed});path_age.set(0);path_age.fractional=initial_time;path_age.current=i32(initial_time);path_age.previous=wrapping_add(path_age.current,-1);
    if(inherited)path=*inherited;else{CurvePathSegment segment;segment.direction={float(std::cos(double(angle))),float(std::sin(double(angle))),0};segment.origin=origin;segment.angle=normalize_angle(angle);segment.speed=speed;path.segments.assign(1,segment);}
    for(u32 i=0;i<count;i++)path.sample(float(path_age.fractional-float(i)),i?&points[i-1]:nullptr,points[i]);outside_delay.set(30);return true;
}
bool CurveLaserMotion::advance(float rate,u32 program_flags)noexcept{
    bool preceding=false;for(u32 i=0;i<points.size();i++){const float time=float(path_age.fractional-float(i));if(time<0){points[i]={source,source_angle,source_speed};continue;}
        const CurvePathSegment* match=nullptr;for(const auto& segment:path.segments)if(time>=segment.begin&&time<segment.end){match=&segment;break;}
        if(match){if(!preceding)CurvePath::evaluate(*match,time,points[i]);else CurvePath::preceding(*match,time,points[i-1],points[i]);}preceding=true;
    }
    if(outside_delay.current<=0&&!(program_flags&256)){for(const auto& p:points)if(-192.f<float(p.position.x+width)&&float(p.position.x-width)<192.f&&0.f<float(p.position.y+width)&&float(p.position.y-width)<448.f)return false;return true;}
    outside_delay.decrement(&rate);return false;
}
}
