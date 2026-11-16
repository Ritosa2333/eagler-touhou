#pragma once

#include "Types.hpp"
#include <array>
#include <algorithm>

namespace th11 {
struct Th14CoopLoadout { u8 character=0, subtype=0; };
struct Th14CoopInput { u32 held=0; int touch_mode=0; float touch_x=0, touch_y=0; };
struct Th14CoopFrame { std::array<Th14CoopInput,3> seats{}; };
struct Th14Ghost {
    u32 random=0, clock=0;
    i32 dx=0,dy=0;
    void advance(i32& x,i32& y) noexcept {
        if(clock++%90==0){
            const auto next=[this](){
                random^=random<<13;random^=random>>17;random^=random<<5;return random;
            };
            dx=i32(next()%97)-48;dy=i32(next()%81)-40;
        }
        x=std::clamp(x+dx,-23552,23552);y=std::clamp(y+dy,4096,55296);
    }
};
constexpr float th14_dist_sq(Vec3 a,Vec3 b) noexcept {
    const float x=a.x-b.x,y=a.y-b.y;return x*x+y*y;
}
constexpr i32 th14_boss_damage(i32 amount,u32 live,u32& remainder) noexcept {
    if(amount<=0||live<=1)return amount;
    const auto scaled=u64(u32(amount))*(live>=3?9:12)+remainder;
    remainder=u32(scaled%20);return i32(scaled/20);
}
constexpr float th14_mallet_scale(float factor,u32 seats) noexcept {
    return factor*(seats>=3?.5f:seats==2?.67f:1.f);
}
constexpr i32 th14_coop_life_pieces=3;
constexpr i32 th14_coop_bomb_pieces(u32 seats) noexcept {return seats>=3?3:4;}
constexpr u32 th14_coop_shield_refund(u32 seats) noexcept {return seats>=3?1:seats==2?2:3;}
}
