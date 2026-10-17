#pragma once
#include "Timer.hpp"
namespace th10 {
// Enemy bullets and lasers use the same 24-byte behavior command format.
struct ProjectileCommand {
    u32 arguments[4],type,concurrent;
    float floating(u32 index) const noexcept {float result;std::memcpy(&result,arguments+index,4);return result;}
    i32 integer(u32 index) const noexcept {return static_cast<i32>(arguments[index]);}
};
struct ProjectileModifier {
    Timer timer;
    u32 timer_flags;
    float first,second;
    Vec3 vector;
    i32 duration,count,iteration;
};
static_assert(sizeof(ProjectileCommand)==0x18);
static_assert(sizeof(ProjectileModifier)==0x34);
}
