#pragma once
#include "core/shared/CircleCollision.hpp"
namespace kaikeidou {
inline bool omniverse_behavior_overlap(std::uint64_t ax,std::uint64_t ay,std::uint64_t bx,std::uint64_t by,std::uint64_t ar,std::uint64_t br,unsigned precision){
 for(auto value:{ax,ay,bx,by})if((value&0x7ff0000000000000ull)==0x7ff0000000000000ull)return false;
 return circles_overlap(ax,ay,bx,by,ar,br,precision);
}
}
