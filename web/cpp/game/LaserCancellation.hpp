#pragma once
#include "BulletState.hpp"
#include "LaserMotion.hpp"
namespace th15 {
struct LaserCancellationHost:CancellationRewardHost {
    virtual bool effect(i32 script,const Vec3& position,bool tracked)=0;
    virtual bool segment(const Vec3& position,float length)=0;
    virtual bool stationary_segment(const Vec3& position,float length,float distance,bool rectangle){return segment(position,length);}
};
class LaserCancellation {
    Rng& random;BulletCancellationRewards& rewards;LaserCancellationHost& host;
    bool clip(MovingLaserMotion&,u32& flags,const Vec3& origin,const Vec3& step,const std::vector<u8>& mask,u32 count);
    bool clip_stationary(MovingLaserMotion&,const Vec3& origin,const Vec3& step,const std::vector<u8>& mask,u32 count,bool rectangle);
public:
    std::string error;
    LaserCancellation(Rng& rng,BulletCancellationRewards& r,LaserCancellationHost& h):random(rng),rewards(r),host(h){}
    i32 all(const Vec3& origin,float angle,float length,i32 type,i32 color,i32 protection,bool honor_protection,i32 reward,i32& state,bool stationary=false);
    i32 circle(MovingLaserMotion&,u32& flags,i32 type,i32 color,i32 protection,bool honor_protection,const Vec3& center,float radius,i32 reward,bool stationary=false);
    i32 rectangle(MovingLaserMotion&,u32& flags,i32 type,i32 color,i32 protection,bool honor_protection,const Vec3& center,const Vec2& size,float angle,i32 reward,bool stationary=false);
    static i32 effect_script(i32 type,i32 color)noexcept;
    static i32 query(const Vec3& origin,float angle,float length,float width,const Vec3& center,float radius)noexcept;
};
}
