#pragma once
#include "../../cpp/game/BulletManager.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "enemy-fixture.hpp"
#include "render-fixture.hpp"
#include <vector>
namespace th11::test {
struct BulletManagerEvent {i32 kind,id;float x,y;};
struct BulletCancelEvent {i32 kind,id;Vec3 position;u32 color;float angle,speed;};
struct BulletManagerFixture:BulletWorld {
    AnmResource resource;AnmManager animations;std::unique_ptr<BulletManager> manager;
    std::vector<BulletManagerEvent> events;i32 collision_result=0;
    std::vector<BulletCancelEvent> cancellation_events;
    EnemyFixture enemy;
    RecordingGraphics graphics;AnmRenderer renderer{graphics};
    bool load(const u8* p,u32 n){if(!resource.open(p,n))return false;manager=std::make_unique<BulletManager>(resource,animations,*this,0);enemy.commands.bullets=manager.get();enemy.world.shared_random=&animations.script_rng;return true;}
    bool sound(i32 id,float x,bool positional)override{events.push_back({positional?1:0,id,x,0});return true;}
    i32 collision(const BulletState&)override{return collision_result;}
    bool effect(const Vec3& p,i32 id)override{events.push_back({2,id,p.x,p.y});cancellation_events.push_back({2,id,p,0,0,0});return true;}
    bool register_graze()override{events.push_back({3,0,0,0});return true;}
    bool reward_graze()override{events.push_back({4,0,0,0});return true;}
    bool cancel_reward(const Vec3& p)override{cancellation_events.push_back({5,8,p,0xffffffff,-1.5707963705062866f,.6f});return true;}
    bool convert_enemies(const Vec3& p,float radius,bool reward)override{cancellation_events.push_back({8,i32(reward),p,0,radius,0});return true;}
    bool cancel_shot(Vec3 p,float angle)override{cancellation_events.push_back({7,0,p,0,angle,0});return true;}
    bool cancel_enemies(const Vec3& p,float radius,bool reward)override{cancellation_events.push_back({6,i32(reward),p,0,radius,0});return true;}
    bool cancel_enemy_beam(const Vec3& p,float width,bool reward)override{cancellation_events.push_back({9,i32(reward),p,0,width,0});return true;}
};
}
