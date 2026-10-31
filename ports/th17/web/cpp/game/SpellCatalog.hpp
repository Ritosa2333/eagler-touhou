#pragma once
#include "Types.hpp"
#include <array>
namespace th17 {
// Original spell numbering and the seven stage lists. These are data, shared
// by spell practice and Player Data, independently of executable addresses.
struct SpellCatalog {
 static constexpr i32 count=101;
 static constexpr std::array<i32,7> groups{{2,3,3,3,4,7,13}};
 static i32 rank(i32 spell)noexcept{return spell<0||spell>=count?-1:spell<88?spell%4:4;}
 static i32 rank_count(i32 difficulty)noexcept{return difficulty>=0&&difficulty<4?22:difficulty==4?13:0;}
 static std::array<i32,5> group(i32 stage,i32 number)noexcept{
  if(stage<0||stage>=7||number<0||number>=groups[stage])return {{-1,-1,-1,-1,-1}};
  constexpr i32 first[]={0,8,20,32,44,60,88};
  if(stage==6)return {{first[stage]+number,-1,0,0,0}};
  const i32 n=first[stage]+number*4;return {{n,n+1,n+2,n+3,-1}};
 }
};
}
