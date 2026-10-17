#pragma once
#include "../../cpp/game/AttackAreas.hpp"
#include <vector>
namespace attack_areas_test {
using namespace th09;
static_assert(sizeof(AttackArea)==68);
struct Fixture:AreaCollisionActions {
    AttackAreas areas;Rng rng;AreaCollisionContext context{rng,*this};Bullet bullet;Bounds graze;TransferParameters transfer;std::vector<std::array<u32,10>> events;
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    TransferParameters* create_transfer(i32 effect,const Vec3& p,const Vec3& target)override{events.push_back({0,u32(effect),bits(p.x),bits(p.y),bits(p.z),bits(target.x),bits(target.y),bits(target.z)});return &transfer;}
    void charge(float n)override{events.push_back({1,bits(n)});}
    void play_sound(i32 n,i32 pan)override{events.push_back({2,u32(n),u32(pan)});}
    void combo(const Vec3& p,i32 a,i32 b,i32 d,i32 score)override{events.push_back({3,bits(p.x),bits(p.y),bits(p.z),u32(a),u32(b),u32(d),u32(score)});}
    void add_score(i32 n)override{events.push_back({4,u32(n)});}
};
}
