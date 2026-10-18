#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
struct BossHealthSegment {float fraction=0;u32 color=0;};
struct BossHealthTrack {std::array<BossHealthSegment,10> segments{};u32 reserved=0;};
class BossHud {
public:
    std::array<BossHealthTrack,3> health{};i32 displayed_segments=0;
    bool segment(i32 boss,i32 index,float fraction,u32 color)noexcept{if(boss<0||boss>=3||index<0||index>=8)return false;health[u32(boss)].segments[u32(index)+2]={fraction,color};return true;}
};
}
