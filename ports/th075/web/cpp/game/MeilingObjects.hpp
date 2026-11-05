#pragma once
#include "FighterObject.hpp"
#include "MeilingState.hpp"
namespace th075 {struct MeilingObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_meiling_object(MeilingObject&,FighterActor&,FighterActor&,MeilingState&,ReplayStreams&,FighterActionEvents&);}
