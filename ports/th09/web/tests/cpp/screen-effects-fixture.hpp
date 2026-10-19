#pragma once
#include "../../cpp/game/ScreenEffects.hpp"
namespace screen_effects_test {
using namespace th09;
inline u32 float_bits(float value){u32 bits;std::memcpy(&bits,&value,4);return bits;}
struct Fixture:ScreenEffectOutput {
    Rng random;ScreenEffects effects{random,*this};ScreenEffectContext context;
    Vec2 offsets[4]{};std::vector<std::array<u32,6>> rectangles;
    void shake(i32 side,float x,float y)override{offsets[side]={x,y};}
    void rectangle(float l,float t,float r,float b,u32 color,bool full)override{rectangles.push_back({float_bits(l),float_bits(t),float_bits(r),float_bits(b),color,u32(full)});}
};
}
