#pragma once
#include "FighterObject.hpp"
#include "RemiliaState.hpp"
namespace th075 {struct RemiliaObject:FighterObject {i16 counter=0,timer=0,angle=0,cycle=0,variant=0,auxiliary=0;};
void advance_remilia_object(RemiliaObject&,FighterActor&,FighterActor&,RemiliaState&,ReplayStreams&,FighterActionEvents&);}
