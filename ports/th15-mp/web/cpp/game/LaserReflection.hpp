#pragma once
#include "LaserMotion.hpp"
#include <string>
namespace th15 {
struct LaserReflectionHost {
    virtual ~LaserReflectionHost()=default;
    virtual Vec3 reflection_origin()const {return {};}
    virtual bool reflected_laser(const Vec3&,float angle,float speed){return false;}
    virtual void reflection_sound(i32)=0;
};
struct LaserReflection {
    float speed=0;i32 count=0,limit=0;u32 sides=0;std::string error;
    i32 advance(const MovingLaserMotion&,u32& flags,i32 sound,LaserReflectionHost&);
};
}
