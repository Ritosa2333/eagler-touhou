#pragma once
#include "FighterObject.hpp"
#include "MarisaState.hpp"
namespace th075 {
struct MarisaObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_marisa_object(MarisaObject&,FighterActor& owner,FighterActor& opponent,MarisaState&,ReplayStreams&,FighterActionEvents&);
}
