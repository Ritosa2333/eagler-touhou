#pragma once
#include "LaserMotion.hpp"
#include "BulletState.hpp"
namespace th15 {
struct LaserAcceleration {
    Timer timer{0,0,0,0,0};float speed=0,angle=0;Vec3 vector{};i32 duration=0;
    bool advance(MovingLaserMotion&,u32& flags,float rate)noexcept;
};
struct LaserTurn {
    Timer timer{0,0,0,0,0};float speed=0,angle=0;i32 duration=0,limit=0,count=0,mode=0;
    bool advance(MovingLaserMotion&,u32& flags,float rate,i32 sound=-1,BulletSoundHost* host=nullptr);
};
}
