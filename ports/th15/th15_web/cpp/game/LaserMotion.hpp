#pragma once
#include "Timer.hpp"
namespace th15 {
struct MovingLaserMotion {
    Vec3 position{},velocity{};float angle=0,length=0,width=0,speed=0,traveled=0;
    float maximum_length=0,end_distance=0;Timer outside_delay{0,0,0,0,0},invulnerability{0,0,0,0,0},animation_age{0,0,0,0,0};
    void set_velocity()noexcept;
    bool advance(float rate)noexcept;
    void finish_frame(float rate)noexcept{animation_age.tick(&rate);}
};
struct StationaryLaserMotion {
    Vec3 position{},drift{};float angle=0,length=0,width=2,speed=0,angular_velocity=0,maximum_length=0,target_width=0;
    i32 state=3,delay=0,warmup=0,active=0,fade=0;Timer age;float traveled=0;
    StationaryLaserMotion(){age.set(0);}
    bool advance(float rate,const Vec3* anchor=nullptr)noexcept;
    void finish_frame(float rate)noexcept{age.tick(&rate);}
};
}
