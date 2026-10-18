#pragma once
#include "EclVm.hpp"
#include "EnemyAppearance.hpp"
namespace th08 {
struct EnemySpawnResult {EclVm* enemy=nullptr;bool failed=true;};
struct EnemySpawnActions:EnemyAppearanceActions {
    virtual ~EnemySpawnActions()=default;
    virtual EnemySpawnResult spawn(const TimelineSpawn&,const EclContext::Locals&)=0;
    virtual AnmVm* overlay(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual void panned_sound(i32 index,float x)=0;
};
bool execute_enemy_spawn(EclVm&,const EclInstruction&,EclGlobals&,EnemySpawnActions&);
}
