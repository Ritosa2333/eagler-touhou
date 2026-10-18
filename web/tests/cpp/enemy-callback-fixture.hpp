#pragma once
#include "bullet-manager-fixture.hpp"
#include "../../cpp/game/EnemyCallbacks.hpp"
#include "../../cpp/game/EnemyManager.hpp"
namespace th11::test {
struct EnemyCallbackFixture:BulletManagerFixture,EnemyCallbackWorld {
    EnemyCallbacks callbacks{enemy.world,enemy.commands,*this};
    Enemy linked[8]{};AnmVm arc{};
    struct Event {i32 kind,value;Vec3 position;float a,b,c;};
    std::vector<Event> calls;std::vector<EnemySpawn> spawns;
    i32 damage_value=0;
    EnemyCallbackFixture(){for(u32 i=0;i<8;++i){linked[i].state.initialize(&linked[i],&enemy.world.rate);linked[i].state.manager_link.next=i==7?nullptr:&linked[i+1].state.manager_link;}arc.initialize();enemy.commands.callbacks=&callbacks;}
    EnemyLink* callback_enemies()override{return &linked[0].state.manager_link;}
    bool callback_spawn(const EnemyState& e,Vec3 p)override{EnemySpawn spawn;spawn.position=p;spawn.health=spawn.score=10;spawn.drop=-2;std::memcpy(spawn.integers,e.integers,48);spawns.push_back(spawn);return true;}
    bool callback_move_player(Vec3 p)override{calls.push_back({0,0,p,0,0,0});return true;}
    bool callback_indicator(i16 interrupt,bool update)override{calls.push_back({1,interrupt,{},float(update),0,0});return true;}
    AnmVm* callback_animation(u32&)override{return &arc;}
    bool callback_damage(Vec3 p,Vec2 size,i32& out)override{calls.push_back({2,damage_value,p,size.x,size.y,0});out=damage_value;return true;}
    bool callback_collision(Vec3 p,float angle,float width,float length)override{calls.push_back({3,0,p,angle,width,length});return true;}
};
}
