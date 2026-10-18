#pragma once
#include "../../cpp/game/LaserManager.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include <memory>
namespace laser_test {
using namespace th09;
struct Collision {Vec2 center,size;Vec3 pivot;float angle;};
struct Fixture:LaserCollisionActions {
    AnmResource resource;Rng random;AnmExecutor executor{random};std::unique_ptr<LaserManager> manager;
    Vec3 player;std::vector<Collision> collisions;
    bool load(const u8* data,u32 size){if(!resource.load(8,data,size))return false;manager=std::make_unique<LaserManager>(resource.view(),executor);return true;}
    void add_laser_collision(const Vec2& center,const Vec2& size,const Vec3& pivot,float angle)override{collisions.push_back({center,size,pivot,angle});}
};
}
