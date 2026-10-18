#pragma once
#include "../../cpp/game/BulletManager.hpp"
#include "../../cpp/game/EnemyManager.hpp"
#include "../../cpp/game/ItemManager.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/LaserManager.hpp"
namespace th11::test {
struct CancellationFixture:BulletWorld,EnemyFrameWorld,ItemWorld {
    AnmResource resource;AnmManager animations;EclProgram program;EnemyEnvironment environment;EnemyCommandEnvironment commands;
    BulletManager bullets{resource,animations,*this,0};ItemManager items{resource,animations,*this,0};
    EnemyManager enemies{program,environment,commands,*this};std::array<Enemy,96> targets{};
    struct LaserCallbacks:LaserWorld {
        CancellationFixture& fixture;
        explicit LaserCallbacks(CancellationFixture& f):fixture(f){}
        bool cancel_effect(Vec3 p,i32 script)override{return fixture.visual(p,script);}
        bool cancel_reward(Vec3 p)override{return fixture.point(p);}
        bool sound(i32,float,bool)override{return true;}
        i32 collision(Vec3,float,float,float)override{return 0;}
    } laser_world{*this};
    LaserManager lasers{resource,animations,laser_world,0};
    CancellationFixture(){for(u32 i=0;i<targets.size();++i){auto& n=targets[i].state.manager_link;n.value=&targets[i];n.next=i+1<targets.size()?&targets[i+1].state.manager_link:nullptr;n.previous=i?&targets[i-1].state.manager_link:nullptr;}enemies.first=&targets[0].state.manager_link;}
    bool load(const u8* p,u32 n){return resource.open(p,n);}
    bool visual(Vec3 p,i32 script){p.x=float((double(p.x)+32)+192);p.y=float(double(p.y)+16);return animations.create(resource,script,0,22,false,false,&p)!=nullptr;}
    bool point(Vec3 p){return items.spawn(8,p,0xffffffff,-1.5707963705062866f,.6f)==0;}
    bool effect(const Vec3& p,i32 script)override{return visual(p,script);}
    bool colored_effect(const Vec3& position,i32 script,u32 color)override{Vec3 p=position;p.x=float((double(p.x)+32)+192);p.y=float(double(p.y)+16);auto* vm=animations.create(resource,script,0,22,false,false,&p);if(!vm)return false;vm->color=color;return true;}
    bool effect(Vec3 p,i32 script)override{return visual(p,script);}
    bool cancel_reward(const Vec3& p)override{return point(p);}
    bool cancel_reward(Vec3 p)override{return point(p);}
    bool cancel_enemies(const Vec3& p,float radius,bool reward)override{return enemies.cancel_circle(p,radius,reward);}
    void clear_shot_targets(Enemy*)override{}
    bool sound(i32,float)override{return true;}
    bool rank_delta(i32)override{return true;}
    bool collect(ItemState&,bool&)override{return true;}
};
}
