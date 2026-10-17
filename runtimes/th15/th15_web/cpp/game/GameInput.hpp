#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
struct GameInput {
 u32 held=0,previous=0,repeated=0,pressed=0,released=0,long_held=0;
 std::array<u32,32> repeat_age{},duration{};
 void calculate()noexcept;
 void update(u32 current)noexcept{previous=held;held=current;calculate();}
 void recording_update(u16 current,bool auto_focus)noexcept{update(current);if(auto_focus&&(held&1)&&duration[0]>=10)held|=8;}
};
}
