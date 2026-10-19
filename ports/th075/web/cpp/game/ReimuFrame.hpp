#pragma once
#include "FighterActionEvents.hpp"
namespace th075 {
struct ReimuFrameState {i16 blockEffectSent=0,farRangeFrames=0;};
void advance_reimu_frame(FighterActor&,const FighterActor& opponent,ReimuFrameState&,FighterActionEvents&);
}
