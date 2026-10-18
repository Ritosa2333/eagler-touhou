#pragma once
#include "CurveMotion.hpp"
#include "LaserProgram.hpp"
namespace th15 {
struct CurveProgramHost:LaserProgramHost {
    virtual bool rebind_curve_body(i32 type,i32 color)=0;
    virtual void curve_blend(bool additive)=0;
};
struct CurveProgramContext {CurveLaserMotion& motion;u32& id;i32& state;i32& protection;CurveProgramHost& host;i32 queued_sound=-1;};
class CurveProgram {
    bool append_transition(CurveLaserMotion&,const BulletTransform&);
public:
    u32 flags=0;i32 index=0;LaserAcceleration acceleration;LaserTurn turn;LaserReflection reflection;
    Timer boost_timer{0,0,0,0,0},angular_timer{0,0,0,0,0},wait{0,0,0,0,0},freeze{0,0,0,0,0},protection_timer{0,0,0,0,0};
    i32 boost_stage=0,angular_duration=0,protection_parameter=0;float angular_speed=0,angular_angle=0;std::string error;
    bool activate(std::array<BulletTransform,18>&,CurveProgramContext&);
    bool update(std::array<BulletTransform,18>&,CurveProgramContext&,float rate);
};
}
