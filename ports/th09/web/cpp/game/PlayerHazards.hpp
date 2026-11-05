#pragma once
#include "PlayerCollision.hpp"
#include "Bullet.hpp"
#include <array>
namespace th09 {
struct PlayerHazard {Vec3 position,pivot,half_extent;float radius=0,angle=0;Bullet* bullet=nullptr;};
struct HazardPlayer {
    i32 state=0;Timer invulnerability{0,0,0};Vec3 position,half_extent;float hit_radius=0;Bounds hit_bounds;
};
class PlayerHazards {
    PlayerHazard* collision(HazardPlayer&,float additional_radius);
public:
    static constexpr u32 capacity=128;
    std::array<PlayerHazard,capacity> entries;u32 count=0;
    void clear(){count=0;}
    void circle(const Vec3&,float radius,Bullet* source=nullptr);
    void box(const Vec3&,const Vec3& extent,Bullet* source=nullptr);
    void rotated_box(const Vec3&,const Vec3& extent,const Vec3& pivot,float angle,Bullet* source=nullptr);
    PlayerHazard* first_hit(HazardPlayer&);
    PlayerHazard* sample(HazardPlayer&,const Vec3& position,const Vec3& half_extent,float additional_radius);
};
}
