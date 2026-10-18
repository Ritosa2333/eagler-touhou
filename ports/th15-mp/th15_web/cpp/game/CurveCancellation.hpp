#pragma once
#include "CurveMotion.hpp"
#include "BulletState.hpp"
namespace th15 {
struct CurveCancellationHost:CancellationRewardHost {
    virtual bool curve_effect(i32 script,const Vec3&,bool tracked)=0;
    virtual bool split_curve(u32 count,float initial_time,const CurvePath&)=0;
};
class CurveCancellation {
    Rng& random;BulletCancellationRewards& rewards;CurveCancellationHost& host;
    bool clip_rectangle(CurveLaserMotion&,u32& flags,std::vector<u8>&,float rate);
public:
    std::string error;
    CurveCancellation(Rng& r,BulletCancellationRewards& b,CurveCancellationHost& h):random(r),rewards(b),host(h){}
    bool all(const CurveLaserMotion&,i32 color,i32 protection,bool honor,i32& state);
    i32 circle(CurveLaserMotion&,u32& flags,i32 color,i32 protection,bool honor,const Vec3&,float radius,i32 reward);
    bool rectangle(CurveLaserMotion&,u32& flags,i32 color,i32 protection,bool honor,const Vec3&,const Vec2&,float angle,i32 reward,float rate);
};
}
