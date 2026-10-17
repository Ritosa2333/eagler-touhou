#pragma once
#include "EnemyRuntime.hpp"
namespace th15 {
struct EnemyShotDamage {i32 amount=0;bool direct=false;Vec3 position{};};
struct EnemyDamageHost {
    virtual ~EnemyDamageHost()=default;
    virtual bool shot_damage(EnemyState&,bool special,EnemyShotDamage&)=0;
    virtual bool additional_damage(EnemyState&,i32 incoming,i32&)=0;
    virtual int die(EnemyRuntime&,float rate)=0;
    virtual bool contact(EnemyState&,AnmVm*,i32& result)=0;
    virtual bool graze(const Vec3&)=0;
    virtual bool sound(i32,const Vec3&)=0;
};
// Hit detection is supplied by the player/shot manager. Enemy life, phase
// changes, sound selection and damage flashing follow the original order.
class EnemyDamage {
    EnemyWorldState& world;EnemyDamageHost& host;EnemyVisualHost& animations;
    int fail(EnemyRuntime&,const char* message);
    int interrupt(EnemyRuntime&,const std::string&,float rate,bool immediate);
public:
    EnemyDamage(EnemyWorldState& world,EnemyDamageHost& host,EnemyVisualHost& animations):world(world),host(host),animations(animations){}
    int update(EnemyRuntime&,float rate);
};
}
