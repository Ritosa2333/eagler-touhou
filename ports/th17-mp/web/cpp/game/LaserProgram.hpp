#pragma once
#include "LaserDynamics.hpp"
#include "LaserVisual.hpp"
#include "LaserReflection.hpp"
namespace th17 {
BulletShooter laser_bullet_request(const std::array<BulletTransform,18>&,u32 index,const Vec3& origin,float angle,float distance,float speed,const Vec3& player);
struct LaserProgramHost:LaserReflectionHost {
    virtual ~LaserProgramHost()=default;virtual void laser_sound(i32,bool queued)=0;
    void reflection_sound(i32 id)override{laser_sound(id,true);}
    virtual bool emit_laser_bullets(const BulletShooter&){return false;}
    virtual bool erase_emitting_laser(){return false;}
};
struct LaserProgramContext {MovingLaserMotion& motion;LaserVisual& visual;u32& id;i32& state;i32& protection;Vec3 player;LaserProgramHost& host;i32 queued_sound=-1;};
class LaserProgram {
public:
    u32 flags=0;i32 index=0;LaserAcceleration acceleration;LaserTurn turn;LaserReflection reflection;
    Timer boost_timer{0,0,0,0,0};i32 boost_stage=0;Timer angular_timer{0,0,0,0,0};float angular_speed=0,angular_angle=0;i32 angular_duration=0;
    Timer wait{0,0,0,0,0},freeze{0,0,0,0,0};std::string error;
    bool activate(std::array<BulletTransform,18>&,LaserProgramContext&);
    bool update(std::array<BulletTransform,18>&,LaserProgramContext&,float rate);
};
}
