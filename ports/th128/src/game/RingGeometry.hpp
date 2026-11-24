#pragma once
#include "animation/AnmTypes.hpp"

namespace th128::geometry {
struct RingVertex {
    float x=0,y=0,z=0,rhw=1;
    std::uint32_t color=0xffffffffu;
    float u=0,v=0;
};
std::optional<std::vector<RingVertex>> ring(const anm::State& state,const anm::SpriteInfo& sprite,float x,float y,float z);
}
