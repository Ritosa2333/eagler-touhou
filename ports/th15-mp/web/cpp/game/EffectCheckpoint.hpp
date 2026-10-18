#pragma once
#include "EffectManager.hpp"
#include "AnmCheckpoint.hpp"
namespace th15 {
class EffectCheckpoint {
    EffectManager& effects;AnmRegistry& registry;AnmCheckpoint& animations;
    std::array<u32,1024> handles{};i32 cursor=0;bool available=false;
public:
    std::string error;
    EffectCheckpoint(EffectManager& e,AnmRegistry& r,AnmCheckpoint& a):effects(e),registry(r),animations(a){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&,AnmManager&);
    bool read_file(const u8*,u32,u32& consumed,AnmManager&);
    const std::array<u32,1024>& saved_handles()const noexcept{return handles;}i32 saved_cursor()const noexcept{return cursor;}
};
}
