#include "LaserDynamics.hpp"
#include <cmath>
namespace th15 {
bool LaserAcceleration::advance(MovingLaserMotion& motion,u32& flags,float rate)noexcept{
    if(timer.current>=duration){flags&=~4u;return true;}motion.speed=float(float(speed*rate)+motion.speed);motion.velocity={float(motion.velocity.x+float(vector.x*rate)),float(motion.velocity.y+float(vector.y*rate)),float(motion.velocity.z+float(vector.z*rate))};
    if(std::abs(motion.velocity.x)>.0001f||std::abs(motion.velocity.y)>.0001f)motion.angle=float(std::atan2(double(motion.velocity.y),double(motion.velocity.x)));timer.tick(&rate);return false;
}
bool LaserTurn::advance(MovingLaserMotion& motion,u32& flags,float rate,i32 sound,BulletSoundHost* host){
    float velocity_speed;if(timer.current<duration)velocity_speed=float(motion.speed-float(float(timer.fractional*motion.speed)/float(duration)));
    else{if(sound>=0&&host)host->play(sound);motion.angle=float(angle+motion.angle);motion.speed=speed;count=wrapping_add(count,1);timer.set(0);velocity_speed=speed;if(count>=limit){motion.velocity={float(std::cos(double(motion.angle))*double(velocity_speed)),float(std::sin(double(motion.angle))*double(velocity_speed)),motion.velocity.z};flags&=~16u;return true;}}
    motion.velocity={float(std::cos(double(motion.angle))*double(velocity_speed)),float(std::sin(double(motion.angle))*double(velocity_speed)),motion.velocity.z};timer.tick(&rate);return false;
}
}
