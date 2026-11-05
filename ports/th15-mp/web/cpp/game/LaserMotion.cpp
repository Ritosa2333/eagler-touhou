#include "LaserMotion.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
bool StationaryLaserMotion::advance(float rate,const Vec3* anchor)noexcept{
    if(length<maximum_length){length=float(float(speed*rate)+length);if(length>maximum_length)length=maximum_length;}
    angle=normalize_angle(float(float(angular_velocity*rate)+angle));if(anchor)position=*anchor;
    position={float(position.x+float(drift.x*rate)),float(float(drift.y*rate)+position.y),float(float(drift.z*rate)+position.z)};
    switch(state){case 3:if(age.current>=delay){age.set(0);state=4;}break;
        case 4:if(age.current<warmup){width=float(float(target_width*age.fractional)/float(warmup));break;}age.set(0);state=2;width=target_width;[[fallthrough]];
        case 2:if(age.current<active)break;age.set(0);state=5;[[fallthrough]];
        case 5:if(age.current>=fade)return true;width=float(target_width-float(float(age.fractional*target_width)/float(fade)));break;
        default:break;}return false;
}
void MovingLaserMotion::set_velocity()noexcept{velocity={float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};}
bool MovingLaserMotion::advance(float rate)noexcept{
    const float distance=float(speed*rate);
    if(maximum_length>length){length=float(distance+length);if(length>maximum_length)length=maximum_length;}
    else{traveled=float(distance+traveled);position={float(position.x+float(velocity.x*rate)),float(float(velocity.y*rate)+position.y),float(float(velocity.z*rate)+position.z)};
        if(end_distance>0&&float(length+traveled)>end_distance){length=float(end_distance-traveled);maximum_length=length;if(length<=0)return true;}}
    if(outside_delay.current<=0&&invulnerability.current<=0){const float x=float(position.x+float(std::cos(double(angle))*double(length))),y=float(position.y+float(std::sin(double(angle))*double(length)));
        const auto outside=[&](float px,float py){return float(px+width)<=-192||float(px-width)>=192||float(py+width)<=0||float(py-width)>=448;};if(outside(position.x,position.y)&&outside(x,y))return true;
    }else{if(outside_delay.current>0)outside_delay.decrement(&rate);if(invulnerability.current>0)invulnerability.decrement(&rate);}return false;
}
}
