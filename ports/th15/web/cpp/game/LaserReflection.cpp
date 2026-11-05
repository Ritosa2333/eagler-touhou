#include "LaserReflection.hpp"
#include "SegmentIntersection.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
i32 LaserReflection::advance(const MovingLaserMotion& motion,u32& flags,i32 sound,LaserReflectionHost& host){
    error.clear();const Vec2 start{motion.position.x,motion.position.y};
    const Vec2 end{float(start.x+float(std::cos(double(motion.angle))*double(motion.length))),float(start.y+float(std::sin(double(motion.angle))*double(motion.length)))};
    if(end.x>-192&&end.x<192&&end.y>0&&end.y<448)return 0;
    bool completed=false;
    const auto reflect=[&](u32 side,bool outside,const Vec2& a,const Vec2& b,bool vertical){
        if(!(sides&side)||!outside)return true;completed=true;if(sides&16)return true;
        Vec3 origin=host.reflection_origin();Vec2 crossing{origin.x,origin.y};segment_intersection(a,b,end,start,crossing);origin={crossing.x,crossing.y,0};
        const float angle=vertical?normalize_angle(float(-motion.angle-3.1415927410125732421875f)):-motion.angle;
        if(!host.reflected_laser(origin,angle,speed)){error="Failed to create reflected moving laser";return false;}return true;
    };
    // Both original vertical segments begin at Y=-192, including above the field.
    if(!reflect(1,end.y<0,{-256,0},{256,0},false)||!reflect(2,end.y>448,{-256,448},{256,448},false)||!reflect(4,end.x<-192,{-192,-192},{-192,640},true)||!reflect(8,end.x>192,{192,-192},{192,640},true))return -1;
    if(!completed)return 0;flags&=~0x40u;if(sound>=0)host.reflection_sound(sound);return 1;
}
}
