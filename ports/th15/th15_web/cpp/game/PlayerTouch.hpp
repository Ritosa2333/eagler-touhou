#pragma once
#include "Types.hpp"
#include <cmath>
namespace th15 {
// A platform request for this tick, never part of the native player snapshot.
// Original keyboard/replay input leaves mode zero. Browser replays record the
// target in a separate USER block outside the encrypted original payload.
struct PlayerTouch {
 i32 mode=0;float x=0,y=0;
 bool valid()const noexcept{return mode==0||((mode==1||mode==2)&&std::isfinite(x)&&std::isfinite(y)&&std::abs(x)<=65536&&std::abs(y)<=65536);}
};
}
