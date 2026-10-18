#pragma once
#include "EnemyRuntime.hpp"
#include "EnemyDrops.hpp"
namespace th15 {
struct EnemyDeathEffect {i32 resource=0,script=0;Vec3 position{};float rotation=-1.57079637050628662109375f;i32 layer=3;u32 render_flags_set=0x40400,render_flags_clear=0x80000;};
struct EnemyDeathHost:ItemSpawnHost {
    virtual bool sound(i32,const Vec3&)=0;
    virtual bool effect(const EnemyDeathEffect&)=0;
    virtual bool callback(EnemyRuntime&)=0;
};
class EnemyDeath {
    EnemyWorldState& world;Rng& random;EnemyDeathHost& host;
    int fail(EnemyRuntime&,const char* message);
public:
    EnemyDeath(EnemyWorldState& world,Rng& random,EnemyDeathHost& host):world(world),random(random),host(host){}
    // 1 tells the manager to remove the entity. -2 exposes an unavailable
    // resource/callback instead of silently omitting an original effect.
    int execute(EnemyRuntime&,float rate);
};
}
