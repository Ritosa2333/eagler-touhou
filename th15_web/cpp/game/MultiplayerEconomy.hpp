#pragma once
#include "PlayerLife.hpp"
#include <array>
namespace th15 {
constexpr bool multiplayer_can_give_power(i32 power,i32 step)noexcept{return step>0&&power>=step&&power-step>=step;}
constexpr i32 multiplayer_chapter_threshold(unsigned seats)noexcept{return seats==3?2000000:seats==2?1500000:1000000;}
// TH11's shared-fragment accounting, with TH15's native extend thresholds.
inline std::array<bool,3> multiplayer_fragment(i32& fragments,i32& tier,i32 difficulty,const std::array<PlayerLifeSession*,3>& seats,unsigned count){
 const i32 threshold=difficulty==4?(tier<5?5:99999999):3;
 const bool extend=++fragments>=threshold;
 if(extend){fragments-=threshold;++tier;}
 std::array<bool,3> added{};
 for(unsigned s=0;s<count;++s){auto& stock=*seats[s];stock.life_pieces=fragments;if(extend&&stock.extra_lives<8){++stock.extra_lives;added[s]=true;}}
 return added;
}
}
