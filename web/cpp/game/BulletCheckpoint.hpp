#pragma once
#include "BulletScene.hpp"
#include "AnmManager.hpp"
#include "AnmCheckpoint.hpp"
namespace th15 {
class BulletCheckpoint {
    struct Slot {BulletState state;AnmVm body,overlay;};
    BulletScene& scene;AnmManager& animations;AnmCheckpoint& pool;i32 bank;
    std::vector<Slot> slots;std::array<u32,BulletManager::capacity> effects{};Vec2 reflection_bounds{};i32 reward_count=0;bool available=false;
public:
    std::string error;
    BulletCheckpoint(BulletScene& s,AnmManager& a,AnmCheckpoint& p,i32 resource):scene(s),animations(a),pool(p),bank(resource),slots(BulletManager::capacity){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&);
    bool read_file(const u8*,u32,u32& consumed);
    const BulletState* state(u32 i)const noexcept{return i<slots.size()?&slots[i].state:nullptr;}
};
}
