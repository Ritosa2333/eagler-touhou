#pragma once
#include "EnemyCommands.hpp"
#include "EnemyVariables.hpp"
#include "EclThreads.hpp"
namespace th15 {
class EnemyRuntime;
struct EnemyRuntimeHost {
    virtual ~EnemyRuntimeHost()=default;
    virtual EnemyVisualHost* animations()=0;
    virtual LaserScene* lasers(){return nullptr;}
    virtual int after_script(EnemyRuntime&,float rate)=0;
    virtual int collide_and_damage(EnemyRuntime&,float rate)=0;
    virtual bool update_distortion(EnemyState&,float rate)=0;
};
class EnemyRuntime {
    EnemyWorldState& world;EnemyRuntimeHost& host;
    int update(float rate,const Vec3& background_delta);
public:
    EnemyState state;EnemyVariables variables;EnemyCommands commands;EclThreads scripts;
    std::string error;
    EnemyRuntime(EclProgram& program,EnemyWorldState& world,Rng& random,Rng& visual_random,EnemyRuntimeHost& host);
    bool initialize(const EnemySpawnRequest&,u32 identifier);
    bool switch_routine(const std::string& name);
    // The manager clears this after a successful visit, not before traversal:
    // a just-spawned enemy has already executed its initial update.
    void clear_update_guard()noexcept{state.flags&=~0x40000u;}
    // 0 keeps the entity, -1 removes it, -2 is an explicit integration error.
    int step(float rate,const Vec3& background_delta={});
};
}
