#pragma once
#include "AnmManager.hpp"
namespace th15 {
class EffectManager {
    AnmManager& animations;i32 resource;
public:
    std::array<u32,1024> handles{};i32 cursor=0;std::string error;
    EffectManager(AnmManager& a,i32 id):animations(a),resource(id){}
    i32 reserve();
    u32 create(i32 script,const Vec3&,float rotation=0);
    u32 tracked(i32 script,const Vec3&,float rotation=0);
    u32 trail(const Vec3&,Rng& visual_random);
    void track(u32 handle){const i32 slot=reserve();if(slot>=0)handles[u32(slot)]=handle;}
};
}
