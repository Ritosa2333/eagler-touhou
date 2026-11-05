#pragma once
#include "PlayerCollision.hpp"
#include "Timer.hpp"
#include "CurveMotion.hpp"
namespace th15 {
struct LaserContactHost {
    virtual ~LaserContactHost()=default;
    virtual bool cancel_hit(const Vec3& player)=0;
    virtual void graze_spark(const Vec3&)=0;
    virtual void graze_flash()=0;
    virtual void graze_resonance(float)=0;
};
struct LaserContact {
    Timer graze_age{0,0,0,0,0};
    bool moving(const Vec3& position,float angle,float length,float width,u32 flags,bool graze_only,float rate,const PlayerCollision&,PlayerDamageHost&,LaserContactHost&);
    bool stationary(const Vec3& position,float angle,float length,float width,i32 state,bool graze_only,float rate,const PlayerCollision&,PlayerDamageHost&,LaserContactHost&);
    bool curved(CurveLaserMotion&,bool graze_only,float rate,const PlayerCollision&,PlayerDamageHost&,LaserContactHost&);
private:
    bool apply(const Vec3&,float angle,float length,float width,bool graze_only,float rate,const PlayerCollision&,PlayerDamageHost&,LaserContactHost&);
};
}
