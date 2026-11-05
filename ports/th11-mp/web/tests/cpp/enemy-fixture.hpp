#pragma once
#include "../../cpp/game/EnemyState.hpp"
#include "../../cpp/game/EnemyCommands.hpp"
#include "../../cpp/game/EnemyScript.hpp"
namespace th11::test {
struct EnemyFixture {
    Enemy enemy,boss;EnemyEnvironment world;EnemyGlobals globals;
    i32 sprite=0;
    EnemyCommandEnvironment commands;
    u32 visibility_count=0,visibility_events[128]{};
    EnemyScriptServices services;
    EnemyFixture():globals(enemy.state,world),services(enemy.state,world,commands){enemy.state.initialize(&enemy,&world.rate);boss.state.initialize(&boss,&world.rate);world.boss=&boss.state;world.animation_user=this;world.animation_sprite=[](u32,void* p){return static_cast<EnemyFixture*>(p)->sprite;};commands.animation_user=this;commands.animation_visibility=[](u32 id,bool visible,void* p){auto& f=*static_cast<EnemyFixture*>(p);if(f.visibility_count>=64)__builtin_trap();f.visibility_events[2*f.visibility_count]=id;f.visibility_events[2*f.visibility_count+1]=visible;++f.visibility_count;};}
};
}
